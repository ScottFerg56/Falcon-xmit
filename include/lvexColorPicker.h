#pragma once
#include <lvgl.h>
#include "lvexEvents.h"
#include "lvexColorPickerHSV.h"
#include "lvexPalette.h"

class lvexColorPicker : public ColorClient, public EventClient
{
public:
    static void Show(lv_color_t color, const char*title, ColorClient* client);
    // as a ColorClient, we receive color change events from the HSV picker and palette
    virtual void ColorChanged(lv_color_t color);
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
    lv_obj_t* window;
    lv_obj_t* lblTitle;
    lv_color_t currentColor;
    ColorClient* colorClient;
    lvexColorPickerHSV pickerHSV;
    lvexPalette palette;
    lv_obj_t* panelSample;
    void setColor(lv_color_t color);
    lv_color_t getColor() { return currentColor; }
};
