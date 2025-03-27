#pragma once
#include <lvgl.h>

class EventClient
{
protected:
    virtual void EventFired(lv_event_t* e) = 0;
    // this helper causes the client to implicitly convert the client
    // to EventClient* to make sure we get the right vtable in multiple inheritance cases
    void AddEvent(lv_obj_t* obj, lv_event_code_t filter)
    {
        lv_obj_add_event_cb(obj, EventHandler, filter, this);
    }
private:
    static void EventHandler(lv_event_t* e)
    {
        auto client = (EventClient*)lv_event_get_user_data(e);
        // auto code = lv_event_get_code(e);
        // auto obj = lv_event_get_target_obj(e);
        // auto id = (int)lv_obj_get_id(obj);
        // flogv("code: %d  id: %d  client: %X", code, id, (uint32_t)client);
        client->EventFired(e);
    }
};

// clients who want notification from color sources
class ColorClient
{
public:
    virtual void ColorChanged(lv_color_t color) = 0;
};
