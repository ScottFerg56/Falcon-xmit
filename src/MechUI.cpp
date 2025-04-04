#include <lvgl.h>
#include "xmit.h"
#include "MechUI.h"
#include "FLogger.h"

enum controlIds
{
    noZero,
    swSweep,
    lblSpeedRect,
    slSpeedRect,
    lblPosition,
    slPosition,
    swUpDown,
    btnUp,
    btnStop,
    btnDown,
    lblSpeedRamp,
    slSpeedRamp,
};

void MechUI::Create(lv_obj_t* parent)
{
    //
    // Rectenna grid
    //
    static int32_t col_dscA[] = { LV_GRID_CONTENT, 80, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t row_dscA[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };

    // Create a grid container
    auto grid = gridRect = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dscA, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dscA, 0);
    lv_obj_set_size(grid, 800, 240);
    lv_obj_set_align(grid, LV_ALIGN_TOP_MID);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 20, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

    // Rectenna
    auto lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Rectenna");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 3, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_30, 0);
    // sweep switch and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Sweep");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "off");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    auto sw = lv_switch_create(grid);
    lv_obj_set_size(sw, 80, 40);
    lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(sw, (void*)(swSweep));
    AddEvent(sw, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "on");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    // speed slider and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Speed");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblSpeedRect);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    auto slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 500, 30);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 2, 2, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_id(slider, (void*)slSpeedRect);
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);
    // position slider and label
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Position");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblPosition);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 500, 30);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 2, 2, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_id(slider, (void*)slPosition);
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);

    SendCmd(String("?a"));
    SendCmd(String("?r"));
}

void MechUI::EventFired(lv_event_t * e)
{
    auto obj = lv_event_get_target_obj(e);
    auto id = (int)lv_obj_get_id(obj);
    auto code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        // switch (id)
        // {
        // }
        break;
    case LV_EVENT_VALUE_CHANGED:
        switch (id)
        {
            case swSweep:
                {
                    auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
                    SendCmd(String("=as") + (checked ? '1' : '0'));
                }
                break;
            case slSpeedRect:
                {
                    auto speed = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblSpeedRect);
                    String spd(speed);
                    lv_label_set_text(lbl, spd.c_str());
                    SendCmd("=av" + spd);
                }
                break;
            case slPosition:
                {
                    auto position = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblPosition);
                    String pos(position);
                    lv_label_set_text(lbl, pos.c_str());
                    SendCmd("=ap" + pos);
                    // setting a position turns sweep off
                    // so reflect that here since we're blocking updates from rcvr
                    auto sw = lv_obj_get_child_by_id(gridRect, (void*)swSweep);
                    if (sw)
                        lv_obj_set_state(sw, LV_STATE_CHECKED, false);
                }
                break;
        }
    }
}

void MechUI::Command(String cmd)
{
    if (cmd.length() == 0 || cmd[0] != '=')
        return;
    int inx = 1;
    if (cmd[1] == 'a')
    {
        if (RectInitComplete())
            return;
        // Rectenna
        switch (cmd[2])
        {
            case 's':   // sweep on/off
                {
                    RectInitFlags != 0b0001;
                    auto sw = lv_obj_get_child_by_id(gridRect, (void*)swSweep);
                    if (sw)
                        lv_obj_set_state(sw, LV_STATE_CHECKED, cmd[3] == '1');
                }
                break;
            case 'v':   // sweep speed
                {
                    RectInitFlags != 0b0010;
                    auto speed = cmd.substring(3).toInt();
                    auto slider = lv_obj_get_child_by_id(gridRect, (void*)slSpeedRect);
                    if (slider)
                        lv_slider_set_value(slider, speed, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblSpeedRect);
                    lv_label_set_text(lbl, String(speed).c_str());
                }
                break;
            case 'p':   // position
                {
                    RectInitFlags != 0b0100;
                    auto position = cmd.substring(3).toInt();
                    auto slider = lv_obj_get_child_by_id(gridRect, (void*)slPosition);
                    if (slider)
                        lv_slider_set_value(slider, position, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblPosition);
                    lv_label_set_text(lbl, String(position).c_str());
                }
                break;
        }
    }
}

MechUI MechUI::mechUI;
