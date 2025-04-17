#include <lvgl.h>
#include "xmit.h"
#include "LightsUI.h"
#include "lvexColorPicker.h"
#include "FLogger.h"

LightsUI LightsUI::lightsUI;

enum lightColumns
{
    colExpander,
    colIndent,
    colName,
    colLblOff,
    colSwitch,
    colLblOn,
    colSettings,
    colPad,
};

const char* sOff = "off";
const char* sOn = "on";

int32_t IdOfRowCol(int32_t row, int32_t col) { return row * 100 + col; }
int32_t RowOfId(int32_t id) { return id / 100; }
int32_t ColOfId(int32_t id) { return id % 100; }
bool IsGroup(OMObject* light) { return light->Properties.size() == 1; }
bool IsGrouped(OMObject* light) { return IsGroup((OMObject*)light->Parent); }

uint32_t GetRow(OMObject* light)
{
    auto obj = (lv_obj_t*)light->Data;
    auto id = (uint32_t)lv_obj_get_id(obj);
    return RowOfId(id);
}

void LightsUI::PropertyUpdate(OMProperty* prop)
{
    if (prop->Id != 'o')
        return;
    auto sw = (lv_obj_t*)prop->Data;
    if (!sw)
    {
        floge("Prop data [%s] not set", prop->Parent->Name);
        return;
    }
    lv_obj_set_state(sw, LV_STATE_CHECKED, ((OMPropertyBool*)prop)->Value);
}

void LightsUI::EventFired(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target_obj(e);
    auto id = (int)lv_obj_get_id(obj);
    uint8_t col = ColOfId(id);
    auto light = (OMObject*)lv_obj_get_user_data(obj);
    if (!light)
    {
        floge("Light [%d] not set", id);
        return;
    }
    // flogv("Light Clicked id: %d", id);
    if (code == LV_EVENT_CLICKED && col == colSettings)
    {
        // settings button clicked
        lvexColorPicker::Show(light);
    }
    else if (code == LV_EVENT_CLICKED && col == colExpander)
    {
        // expander button clicked
        lightToggleExpansion(light);
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        // on-off switch clicked
        auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
        flogv("Switch [%d] %s: %s", (uint32_t)lv_obj_get_id(obj), light->Name, checked ? "On" : "Off");
        ((OMPropertyBool*)(light->GetProperty('o')))->SetSend(checked);
    }
}

void LightsUI::lightRowShow(OMObject* light, bool show)
{
    auto row = GetRow(light);
    //flogv("Show row [%d] %d", row, show);
    for (int col = 0; col <= colPad; col++)
    {
        auto obj = lv_obj_get_child_by_id(grid, (void*)(IdOfRowCol(row, col)));
        if (obj)
            lv_obj_update_flag(obj, LV_OBJ_FLAG_HIDDEN, !show);
    }
}

void LightsUI::lightToggleExpansion(OMObject* light)
{
    int32_t r = GetRow(light->Objects[0]);
    auto lbl = lv_obj_get_child_by_id(grid, (void*)(IdOfRowCol(r, colName)));
    bool expand = lv_obj_has_flag(lbl, LV_OBJ_FLAG_HIDDEN);
    for (auto l : light->Objects)
    {
        lightRowShow(l, expand);
    }
    auto btn = lv_obj_get_child_by_id(grid, (void*)(IdOfRowCol(GetRow(light), colExpander)));
    lbl = lv_obj_get_child_by_type(btn, 0, &lv_label_class);
    lv_label_set_text(lbl, expand ? LV_SYMBOL_MINUS : LV_SYMBOL_PLUS);
}

void LightsUI::Create(lv_obj_t* parent, Root& root)
{
    static int32_t numRows = 0;
    auto lights = root.GetObject('l');
    lights->TraverseProperties([](OMProperty* p) {
        if (p->Id == 'o')
            numRows++;
    });

    static int32_t col_dsc[] =
        //Expander         Indent Name LblOff           Switch           LblOn            Settings         Pad
        { LV_GRID_CONTENT, 30,    140, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, 1, LV_GRID_TEMPLATE_LAST };
    int32_t* row_dsc = new int32_t[numRows+1];
    for (size_t i = 0; i < numRows; i++)
    {
        row_dsc[i] = LV_GRID_CONTENT;
    }
    row_dsc[numRows] = LV_GRID_TEMPLATE_LAST;

    // Create a grid container
    grid = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_size(grid, 780, 420);
    lv_obj_center(grid);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 10, 0);
    lv_obj_set_style_pad_row(grid, 0, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_VER);
    lv_obj_remove_flag(grid, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

    int32_t row = 0;
    CreateRows(lights, row);
    // UI states will be filled in after setup requests values from the device
}

void LightsUI::CreateRows(OMObject* parent, int32_t& row)
{
    for (auto light : parent->Objects)
    {
        auto onProp = light->GetProperty('o');
        if (!onProp)
            continue;
        bool isGroup = IsGroup(light);
        bool isGrouped = IsGrouped(light);
        //
        // EXPANDER (button)
        //
        if (isGroup)
        {
            // this is a group object, create an expander button
            lv_obj_t* btn = lv_button_create(grid);
            lv_obj_set_size(btn, 40, 40);
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, colExpander, 1, LV_GRID_ALIGN_CENTER, row, 1);
            lv_obj_set_id(btn, (void*)(IdOfRowCol(row, colExpander)));
            lv_obj_set_user_data(btn, (void*)light);
            AddEvent(btn, LV_EVENT_CLICKED);
            auto lbl = lv_label_create(btn);
            lv_label_set_text(lbl, LV_SYMBOL_PLUS);
            lv_obj_center(lbl);
        }
        //
        // NAME
        //
        lv_obj_t * lbl = lv_label_create(grid);
        lv_label_set_text(lbl, light->Name);
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, isGrouped ? colName : colIndent, isGrouped ? 1 : 2, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(IdOfRowCol(row, colName)));
        light->Data = lbl;
        lv_obj_set_user_data(lbl, (void*)light);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_30, 0);
        //
        // LABEL OFF
        //
        lbl = lv_label_create(grid);
        lv_label_set_text(lbl, sOff);
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, colLblOff, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(IdOfRowCol(row, colLblOff)));
        //
        // SWITCH
        //
        lv_obj_t * sw = lv_switch_create(grid);
        lv_obj_set_size(sw, 80, 40);
        lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_START, colSwitch, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(sw, (void*)(IdOfRowCol(row, colSwitch)));
        lv_obj_set_user_data(sw, (void*)light);
        onProp->Data = sw;
        AddEvent(sw, LV_EVENT_VALUE_CHANGED);
        //
        // LABEL ON
        //
        lbl = lv_label_create(grid);
        lv_label_set_text(lbl, sOn);
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, colLblOn, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(IdOfRowCol(row, colLblOn)));
        //
        // SETTINGS (button)
        //
        if (!isGroup)
        {
            lv_obj_t* btn = lv_button_create(grid);
            lv_obj_set_size(btn, 60, 40);
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, colSettings, 1, LV_GRID_ALIGN_CENTER, row, 1);
            lv_obj_set_id(btn, (void*)(IdOfRowCol(row, colSettings)));
            lv_obj_set_user_data(btn, (void*)light);
            AddEvent(btn, LV_EVENT_CLICKED);
            lbl = lv_label_create(btn);
            lv_label_set_text(lbl, LV_SYMBOL_SETTINGS);
            lv_obj_center(lbl);
        }
        //
        // PADDING (vertical)
        //
        auto pad = lv_obj_create(grid);
        lv_obj_set_size(pad, 1, 60);
        lv_obj_set_grid_cell(pad, LV_GRID_ALIGN_CENTER, colPad, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(pad, (void*)(IdOfRowCol(row, colPad)));

        lightRowShow(light, !isGrouped);
        ++row;
        if (isGroup)
            CreateRows(light, row);
    }
}
