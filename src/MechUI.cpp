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
    static int32_t col_dscA[] = { 160, LV_GRID_CONTENT, 80, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t row_dscA[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };

    // Create a grid container
    auto grid = gridRect = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dscA, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dscA, 0);
    lv_obj_set_size(grid, 800, 180);
    lv_obj_align(grid, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 20, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

    // Rectenna
    auto lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Rectenna");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_30, 0);
    // sweep switch and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Sweep");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "off");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    auto sw = lv_switch_create(grid);
    lv_obj_set_size(sw, 80, 40);
    lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_id(sw, (void*)(swSweep));
    AddEvent(sw, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "on");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 4, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // speed slider and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Speed");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblSpeedRect);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    auto slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 2, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(slider, (void*)slSpeedRect);
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);
    // position slider and label
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Position");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblPosition);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 2, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_id(slider, (void*)slPosition);
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);

    //
    // Ramp grid
    //
    static int32_t col_dscR[] = { 160, LV_GRID_CONTENT, 80, 80, 80, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t row_dscR[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };

    // Create a grid container
    grid = gridRamp = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dscR, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dscR, 0);
    lv_obj_set_size(grid, 800, 140);
    lv_obj_align(grid, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 20, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

    // Ramp
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Ramp");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_30, 0);

    // Position up/stop/dn buttons and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Position");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // UP
    auto btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_id(btn, (void*)btnUp);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_CHECKED);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_UP);
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // STOP
    btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_id(btn, (void*)btnStop);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_AMBER), LV_STATE_CHECKED);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_STOP);
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // DOWN
    btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_id(btn, (void*)btnDown);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_DOWN);
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // speed slider and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Speed");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblSpeedRamp);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 3, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(slider, (void*)slSpeedRamp);
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);

    SendCmd(String("?a"));
    SendCmd(String("?r"));
}

void MechUI::MutexRampPosition(int idChecked)
{
    for (int id = btnUp; id <= btnDown; id++)
    {
        auto btn = lv_obj_get_child_by_id(gridRamp, (void*)id);
        auto checked = lv_obj_has_state(btn, LV_STATE_CHECKED);
        if (checked)
        {
            if (id == idChecked)
                continue;
            lv_obj_set_state(btn, LV_STATE_CHECKED, false);
        }
        else if (id == idChecked)
        {
            lv_obj_set_state(btn, LV_STATE_CHECKED, true);
        }
    }
}

void MechUI::EventFired(lv_event_t * e)
{
    auto obj = lv_event_get_target_obj(e);
    auto id = (int)lv_obj_get_id(obj);
    auto code = lv_event_get_code(e);
    switch (code)
    {
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
            case btnUp:
                {
                    MutexRampPosition(btnUp);   // retract
                    SendCmd(String("=rsR"));
                }
                break;
            case btnStop:
                {
                    MutexRampPosition(btnStop); // stop
                    SendCmd(String("=rsS"));
                }
                break;
            case btnDown:
                {
                    MutexRampPosition(btnDown);
                    SendCmd(String("=rsE"));    // extend
                }
                break;
            case slSpeedRamp:
                {
                    auto speed = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridRamp, (void*)lblSpeedRamp);
                    String spd(speed);
                    lv_label_set_text(lbl, spd.c_str());
                    SendCmd("=rv" + spd);
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
                    RectInitFlags |= 0b0001;
                    auto sw = lv_obj_get_child_by_id(gridRect, (void*)swSweep);
                    if (sw)
                        lv_obj_set_state(sw, LV_STATE_CHECKED, cmd[3] == '1');
                }
                break;
            case 'v':   // sweep speed
                {
                    RectInitFlags |= 0b0010;
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
                    RectInitFlags |= 0b0100;
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
    else
    {
        if (RampInitComplete())
            return;
        // Ramp
        switch (cmd[2])
        {
            case 's':   // state
                {
                    RampInitFlags |= 0b001;
                    switch (cmd[3])
                    {
                    case 'R':       // Retracted
                    case 'r':       // retracting
                        MutexRampPosition(btnUp);
                        break;
                    case 'S':       // Stopped
                        MutexRampPosition(btnStop);
                        break;
                    case 'E':       // Extended
                    case 'e':       // extending
                        MutexRampPosition(btnDown);
                        break;
                    }
                }
                break;
            case 'v':   // Ramp speed
                {
                    RampInitFlags |= 0b010;
                    auto speed = cmd.substring(3).toInt();
                    auto slider = lv_obj_get_child_by_id(gridRamp, (void*)slSpeedRamp);
                    if (slider)
                        lv_slider_set_value(slider, speed, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridRamp, (void*)lblSpeedRamp);
                    lv_label_set_text(lbl, String(speed).c_str());
                }
                break;
        }
    }
}

MechUI MechUI::mechUI;
