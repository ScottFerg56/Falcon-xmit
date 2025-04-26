#include <lvgl.h>
#if LV_USE_TFT_ESPI
#define DISABLE_ALL_LIBRARY_WARNINGS
#endif
#include "xmit.h"
#include "lvexColorPicker.h"
#include "LightsUI.h"
#include "MechUI.h"
#include "SoundUI.h"
#include "SystemUI.h"

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
#include "ESPNAgent.h"
#include "Debug.h"

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

class LightConnector : public OMConnector
{
public:
    void Init(OMObject* obj) override { }
    void Push(OMObject* obj, OMProperty* prop) override
    {
        LightsUI::GetInstance().PropertyUpdate(prop);
        lvexColorPicker::GetInstance().PropertyUpdate(prop);
    }
    void Pull(OMObject* obj, OMProperty* prop) override { }
};

class MechConnector : public OMConnector
{
public:
    void Init(OMObject* obj) override { }
    void Push(OMObject* obj, OMProperty* prop) override
    {
        MechUI::GetInstance().PropertyUpdate(prop);
    }
    void Pull(OMObject* obj, OMProperty* prop) override { }
};

class SoundConnector : public OMConnector
{
public:
    void Init(OMObject* obj) override { }
    void Push(OMObject* obj, OMProperty* prop) override
    {
        SoundUI::GetInstance().PropertyUpdate(prop);
    }
    void Pull(OMObject* obj, OMProperty* prop) override { }
};

LightConnector LightConn;
MechConnector MechConn;
SoundConnector SoundConn;

class RootConnector : public OMConnector
{
public:
    void Init(OMObject *obj) override {}
    void Push(OMObject *obj, OMProperty *prop) override
    {
        auto id = prop->Id;
        switch (id)
        {
        case 'f':   // free space
            SystemUI::GetInstance().PropertyUpdate(prop);
            break;
        }
    }
    void Pull(OMObject *obj, OMProperty *prop) override {}
};

RootConnector RootConn;

#define GroupConn LightConn
#define RectennaConn MechConn
#define RampConn MechConn
#include "OMDef.h"
  
Root root(false, 'R', "Root", &RootConn);  // this is the controller, not the device
ESPNAgent agent(&SD, &root);

void setup()
{
    Serial.begin(115200);
    delay(4000);
    FLogger::setLogLevel(FLOG_VERBOSE);
    flogv("Falcon xmit " __DATE__ " " __TIME__);
    // FLogger::setPrinter(flog_printer);

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
    }

    agent.Setup(peerMacAddress);
    Debug::GetInstance().Setup();

    root.AddObjects(Objects);
    root.AddProperties(RootProps);
    // UI requires object model to be setup first
    createUI(root);
    root.Setup(&agent);

    flogv("Setup Done");
}

void loop(void)
{
    lv_timer_handler(); // let the GUI work
    delay(5);

    Debug::GetInstance().Run();
    agent.Run();
}
