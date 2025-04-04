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

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    DataSent = false;
    if (status != ESP_NOW_SEND_SUCCESS)
    {
        flogw("Delivery Fail");
        DataConnected = false;
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
    SendData((uint8_t*)cmd.c_str(), cmd.length());
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
                    SendCmd(cmd);
                cmd.clear();
            }
            else
            {
                cmd.concat(c);
            }
        }
    }
}
