#pragma once
#include <lvgl.h>
#include "lvexEvents.h"

class lvexPalette : public EventClient
{
public:
    lv_obj_t* Create(lv_obj_t* parent, ColorClient* client);
protected:
    virtual void EventFired(lv_event_t* e);
private:
    ColorClient* colorClient;
};
