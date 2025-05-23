#pragma once
#include "xmit.h"
#include <lvgl.h>
#include "lvexEvents.h"
#include "lvexColorPickerHSV.h"
#include "lvexPalette.h"

class lvexColorPicker : public ColorClient, public EventClient
{
public:
    static void Show(OMObject* light);
    void Create();
    void PropertyUpdate(OMProperty* prop);
    // as a ColorClient, we receive color change events from the HSV picker and palette
    virtual void ColorChanged(lv_color_t color);
    static lvexColorPicker& GetInstance() { return colorPicker; }
    // Delete copy constructor and assignment operator to prevent copying singleton
    lvexColorPicker(const lvexColorPicker&) = delete;
    lvexColorPicker& operator=(const lvexColorPicker&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // Static member variable to hold the single instance
    static lvexColorPicker colorPicker;
    // private constructor for singleton
    lvexColorPicker() { };
    OMObject* Light = nullptr;
    lv_obj_t* window;
    lv_obj_t* lblTitle;
    lvexColorPickerHSV pickerHSV;
    lvexPalette palette;
    lv_obj_t* panelSample;
    uint8_t ColorInx = 0;
    lv_color_t Colors[2];
    void setColor(uint8_t inx, lv_color_t color);
    void setSpeedLabel(int speed);
    //lv_color_t getColor() { return Colors[ColorInx]; }
};
