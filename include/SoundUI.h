#pragma once
#include <lvgl.h>
#include "OMObject.h"
#include "lvexEvents.h"

class SoundUI : public EventClient
{
public:
    void Create(lv_obj_t* parent, Root& root);
    void PropertyUpdate(OMProperty* prop);
    static SoundUI& GetInstance() { return soundUI; }
    // Delete copy constructor and assignment operator to prevent copying singleton
    SoundUI(const SoundUI&) = delete;
    SoundUI& operator=(const SoundUI&) = delete;
protected:
    virtual void EventFired(lv_event_t* e);
private:
    // Static member variable to hold the single instance
    static SoundUI soundUI;
    // private constructor for singleton
    SoundUI() { };
    lv_obj_t* gridSound = nullptr;
    OMObject* soundObj = nullptr;
};
