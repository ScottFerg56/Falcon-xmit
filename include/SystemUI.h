#pragma once
#include <lvgl.h>
#include "OMObject.h"
#include "lvexEvents.h"

class SystemUI : public EventClient
{
public:
    void Create(lv_obj_t* parent, Root& root);
    void PropertyUpdate(OMProperty* prop);
    static SystemUI& GetInstance() { return systemUI; }
    // Delete copy constructor and assignment operator to prevent copying singleton
    SystemUI(const SystemUI&) = delete;
    SystemUI& operator=(const SystemUI&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // Static member variable to hold the single instance
    static SystemUI systemUI;
    // private constructor for singleton
    SystemUI() { };
    lv_obj_t* grid;
    Root* pRoot;
};
