#pragma once
#include <lvgl.h>
#include "lvexEvents.h"
#include "lvexColorPicker.h"

class LightsUI : public EventClient
{
public:
    void Create(lv_obj_t* parent, Root& root);
    void PropertyUpdate(OMProperty* prop);
    static LightsUI& GetInstance() { return lightsUI; }
    // Delete copy constructor and assignment operator to prevent copying singleton
    LightsUI(const LightsUI&) = delete;
    LightsUI& operator=(const LightsUI&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // Static member variable to hold the single instance
    static LightsUI lightsUI;
    // private constructor for singleton
    LightsUI() { };
    lv_obj_t* grid;
    void CreateRows(OMObject* light, int32_t& row);
    void lightRowShow(OMObject* light, bool show);
    void lightToggleExpansion(OMObject* light);
};
