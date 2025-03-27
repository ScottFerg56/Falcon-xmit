#include <lvgl.h>
#include "Falcon.h"
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
    bool isParent;
    bool isChild;
    const char* lblOff;
    const char* lblOn;

    uint16_t row;
    bool expanded;
    lv_color_t color;
};

const char* sOff = "off";
const char* sOn = "on";

Control Controls[] =
{
{"Rectenna",    false, false, "stop", "sweep" },
{"Ramp",        false, false, "up",   "down"  },
{"Engine",		false, false, sOff,   sOn     },
{"Landing",		false, false, sOff,   sOn     },
{"Warning",		false, false, sOff,   sOn     },
{"Headlight",	false, false, sOff,   sOn     },
{"Gunwell",     false, false, sOff,   sOn     },
{"Tubes",       true,  false, sOff,   sOn     },
{"Sconce",      false, true,  sOff,   sOn     },
{"Floor",       false, true,  sOff,   sOn     },
{"Hold",		true,  false, sOff,   sOn     },
{"Bay",		    false, true,  sOff,   sOn     },
{"Red",		    false, true,  sOff,   sOn,    },
{"Green",	    false, true,  sOff,   sOn,    },
{"Blue",		false, true,  sOff,   sOn,    },
{"Yellow",      false, true,  sOff,   sOn,    },
{"Cockpit",		true,  false, sOff,   sOn     },
{"Red",		    false, true,  sOff,   sOn,    },
{"Green",	    false, true,  sOff,   sOn,    },
{"Blue",		false, true,  sOff,   sOn,    },
{"Yellow",      false, true,  sOff,   sOn,    },
{"WallUL",      false, true,  sOff,   sOn,    },
{"WallUR",      false, true,  sOff,   sOn,    },
{"WallLL",      false, true,  sOff,   sOn,    },
{"WallLR",      false, true,  sOff,   sOn,    },
{""}
};

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
        lvexColorPicker::Show(control->color, control->name, this);
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
        LV_UNUSED(obj);
        flogv("Switch [%d] %s: %s", (uint32_t)lv_obj_get_id(obj), control->name, lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
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
    for (int r = row + 1; Controls[r].isChild; r++)
    {
        controlRowShow(r, Controls[row].expanded);
    }
    auto btn = lv_obj_get_child_by_id(grid, (void*)(row * 100 + colExpander));
    auto lbl = lv_obj_get_child_by_type(btn, 0, &lv_label_class);
    lv_label_set_text(lbl, Controls[row].expanded ? LV_SYMBOL_MINUS : LV_SYMBOL_PLUS);
}

void ControlsUI::setColorPicker(lvexColorPicker* picker)
{
    colorPicker = picker;
}

lv_obj_t* ControlsUI::Create(lv_obj_t* parent)
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
    row_dsc[ARRAY_LENGTH(Controls)] = LV_GRID_TEMPLATE_LAST;

    for (int row = 0; strlen(Controls[row].name) > 0; row++)
    {
        Controls[row].row = row;
        row_dsc[row] = LV_GRID_CONTENT;
        //
        // EXPANDER (button)
        //
        if (Controls[row].isParent)
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
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, Controls[row].isChild ? colName : colIndent, Controls[row].isChild ? 1 : 2, LV_GRID_ALIGN_CENTER, row, 1);
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
        if (!Controls[row].isParent)
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

        controlRowShow(row, !Controls[row].isChild);
    }
    return grid;
}

void ControlsUI::ColorChanged(lv_color_t color)
{
    if (currentControl >= 0 && currentControl < ARRAY_LENGTH(Controls))
        Controls[currentControl].color = color;
}
