#include <lvgl.h>
#if LV_USE_TFT_ESPI
#define DISABLE_ALL_LIBRARY_WARNINGS
#endif
#include "xmit.h"
#include "lvexColorPicker.h"
#include "LightsUI.h"
#include "MechUI.h"

#define CALIBRATING_TS 0
#include <Elecrow-5in-Display.h>
#include "UI.h"
//#include "..\.pio\libdeps\adafruit_feather_esp32_v2\lvgl\src\core\lv_obj_private.h"
#include "FLogger.h"
#include <esp_now.h>
#include <WiFi.h>
#include <queue>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_MOSI 11
#define SD_MISO 13
#define SD_CLK 12
#define SD_CS 10

SPIClass spi = SPIClass();

// #include <examples/lv_examples.h>
// #include <demos/lv_demos.h>

#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
#endif

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}

// our Mac Address = {0xD8, 0x3B, 0xDA, 0x87, 0x52, 0x58}

uint8_t peerMacAddress[] = {0xE8, 0x9F, 0x6D, 0x20, 0x7D, 0x28};

esp_now_peer_info_t PeerInfo;
bool DataSent = false;
bool DataConnected = false;
bool lockInput = false;
std::queue<String> inputCommands;

struct FilePacketHdr
{
    char        tag;
    uint32_t    packetNum;
};

const uint16_t FilePacketSize = 240;
uint32_t FilePacketCount = 0;
uint32_t FilePacketNumber = 0;
String FilePath;
bool FilePacketSend = false;
uint8_t FilePacketSendErrorCount = 0;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    DataSent = false;
    if (status != ESP_NOW_SEND_SUCCESS)
    {
        flogw("Delivery Fail");
        DataConnected = false;

        if (FilePacketCount != 0)
        {
            if (++FilePacketSendErrorCount > 2)
            {
                // retries failed
                FilePacketSend = false;
                floge("file transfer failed");
            }
            else
            {
                // retry
                FilePacketSend = true;
                floge("file transfer retry %d", FilePacketSendErrorCount);
            }
        }
    }
    else if (FilePacketCount != 0)
    {
        // flogv("File transfer packet %lu suceeded", FilePacketNumber);
    }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *pData, int len)
{
    DataConnected = true;
    if (len > 0)
    {
        String data(pData, len);
        if (data[0] == '=')
        {
            lockInput = true;
            while (data.length() > 0)
            {
                String cmd;
                auto inx = data.indexOf(';');
                if (inx == -1)
                {
                    cmd = data;
                    data.clear();
                }
                else
                {
                    cmd = data.substring(0, inx);
                    data.remove(0, inx+1);
                }
                inputCommands.push(cmd);
            }
            lockInput = false;
        }
        else if (data[0] == '2')
        {
            // ACK received, OK the transmission of next packet
            ++FilePacketNumber;
            FilePacketSend = true;
        }
        else if (data[0] == '3')
        {
            // termination received
            FilePacketCount = 0;
            FilePacketNumber = 0;
            FilePacketSend = false;
            FilePath = "";
        }
        else
        {
            Serial.print(data.c_str());
        }
    }
}

bool SendData(const uint8_t *pData, int len)
{
    if (DataSent)
    {
        // data sent but not acknowledged; wait it out a while
        unsigned long ms = millis();
        while (DataSent)
        {
            if (millis() - ms > 100)
                DataSent = false;
        }
    }
    DataSent = true;
    esp_err_t result = esp_now_send(PeerInfo.peer_addr, pData, len);
    if (result != ESP_OK)
    {
        DataConnected = false;   // don't get caught up in infinite logging loop!!
        floge("Error sending data: %s", esp_err_to_name(result));
        DataSent = false;
        return false;
    }
    return true;
}

void SendCmd(String cmd)
{
    flogv("cmd send: [%s]", cmd);
    if (FilePacketCount > 0)
    {
        floge("cmd send skipped while file transfer in progress");
        return;
    }
    SendData((uint8_t*)cmd.c_str(), cmd.length());
}

void StartFileTransfer(String filePath)
{
    if (filePath.length() > 31)
    {
        floge("Filename length must be < 32");
        return;
    }
    File file = SD.open(filePath.c_str(), FILE_READ);
    if (!file)
    {
        floge("File open failed");
        return;
    }
    uint32_t fileSize = file.size();
    file.close();
    FilePacketSendErrorCount = 0;
    // packet 0 is the start packet with filename but no data
    FilePacketNumber = 0;
    FilePacketCount = fileSize / FilePacketSize;
    if (fileSize % FilePacketSize != 0)
    FilePacketCount++;
    FilePath = filePath;
    String fileName(pathToFileName(filePath.c_str()));
    flogv("Starting transfer: %s  file size: %lu  #packets: %lu", FilePath.c_str(), fileSize, FilePacketCount);
    FilePacketHdr hdr = { '1', FilePacketCount };
    uint8_t messageArray[sizeof(hdr) + fileName.length() + 1];
    memcpy(messageArray, &hdr, sizeof(hdr));
    strcpy((char*)(messageArray + sizeof(hdr)), fileName.c_str());
    SendData(messageArray, sizeof(messageArray));
}

void SendNextFilePacket()
{
    FilePacketSend = false;

    // if got to AFTER the last package
    if (FilePacketNumber > FilePacketCount)
    {
        FilePacketNumber = 0;
        FilePacketCount = 0;
        FilePath = "";
        flogv("File transfer complete");
        return;
    }

    File file = SD.open(FilePath.c_str(), FILE_READ);
    if (!file)
    {
        FilePacketNumber = 0;
        FilePacketCount = 0;
        FilePath = "";
        floge("File open failed");
        return;
    }

    uint32_t packetDataSize = FilePacketSize;
    if (FilePacketNumber == FilePacketCount)
    {
        // last packet - adjust the size
        packetDataSize = file.size() - ((FilePacketCount - 1) * FilePacketSize);
        // flogv("last file packet data size: %lu", packetDataSize);
    }

    FilePacketHdr hdr = { '2', FilePacketNumber };
    uint8_t messageArray[sizeof(hdr) + packetDataSize];
    memcpy(messageArray, &hdr, sizeof(hdr));

    // we'll increment the FilePacketNumber in OnDataSent when packet transmission is confirmed

    file.seek((FilePacketNumber - 1) * FilePacketSize);
    file.readBytes((char*)messageArray + sizeof(hdr), packetDataSize);
    file.close();
    SendData(messageArray, sizeof(messageArray));
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);
  
    File root = fs.open(dirname);
    if(!root){
      Serial.println("Failed to open directory");
      return;
    }
    if(!root.isDirectory()){
      Serial.println("Not a directory");
      return;
    }
  
    File file = root.openNextFile();
    while(file){
      if(file.isDirectory()){
        Serial.print("  DIR : ");
        Serial.println(file.name());
        if(levels){
          listDir(fs, file.name(), levels -1);
        }
      } else {
        Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("  SIZE: ");
        Serial.println(file.size());
      }
      file = root.openNextFile();
    }
  }
  
void setup()
{
    Serial.begin(115200);
    delay(4000);
    FLogger::setLogLevel(FLOG_VERBOSE);
    // FLogger::setPrinter(flog_printer);

    flogv("Millennium Falcon xmit");

    lv_init();

    /* register print function for debugging */
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print );
#endif

    /*Set a tick source so that LVGL will know how much time elapsed. */    
    lv_tick_set_cb(my_tick);

    flogv("init display start");

    if (!setupElecrow_5in_Display())
        flogf("Display init FAILED");

    flogv("init display done");

    lv_display_set_rotation(lv_display_get_default(), LV_DISPLAY_ROTATION_0);  // 0:90:180:270

    flogi("WIFI init");
    if (!WiFi.mode(WIFI_STA))
        flogf("%s FAILED", "WIFI init");

    flogi("MAC addr: %s", WiFi.macAddress().c_str());

    flogi("ESP_NOW init");
    if (esp_now_init() != ESP_OK)
        flogf("%s FAILED", "ESP_NOW init");

    //flogi("ESP_NOW peer add");
    memset(&PeerInfo, 0, sizeof(PeerInfo));
    memcpy(PeerInfo.peer_addr, peerMacAddress, sizeof(PeerInfo.peer_addr));
    //PeerInfo.channel = 0;
    //PeerInfo.encrypt = false;
    if (esp_now_add_peer(&PeerInfo) != ESP_OK)
        flogf("%s FAILED", "ESP_NOW peer add");

    //flogi("ESP_NOW send cb");
    esp_now_register_send_cb(OnDataSent);

    //flogi("ESP_NOW recv cb");
    esp_now_register_recv_cb(OnDataRecv);

    flogi("ESP_NOW init complete");

    spi.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);

    if (!SD.begin(SD_CS, spi, 80000000))
    {
        floge("Card Mount Failed");
    }
    else
    {
        uint8_t cardType = SD.cardType();
        if (cardType == CARD_NONE)
        {
            flogi("No SD card attached");
        }
        else
        {
            listDir(SD, "/", 0);
            Serial.printf("Total space: %llu\n", SD.totalBytes());
            Serial.printf("Used space: %llu\n", SD.usedBytes());
        }
    }

    createUI();

    flogv("Setup Done");
}

void loop(void)
{
    lv_timer_handler(); // let the GUI work
    delay(5);

    // check to send next file transfer packet
    if (FilePacketSend)
        SendNextFilePacket();

    if (!inputCommands.empty() && !lockInput)
    {
        auto cmd = inputCommands.front();
        inputCommands.pop();
        flogv("data received: [%s]", cmd.c_str());
        lvexColorPicker::GetInstance().Command(cmd);
        LightsUI::GetInstance().Command(cmd);
        MechUI::GetInstance().Command(cmd);
    }
    if (Serial.available())
    {
        static String cmd;
        while (Serial.available())
        {
            char c = Serial.read();
            // echo back to terminal
            if (c == '\n')
                continue;
            Serial.write(c);
            if (c == '\r')
                Serial.write('\n');
            if (c < ' ')
            {
                if (cmd.length() > 0)
                {
                    if (cmd[0] == '=')
                    {
                        SendCmd(cmd);
                    }
                    else if (cmd[0] == 'x')
                    {
                        StartFileTransfer("/Sad R2D2.mp3");
                    }
                }
                cmd.clear();
            }
            else
            {
                cmd.concat(c);
            }
        }
    }
}
