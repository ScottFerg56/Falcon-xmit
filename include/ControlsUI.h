#pragma once
#include <lvgl.h>
#include "lvexEvents.h"
#include "lvexColorPicker.h"

class ControlsUI : public EventClient
{
public:
    void Create(lv_obj_t* parent);
    void Command(String cmd);
    static ControlsUI& GetInstance() { return controlsUI; }
    // Delete copy constructor and assignment operator to prevent copying singleton
    ControlsUI(const ControlsUI&) = delete;
    ControlsUI& operator=(const ControlsUI&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // Static member variable to hold the single instance
    static ControlsUI controlsUI;
    // private constructor for singleton
    ControlsUI() { };
    uint8_t currentControl;
    lv_obj_t* grid;
    void controlRowShow(int row, bool show);
    void controlToggleExpansion(int row);
};
