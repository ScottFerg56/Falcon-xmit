#include <lvgl.h>
#include "xmit.h"
#include "SoundUI.h"
#include "FLogger.h"

SoundUI SoundUI::soundUI;

std::vector<String> SplitString(String str, char delimiter)
{
    std::vector<String> strs;
    while (str.length() > 0)
    {
      int index = str.indexOf(delimiter);
      if (index == -1) // No space found
      {
        strs.push_back(str);
        break;
      }
      else
      {
        strs.push_back(str.substring(0, index));
        str = str.substring(index + 1);
      }
    }
    return strs;
}

enum soundControlIds
{
    noZero,
    ddSounds,
    btnPlay,
    btnDelete,
    lblVolume,
    slVolume,
};

void SoundUI::Create(lv_obj_t* parent, Root& root)
{
    soundObj = root.GetObject('s');

    //
    // Sound grid
    //
    static int32_t col_dsc[] = { 160, LV_GRID_CONTENT, 80, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t row_dsc[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };

    // Create a grid container
    auto grid = gridSound = lv_obj_create(parent);

    lv_obj_set_style_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP, 0);
    lv_obj_set_style_flex_main_place(grid, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_layout(grid, LV_LAYOUT_FLEX, 0);

    // lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    // lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_size(grid, 650, 180);
    lv_obj_align(grid, LV_ALIGN_TOP_MID, 0, 20);
    // lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 20, 0);
    lv_obj_set_style_pad_row(grid, 40, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

    // Sounds dropdown and buttons
    auto dds = lv_dropdown_create(grid);
    lv_obj_set_id(dds, (void*)ddSounds);
    lv_obj_set_size(dds, 400, 30);
    // Set the text alignment to center
    auto list = lv_dropdown_get_list(dds);
    lv_obj_set_style_text_align(list, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_opa(dds, LV_OPA_0, 0);
    auto prop = soundObj->GetProperty('p');
    lv_obj_set_user_data(dds, prop);
    prop->Data = dds;
    soundObj->GetProperty('l')->Data = dds;
    AddEvent(dds, LV_EVENT_VALUE_CHANGED);

    // play button
    auto btn = lv_btn_create(grid);
    lv_obj_set_id(btn, (void*)btnPlay);
    lv_obj_set_size(btn, 60, 40);
    auto lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_PLAY);
    lv_obj_center(lbl);
    prop = soundObj->GetProperty('p');
    lv_obj_set_user_data(btn, prop);
    prop->Data = btn;
    AddEvent(btn, LV_EVENT_CLICKED);
    
    // delete button
    btn = lv_btn_create(grid);
    lv_obj_set_id(btn, (void*)btnDelete);
    lv_obj_set_size(btn, 60, 40);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_TRASH);
    lv_obj_center(lbl);
    prop = soundObj->GetProperty('x');
    lv_obj_set_user_data(btn, prop);
    prop->Data = btn;
    AddEvent(btn, LV_EVENT_LONG_PRESSED);

    // Volume slider and labels
    lbl = lv_label_create(grid);
    prop = soundObj->GetProperty('v');
    lv_label_set_text(lbl, prop->Name);
    // lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lbl = lv_label_create(grid);
    lv_obj_set_id(lbl, (void*)lblVolume);
    lv_label_set_text(lbl, "00");
    // lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    auto slider = lv_slider_create(grid);
    lv_obj_set_size(slider, 300, 30);
    lv_slider_set_range(slider, ((OMPropertyLong*)prop)->Min, ((OMPropertyLong*)prop)->Max);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 2, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(slider, (void*)slVolume);
    lv_obj_set_user_data(slider, prop);
    prop->Data = slider;
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);
}

void SoundUI::EventFired(lv_event_t* e)
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
            case ddSounds:
                {
                    auto value = lv_dropdown_get_selected(obj);
                    ((OMPropertyLong*)prop)->SetSend(value + 1);
                }
                break;
            case slVolume:
                {
                    auto volume = lv_slider_get_value(obj);
                    auto lbl = lv_obj_get_child_by_id(gridSound, (void*)lblVolume);
                    lv_label_set_text(lbl, String(volume).c_str());
                    ((OMPropertyLong*)prop)->SetSend(volume);
                }
                break;
        }
    case LV_EVENT_CLICKED:
        switch (id)
        {
            case btnPlay:   // play selected sound
                {
                    auto dd = lv_obj_get_child_by_id(gridSound, (void*)ddSounds);
                    auto value = lv_dropdown_get_selected(dd);
                    ((OMPropertyLong*)prop)->SetSend(value + 1);
                }
                break;
        }
        break;
    case LV_EVENT_LONG_PRESSED:
        switch (id)
        {
            case btnDelete: // delete the selected sound on the device
                {
                    auto dd = lv_obj_get_child_by_id(gridSound, (void*)ddSounds);
                    auto value = lv_dropdown_get_selected(dd);
                    ((OMPropertyLong*)prop)->SetSend(value + 1);
                }
                break;
        }
        break;
    }
}

void SoundUI::PropertyUpdate(OMProperty* prop)
{
    if (prop->Parent == soundObj)
    {
        auto obj = (lv_obj_t*)prop->Data;
        if (obj == nullptr)
        {
            floge("property not set: %s", prop->Name);
            return;
        }
        switch (prop->Id)
        {
            case 'l':   // sound list
                {
                    auto value = ((OMPropertyString*)prop)->Value;
                    auto items = SplitString(value, ',');
                    lv_dropdown_clear_options(obj);
                    for (auto item : items)
                    {
                        lv_dropdown_add_option(obj, item.c_str(), LV_DROPDOWN_POS_LAST);
                    }
                    lv_dropdown_set_selected(obj, 0);
                }
                break;
            case 'v':   // volume
                {
                    auto volume = ((OMPropertyLong*)prop)->Value;
                    lv_slider_set_value(obj, volume, LV_ANIM_OFF);
                    auto lbl = lv_obj_get_child_by_id(gridSound, (void*)lblVolume);
                    lv_label_set_text(lbl, String(volume).c_str());
                }
                break;
        }
    }
}
