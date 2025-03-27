#include "lvexPalette.h"
#include "FLogger.h"

lv_obj_t* lvexPalette::Create(lv_obj_t* parent, ColorClient* client)
{
    colorClient = client;
    static const char* map[] = {" ", " ", " ", " ", " ", " ", " ", " ", " ", "\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", "\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", ""};

    lv_obj_t* btnm = lv_buttonmatrix_create(parent);
    lv_buttonmatrix_set_map(btnm, map);

    lv_obj_set_style_pad_all(btnm, 0, 0);
    lv_obj_set_style_pad_gap(btnm, 0, 0);
    lv_obj_set_style_clip_corner(btnm, true, 0);
    lv_obj_set_style_radius(btnm, 0, 0);
    lv_obj_set_style_border_width(btnm, 0, 0);

    lv_obj_set_style_radius(btnm, 0, LV_PART_ITEMS);
    lv_obj_set_style_border_width(btnm, 2, LV_PART_ITEMS);
    lv_obj_set_style_border_color(btnm, lv_palette_main(LV_PALETTE_GREY), LV_PART_ITEMS);

    // Allow selecting on one number at time
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_one_checked(btnm, true);
    lv_buttonmatrix_set_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKED);
    // add draw event
    AddEvent(btnm, LV_EVENT_DRAW_TASK_ADDED);
    lv_obj_add_flag(btnm, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    // add click event
    AddEvent(btnm, LV_EVENT_CLICKED);
    lv_obj_set_size(btnm, 360, 120);
    return btnm;
}

void lvexPalette::EventFired(lv_event_t * e)
{
    lv_obj_t* obj = lv_event_get_target_obj(e);
    auto code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        uint32_t i = lv_buttonmatrix_get_selected_button(obj);
        if (i > 26)
            return;
        uint8_t r = i % 3;
        uint8_t g = (i / 3) % 3;
        uint8_t b = i / 9;
        static uint8_t m[] = { 0, 0x80, 0xFF };
        auto color = lv_color_make(m[r], m[g], m[b]);
        colorClient->ColorChanged(color);
    }
    else if (code == LV_EVENT_DRAW_TASK_ADDED)
    {
        lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
        lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
        /*When the button matrix draws the buttons...*/
        if(base_dsc->part == LV_PART_ITEMS)
        {
            auto i = base_dsc->id1;
            uint8_t r = i % 3;
            uint8_t g = (i / 3) % 3;
            uint8_t b = i / 9;
            static uint8_t m[] = { 0, 0x80, 0xFF };
            auto color = lv_color_make(m[r], m[g], m[b]);
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if (fill_draw_dsc)
            {
                fill_draw_dsc->radius = 0;
                fill_draw_dsc->color = color;
            }
        }
    }
}
