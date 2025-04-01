#pragma once
#include "xmit.h"
#include <lvgl.h>
#include "lvexEvents.h"
#include "lvexColorPickerHSV.h"
#include "lvexPalette.h"

class lvexColorPicker : public ColorClient, public EventClient
{
public:
    static void Show(const char* title, const char* cmdPath);
    // as a ColorClient, we receive color change events from the HSV picker and palette
    virtual void ColorChanged(lv_color_t color);
    static lvexColorPicker& GetInstance() { return colorPicker; }
    void Command(String cmd);
    // Delete copy constructor and assignment operator to prevent copying
    lvexColorPicker(const lvexColorPicker&) = delete;
    lvexColorPicker& operator=(const lvexColorPicker&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // singleton instance
    static lvexColorPicker colorPicker;
    // Static member variable to hold the single instance
    lvexColorPicker() { };
    void Create();
    String CmdPath;
    lv_obj_t* window;
    lv_obj_t* lblTitle;
    lvexColorPickerHSV pickerHSV;
    lvexPalette palette;
    lv_obj_t* panelSample;
    uint8_t ColorInx = 0;
    lv_color_t Colors[2];
    uint8_t InitFlags = 0;
    bool InitComplete() { return InitFlags == 0b0111111; }
    void setColor(uint8_t inx, lv_color_t color);
    //lv_color_t getColor() { return Colors[ColorInx]; }
};
