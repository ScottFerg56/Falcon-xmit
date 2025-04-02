#include <lvgl.h>
#include "xmit.h"
#include "ControlsUI.h"
#include "lvexColorPicker.h"
#include "FLogger.h"

enum controlColumns
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

struct Control
{
    const char* name;
    bool isFather;
    bool isSon;
    const char* lblOff;
    const char* lblOn;
    const char* cmdPath;

    uint16_t row;
    bool expanded;
};

const char* sOff = "off";
const char* sOn = "on";

Control Controls[] =
{
{"Rectenna",    false, false, "stop", "sweep", "as"},
{"Ramp",        false, false, "up",   "down",  "rs"},
{"Engine",		false, false, sOff,   sOn,     "le"},
{"Landing",		false, false, sOff,   sOn,     "ll"},
{"Warning",		false, false, sOff,   sOn,     "lw"},
{"Headlight",	false, false, sOff,   sOn,     "ld"},
{"Gunwell",     false, false, sOff,   sOn,     "lg"},
{"Ramp",        false, false, sOff,   sOn,     "lr"},
{"Tubes",       true,  false, sOff,   sOn,     "lt"},
   {"Sconce",   false, true,  sOff,   sOn,     "lts"},
   {"Floor",    false, true,  sOff,   sOn,     "ltf"},
{"Hold",		true,  false, sOff,   sOn,     "lh"},
  {"Bay",		false, true,  sOff,   sOn,     "lhy"},
  {"Bed",		false, true,  sOff,   sOn,     "lhb"},
  {"Grates",    false, true,  sOff,   sOn,     "lhg"},
  {"Monitor",   false, true,  sOff,   sOn,     "lhm"},
  {"Red",		false, true,  sOff,   sOn,     "lh0"},
  {"Green",	    false, true,  sOff,   sOn,     "lh1"},
  {"Blue",      false, true,  sOff,   sOn,     "lh2"},
  {"Yellow",    false, true,  sOff,   sOn,     "lh3"},
{"Cockpit",     true,  false, sOff,   sOn,     "lc"},
  {"Monitor",   false, true,  sOff,   sOn,     "lcm"},
  {"Red",		false, true,  sOff,   sOn,     "lc0"},
  {"Green",	    false, true,  sOff,   sOn,     "lc1"},
  {"Blue",      false, true,  sOff,   sOn,     "lc2"},
  {"Yellow",    false, true,  sOff,   sOn,     "lc3"},
  {"WallUL",    false, true,  sOff,   sOn,     "lc4"},
  {"WallUR",    false, true,  sOff,   sOn,     "lc5"},
  {"WallLL",    false, true,  sOff,   sOn,     "lc6"},
  {"WallLR",    false, true,  sOff,   sOn,     "lc7"},
{""}
};

void ControlsUI::Command(String cmd)
{
    // find the control cmdPath that matches the input
    cmd = cmd.substring(1);
    Control* control = nullptr;
    for (int row = 0; strlen(Controls[row].name) > 0; row++)
    {
        auto ctrl = &Controls[row];
        if (ctrl->isFather) // skip group
            continue;
        if (cmd.startsWith(ctrl->cmdPath))
        {
            control = ctrl;
            break;
        }
    }
    if (control)
    {
        int inx = strlen(control->cmdPath);
        if (cmd[0] == 'l')
        {
            if (cmd[inx] != 'o')
                return;
            ++inx;
        }
        else
        {
            // UNDONE: rectenna/ramp
            return;
        }
        auto sw = lv_obj_get_child_by_id(grid, (void*)(control->row * 100 + colSwitch));
        if (sw)
            lv_obj_set_state(sw, LV_STATE_CHECKED, cmd[inx] == '1');
    }
}

void ControlsUI::EventFired(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target_obj(e);
    auto id = (int)lv_obj_get_id(obj);
    uint8_t row = id / 100;
    uint8_t col = id % 100;
    auto control = &Controls[row];
    // flogv("Control Clicked id: %d  row: %d  col: %d", id, row, col);
    if (code == LV_EVENT_CLICKED && col == colSettings)
    {
        // settings button clicked
        LV_UNUSED(obj);
        currentControl = row;
        lvexColorPicker::Show(control->name, control->cmdPath);
    }
    else if (code == LV_EVENT_CLICKED && col == colExpander)
    {
        // expander button clicked
        LV_UNUSED(obj);
        controlToggleExpansion(row);
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        // on-off switch clicked
        auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
        flogv("Switch [%d] %s: %s", (uint32_t)lv_obj_get_id(obj), control->name, checked ? "On" : "Off");
        if (strlen(control->cmdPath) > 0)
        {
            String cmd(control->cmdPath);
            if (cmd[0] == 'l')
                cmd.concat('o');
            SendCmd('=' + cmd + (checked ? '1' : '0'));
        }
        if (control->isFather)
        {
            // set all nested switches to reflect the group state change
            auto parent = lv_obj_get_parent(obj);
            for (uint8_t r = row + 1; Controls[r].isSon; r++)
            {
                auto son = lv_obj_get_child_by_id(parent, (void*)(r * 100 + colSwitch));
                if (son)
                    lv_obj_set_state(son, LV_STATE_CHECKED, checked);
            }
        }
        else if (control->isSon)
        {
            auto parent = lv_obj_get_parent(obj);
            // find the father's row
            uint8_t r = row;
            while (!Controls[--r].isFather)
                ;
            auto father = lv_obj_get_child_by_id(parent, (void*)(r * 100 + colSwitch));
            if (checked)
            {
                // father of nested switches reflects CHECKED if any sons are checked
                lv_obj_add_state(father, LV_STATE_CHECKED);
            }
            else
            {
                // father of nested switches reflects unCHECKED only if all sons are unchecked
                // scan the son rows
                while (Controls[++r].isSon)
                {
                    auto son = lv_obj_get_child_by_id(parent, (void*)(r * 100 + colSwitch));
                    if (lv_obj_has_state(son, LV_STATE_CHECKED))
                        return;
                }
                lv_obj_remove_state(father, LV_STATE_CHECKED);
            }
        }
    }
}

void ControlsUI::controlRowShow(int row, bool show)
{
    //flogv("Show row [%d] %d", row, show);
    for (int col = 0; col <= colPad; col++)
    {
        auto obj = lv_obj_get_child_by_id(grid, (void*)(row * 100 + col));
        if (obj)
        {
            if (show)
                lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
            else
                lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ControlsUI::controlToggleExpansion(int row)
{
    Controls[row].expanded = !Controls[row].expanded;
    for (int r = row + 1; Controls[r].isSon; r++)
    {
        controlRowShow(r, Controls[row].expanded);
    }
    auto btn = lv_obj_get_child_by_id(grid, (void*)(row * 100 + colExpander));
    auto lbl = lv_obj_get_child_by_type(btn, 0, &lv_label_class);
    lv_label_set_text(lbl, Controls[row].expanded ? LV_SYMBOL_MINUS : LV_SYMBOL_PLUS);
}

void ControlsUI::Create(lv_obj_t* parent)
{
    static int32_t col_dsc[] =
        //Expander         Indent Name LblOff           Switch           LblOn            Settings         Pad
        { LV_GRID_CONTENT, 30,    110, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, 1, LV_GRID_TEMPLATE_LAST };
    static int32_t row_dsc[ARRAY_LENGTH(Controls)+1];

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
    row_dsc[ARRAY_LENGTH(Controls)] = LV_GRID_TEMPLATE_LAST;

    for (int row = 0; strlen(Controls[row].name) > 0; row++)
    {
        Controls[row].row = row;
        row_dsc[row] = LV_GRID_CONTENT;
        //
        // EXPANDER (button)
        //
        if (Controls[row].isFather)
        {
            lv_obj_t* btn = lv_button_create(grid);
            lv_obj_set_size(btn, 40, 40);
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, colExpander, 1, LV_GRID_ALIGN_CENTER, row, 1);
            lv_obj_set_id(btn, (void*)(row * 100 + colExpander));
            AddEvent(btn, LV_EVENT_CLICKED);
            auto lbl = lv_label_create(btn);
            lv_label_set_text(lbl, LV_SYMBOL_PLUS);
            lv_obj_center(lbl);
        }
        //
        // NAME
        //
        lv_obj_t * lbl = lv_label_create(grid);
        lv_label_set_text(lbl, Controls[row].name);
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, Controls[row].isSon ? colName : colIndent, Controls[row].isSon ? 1 : 2, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(row * 100 + colName));
        //
        // LABEL OFF
        //
        lbl = lv_label_create(grid);
        lv_label_set_text(lbl, Controls[row].lblOff);
        lv_obj_set_style_text_color(lbl, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, colLblOff, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(row * 100 + colLblOff));
        //
        // SWITCH
        //
        lv_obj_t * sw = lv_switch_create(grid);
        lv_obj_set_size(sw, 80, 40);
        lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_START, colSwitch, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(sw, (void*)(row * 100 + colSwitch));
        AddEvent(sw, LV_EVENT_VALUE_CHANGED);
        //
        // LABEL ON
        //
        lbl = lv_label_create(grid);
        lv_label_set_text(lbl, Controls[row].lblOn);
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, colLblOn, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(row * 100 + colLblOn));
        lv_obj_set_style_text_color(lbl, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
        //
        // SETTINGS (button)
        //
        if (!Controls[row].isFather)
        {
            lv_obj_t* btn = lv_button_create(grid);
            lv_obj_set_size(btn, 60, 40);
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, colSettings, 1, LV_GRID_ALIGN_CENTER, row, 1);
            lv_obj_set_id(btn, (void*)(row * 100 + colSettings));
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
        lv_obj_set_id(pad, (void*)(row * 100 + colPad));

        controlRowShow(row, !Controls[row].isSon);
    }
    return;
}

ControlsUI ControlsUI::controlsUI;
