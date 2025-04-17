#include <lvgl.h>
#include "xmit.h"
#include "MechUI.h"
#include "FLogger.h"

MechUI MechUI::mechUI;

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

void MechUI::Create(lv_obj_t* parent, Root& root)
{
    rectObj = root.GetObject('a');
    rampObj = root.GetObject('r');
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
    lv_label_set_text(lbl, rectObj->Name);
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_30, 0);
    // sweep switch and labels
    auto prop = rectObj->GetProperty('s');
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, prop->Name);
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "off");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    auto sw = lv_switch_create(grid);
    lv_obj_set_size(sw, 80, 40);
    lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_id(sw, (void*)(swSweep));
    lv_obj_set_user_data(sw, prop);
    prop->Data = sw;
    AddEvent(sw, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "on");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 4, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // speed slider and labels
    prop = rectObj->GetProperty('v');
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, prop->Name);
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblSpeedRect);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    auto slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, ((OMPropertyLong*)prop)->Min, ((OMPropertyLong*)prop)->Max);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 2, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(slider, (void*)slSpeedRect);
    lv_obj_set_user_data(slider, prop);
    prop->Data = slider;
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);
    // position slider and label
    prop = rectObj->GetProperty('p');
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, prop->Name);
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblPosition);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, ((OMPropertyLong*)prop)->Min, ((OMPropertyLong*)prop)->Max);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 2, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_id(slider, (void*)slPosition);
    lv_obj_set_user_data(slider, prop);
    prop->Data = slider;
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
    lv_label_set_text(lbl, rampObj->Name);
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_30, 0);

    // Position up/stop/dn buttons and labels
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Position");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // state - UP
    prop = rampObj->GetProperty('s');
    auto btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_id(btn, (void*)btnUp);
    lv_obj_set_user_data(btn, prop);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_CHECKED);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_UP);
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // state - STOP
    btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_id(btn, (void*)btnStop);
    lv_obj_set_user_data(btn, prop);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_AMBER), LV_STATE_CHECKED);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_STOP);
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // state - DOWN
    btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_id(btn, (void*)btnDown);
    lv_obj_set_user_data(btn, prop);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_DOWN);
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // speed slider and labels
    prop = rampObj->GetProperty('v');
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, prop->Name);
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblSpeedRamp);
    lv_label_set_text(lbl, "100");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, ((OMPropertyLong*)prop)->Min, ((OMPropertyLong*)prop)->Max);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 3, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(slider, (void*)slSpeedRamp);
    lv_obj_set_user_data(slider, prop);
    prop->Data = slider;
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);
    // UI states will be filled in after setup requests values from the device
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
    auto prop = (OMProperty*)lv_obj_get_user_data(obj);
    if (!prop)
    {
        floge("property not set: %d", id);
        return;
    }
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
        switch (id)
        {
            case swSweep:
                {
                    auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
                    ((OMPropertyBool*)prop)->SetSend(checked);
                }
                break;
            case slSpeedRect:
                {
                    auto speed = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblSpeedRect);
                    lv_label_set_text(lbl, String(speed).c_str());
                    ((OMPropertyLong*)prop)->SetSend(speed);
                }
                break;
            case slPosition:
                {
                    auto position = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblPosition);
                    lv_label_set_text(lbl, String(position).c_str());
                    ((OMPropertyLong*)prop)->SetSend(position);
                }
                break;
            case btnUp:
                {
                    MutexRampPosition(btnUp);   // retract
                    ((OMPropertyChar*)prop)->SetSend('R');
                }
                break;
            case btnStop:
                {
                    MutexRampPosition(btnStop); // stop
                    ((OMPropertyChar*)prop)->SetSend('S');
                }
                break;
            case btnDown:
                {
                    MutexRampPosition(btnDown); // extend
                    ((OMPropertyChar*)prop)->SetSend('E');
                }
                break;
            case slSpeedRamp:
                {
                    auto speed = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridRamp, (void*)lblSpeedRamp);
                    lv_label_set_text(lbl, String(speed).c_str());
                    ((OMPropertyLong*)prop)->SetSend(speed);
                }
                break;
        }
    }
}

void MechUI::PropertyUpdate(OMProperty* prop)
{
    auto path = prop->GetPath();
    if (prop->Parent == rectObj)
    {
        auto obj = (lv_obj_t*)prop->Data;
        if (obj == nullptr)
        {
            floge("property not set: %s", prop->Name);
            return;
        }
        // Rectenna
        switch (prop->Id)
        {
            case 's':   // sweep on/off
                lv_obj_set_state(obj, LV_STATE_CHECKED, ((OMPropertyBool*)prop)->Value);
                break;
            case 'v':   // sweep speed
                {
                    auto speed = ((OMPropertyLong*)prop)->Value;
                    lv_slider_set_value(obj, speed, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblSpeedRect);
                    lv_label_set_text(lbl, String(speed).c_str());
                }
                break;
            case 'p':   // position
                {
                    auto position = ((OMPropertyLong*)prop)->Value;
                    lv_slider_set_value(obj, position, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridRect, (void*)lblPosition);
                    lv_label_set_text(lbl, String(position).c_str());
                }
                break;
        }
    }
    else
    {
        // Ramp
        switch (prop->Id)
        {
            case 's':   // state
                {
                    switch (((OMPropertyChar*)prop)->Value)
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
                    auto speed = ((OMPropertyLong*)prop)->Value;
                    auto obj = (lv_obj_t*)prop->Data;
                    if (obj == nullptr)
                    {
                        floge("property not set: %s", prop->Name);
                        return;
                    }
                    lv_slider_set_value(obj, speed, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridRamp, (void*)lblSpeedRamp);
                    lv_label_set_text(lbl, String(speed).c_str());
                }
                break;
        }
    }
}
