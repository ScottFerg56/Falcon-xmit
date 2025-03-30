#include <lvgl.h>
#if LV_USE_TFT_ESPI
#define DISABLE_ALL_LIBRARY_WARNINGS
#endif
#include "xmit.h"
#define CALIBRATING_TS 0
#include <Elecrow-5in-Display.h>
#include "UI.h"
//#include "..\.pio\libdeps\adafruit_feather_esp32_v2\lvgl\src\core\lv_obj_private.h"
#include "FLogger.h"
#include <esp_now.h>
#include <WiFi.h>

// void lv_obj_set_id(lv_obj_t * obj, void * id)
// {
//     obj->id = id;
// }

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
    String cmd(pData, len);
    if (cmd[0] == '=')
        flogv("data received: [%s]", cmd.c_str());
    else
        Serial.print(cmd.c_str());
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
    SendData((uint8_t*)cmd.c_str(), cmd.length());
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

    flogv("createUI start");

    createUI();

    flogv("createUI done");

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

    flogv("Setup Done");
}

void loop(void)
{
    lv_timer_handler(); // let the GUI work
    delay(5);
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
