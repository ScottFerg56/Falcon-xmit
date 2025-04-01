#pragma once
#include <lvgl.h>
#include "lvexEvents.h"

typedef struct
{
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} fhsv;

class lvexColorPickerHSV : public EventClient
{
public:
    lv_obj_t* Create(lv_obj_t* parent, ColorClient* client);
    void setColor(lv_color_t color);
    bool isBusy();
protected:
    virtual void EventFired(lv_event_t* e);
private:
    lv_color_t RGB;
    fhsv HSV;
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
    void setSliderValue(HSVinx inx);
    void setSliderUI(HSVinx inx);
    void setSliderColor(HSVinx inx);
    bool suppressColorChanged;
};
