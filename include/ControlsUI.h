#pragma once
#include <lvgl.h>
#include "lvexEvents.h"
#include "lvexColorPicker.h"

class ControlsUI : public ColorClient, public EventClient
{
public:
    lv_obj_t* Create(lv_obj_t* parent);
    void setColorPicker(lvexColorPicker* picker);
    // as a ColorClient, we receive color change events
    virtual void ColorChanged(lv_color_t color);
    virtual void EventFired(lv_event_t* e);
private:
    lvexColorPicker* colorPicker;
    uint8_t currentControl;
    lv_obj_t* grid;
    void controlRowShow(int row, bool show);
    void controlToggleExpansion(int row);
};

