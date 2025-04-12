#pragma once
#include <lvgl.h>
#include "lvexEvents.h"

class MechUI : public EventClient
{
public:
    void Create(lv_obj_t* parent, Root& root);
    void PropertyUpdate(OMProperty* prop);
    static MechUI& GetInstance() { return mechUI; }
    // Delete copy constructor and assignment operator to prevent copying singleton
    MechUI(const MechUI&) = delete;
    MechUI& operator=(const MechUI&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // Static member variable to hold the single instance
    static MechUI mechUI;
    // private constructor for singleton
    MechUI() { };
    lv_obj_t* gridRect;
    lv_obj_t* gridRamp;
    OMObject* rectObj;
    OMObject* rampObj;
    void MutexRampPosition(int idChecked);
};
