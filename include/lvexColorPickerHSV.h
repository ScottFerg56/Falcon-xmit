#pragma once
#include <lvgl.h>
#include "lvexEvents.h"

class lvexColorPickerHSV : public EventClient
{
public:
    lv_obj_t* Create(lv_obj_t* parent, ColorClient* client);
    void setColor(lv_color_t color);
protected:
    virtual void EventFired(lv_event_t* e);
private:
    lv_color_t currentColor;
    enum HSVinx
    {
        hueInx,
        satInx,
        valInx
    };
    ColorClient* colorClient;
    lv_obj_t* grid;
    lv_obj_t* hsvSliders[3];
    void ChangeColor();
    uint16_t getHue();
    void setHue(uint16_t h);
    uint8_t  getSat();
    void setSat(uint8_t s);
    uint8_t  getVal();
    void setVal(uint8_t v);
    void setSliderColor(HSVinx inx);
    bool suppressColorChanged;
};
