#include "lvexColorPicker.h"
#include "FLogger.h"
#include "xmit.h"

enum controlIds
{
    btnClose,
    ddAnim,
    lblOn,
    lblColor,
    lblRev,
    lblSpeed,
    swOn,
    ddColor,
    slSpeed,
    swRev,
};

const int SpeedSliderMin = 1;
const int SpeedSliderMax = 100;
const int SpeedSliderScale = 100;

void lvexColorPicker::Create()
{
    window = lv_win_create(lv_screen_active());
    lv_obj_set_scroll_dir(window, LV_DIR_NONE);
    auto hdr = lv_win_get_header(window);
    lv_obj_set_height(hdr, 50);
    lblTitle = lv_win_add_title(window, "");

    // Anim selection dropdown
    auto dda = lv_dropdown_create(hdr);
    lv_obj_set_id(dda, (void*)ddAnim);
    lv_dropdown_set_options(dda, "Apple\n"
                            "Banana\n"
                            "Orange\n"
                            "Cherry\n"
                            "Grape\n"
                            "Raspberry\n"
                            "Melon\n"
                            "Orange\n"
                            "Lemon\n"
                            "Nuts");
    lv_obj_set_size(dda, 500, LV_PCT(100));
    // Set the text alignment to center
    auto list = lv_dropdown_get_list(dda);
    lv_obj_set_style_text_align(list, LV_TEXT_ALIGN_CENTER, 0);
    AddEvent(dda, LV_EVENT_VALUE_CHANGED);

    auto btn = lv_win_add_button(window, LV_SYMBOL_CLOSE, 60);
    lv_obj_set_id(btn, (void*)btnClose);
    AddEvent(btn, LV_EVENT_CLICKED);

    auto cont = lv_win_get_content(window);  /*Content can be added here*/
    lv_obj_set_scroll_dir(cont, LV_DIR_NONE);

    auto grid = lv_obj_create(cont);
    auto picker = pickerHSV.Create(grid, this);
    auto pal = palette.Create(grid, this);
    panelSample = lv_obj_create(grid);

    static int32_t col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 10, 0);
    lv_obj_set_style_pad_row(grid, 10, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_size(grid, 800, 260);
    lv_obj_set_align(grid, LV_ALIGN_TOP_MID);

    lv_obj_set_grid_cell(picker, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(pal, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(panelSample, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    auto grid2 = lv_obj_create(cont);
    static int32_t col_dsc2[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(grid2, col_dsc2, 0);
    lv_obj_set_style_grid_row_dsc_array(grid2, row_dsc, 0);
    lv_obj_set_layout(grid2, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_row(grid2, 5, 0);
    lv_obj_set_scroll_dir(grid2, LV_DIR_NONE);
    lv_obj_set_size(grid2, 800, 120);
    lv_obj_set_align(grid2, LV_ALIGN_BOTTOM_MID);

    const char* labels[] = { "On", "Color", "Rev", "Speed" };
    for (int id = lblOn; id <= lblSpeed; id++)
    {
        auto lbl = lv_label_create(grid2);
        lv_label_set_text(lbl, labels[id - lblOn]);
        lv_obj_set_id(lbl, (void*)(id));
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_CENTER, id - lblOn, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    }
    // on/off switch
    auto sw = lv_switch_create(grid2);
    lv_obj_set_size(sw, 80, 40);
    lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(sw, (void*)swOn);
    AddEvent(sw, LV_EVENT_VALUE_CHANGED);

    // Color 1/2 selector
    auto dd = lv_dropdown_create(grid2);
    lv_obj_set_width(dd, 150);
    lv_dropdown_set_options(dd, "Color1\n" "Color2");
    lv_obj_set_grid_cell(dd, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(dd, (void*)ddColor);
    AddEvent(dd, LV_EVENT_VALUE_CHANGED);

    // Reverse switch
    sw = lv_switch_create(grid2);
    lv_obj_set_size(sw, 80, 40);
    lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(sw, (void*)swRev);
    AddEvent(sw, LV_EVENT_VALUE_CHANGED);

    // Speed slider
    auto slider = lv_slider_create(grid2);
    lv_obj_set_size(slider, 350, 30);
    lv_slider_set_range(slider, SpeedSliderMin, SpeedSliderMax);
    lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(slider, (void*)slSpeed);
    AddEvent(slider, LV_EVENT_VALUE_CHANGED);
}

void lvexColorPicker::Show(const char* title, const char* cmdPath)
{
    if (!colorPicker.window)
        colorPicker.Create();
    colorPicker.CmdPath = cmdPath;
    colorPicker.InitFlags = 0;
    lv_disp_t * display = lv_display_get_default();
    auto active_screen = lv_display_get_screen_active(display);

    // attach the dialog window to current active screen
    lv_obj_set_parent(colorPicker.window, active_screen);

    lv_label_set_text(colorPicker.lblTitle, title);
    lv_obj_remove_flag(colorPicker.window, LV_OBJ_FLAG_HIDDEN); /* show the dialog */
    lv_obj_scroll_to_y(colorPicker.window, 0, LV_ANIM_OFF);
    SendCmd("?" + colorPicker.CmdPath);
}

void lvexColorPicker::setSpeedLabel(int speed)
{
    auto lbl = lv_obj_get_child_by_id(window, (void*)lblSpeed);
    if (lbl)
    {
        String txt = "Speed: " + String(speed);
        lv_label_set_text(lbl, txt.c_str());
    }
}

void lvexColorPicker::EventFired(lv_event_t* e)
{
    if (CmdPath.length() == 0)
        return;
    auto obj = lv_event_get_target_obj(e);
    auto id = (int)lv_obj_get_id(obj);
    auto code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        switch (id)
        {
        case btnClose:  // windows close button
            lv_obj_add_flag(window, LV_OBJ_FLAG_HIDDEN);
            CmdPath = "";
            break;
        
        default:
            break;
        }
        break;
    case LV_EVENT_VALUE_CHANGED:
        switch (id)
        {
        case swOn:  // On switch
            {
                auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
                SendCmd("=" + CmdPath + 'o' + (checked ? '1' : '0'));
            }
            break;
        case ddAnim:  // Anim list
            {
                auto inx = lv_dropdown_get_selected(obj);
                SendCmd("=" + CmdPath + 'a' + String(inx));
            }
            break;
        case ddColor:  // Color 1/2 list
            {
                auto inx = lv_dropdown_get_selected(obj);
                ColorInx = inx;
                pickerHSV.setColor(Colors[inx]);
                lv_obj_set_style_bg_color(panelSample, Colors[inx], 0);
            }
            break;
        case slSpeed:  // Speed slider
            {
                // slider values are in tenths of seconds
                auto speed = lv_slider_get_value(obj) * SpeedSliderScale;
                setSpeedLabel(speed);
                SendCmd("=" + CmdPath + 's' + String(speed));
            }
            break;
        case swRev:  // Reverse switch
            {
                auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
                SendCmd("=" + CmdPath + 'r' + (checked ? '1' : '0'));
            }
            break;
        }
        break;
    }
}

void lvexColorPicker::Command(String cmd)
{
    if (InitComplete() || CmdPath.length() == 0 || cmd.length() == 0 || cmd[0] != '=')
        return;
    int inx = 1;
    if (cmd.indexOf(CmdPath) != inx)
        return;
    inx += CmdPath.length();
    switch (cmd[inx])
    {
    case 'o':
        {
            InitFlags |= 0b0000001;
            auto sw = lv_obj_get_child_by_id(window, (void*)swOn);
            if (sw)
                lv_obj_set_state(sw, LV_STATE_CHECKED, cmd[++inx] == '1');
        }
        break;
    case 'a':
        {
            InitFlags |= 0b0000010;
            auto dd = lv_obj_get_child_by_id(window, (void*)ddAnim);
            if (dd)
            {
                auto pre = lv_dropdown_get_selected(dd);
                auto anim = cmd.substring(++inx).toInt();
                if (pre != anim)
                    lv_dropdown_set_selected(dd, (uint32_t)anim);
            }
        }
        break;
    case 'c':
    case 'd':
        {
            InitFlags |= (cmd[inx] == 'c' ? 0b0000100 : 0b0001000);
            if (pickerHSV.isBusy())
                return;
            // which color
            uint8_t ix = cmd[inx] == 'c' ? 0 : 1;
            // string hex color value
            auto s = cmd.substring(++inx);
            const char* p = s.c_str();
            char *pend;
            // hex color value
            auto value = strtoul(p, &pend, 16);
            if (pend == p || value == ULONG_MAX)
            {
                floge("invalid long value: [%s]", p);
                return;
            }
            flogv("received Color%d value %X", ix, value);
            // color value
            auto color = lv_color_hex(value);
            // if current color, set and reflect in UI, otherwise just set
            setColor(ix, color);
        }
        break;
    case 's':
        {
            InitFlags |= 0b0010000;
            // string speed value
            auto speed = cmd.substring(++inx).toInt();
            if (speed > SpeedSliderMax * SpeedSliderScale)
                speed = SpeedSliderMax * SpeedSliderScale;
            else if (speed < SpeedSliderMin * SpeedSliderScale)
                speed = SpeedSliderMin * SpeedSliderScale;
            auto v = (speed + SpeedSliderScale / 2) / SpeedSliderScale;
            auto slider = lv_obj_get_child_by_id(window, (void*)slSpeed);
            if (slider)
                lv_slider_set_value(slider, v, LV_ANIM_OFF);
            setSpeedLabel(speed);
        }
        break;
    case 'r':
        {
            InitFlags |= 0b0100000;
            auto sw = lv_obj_get_child_by_id(window, (void*)swRev);
            if (sw)
                lv_obj_set_state(sw, LV_STATE_CHECKED, cmd[++inx] == '1');
        }
        break;
    default:
        break;
    }
}

void lvexColorPicker::setColor(uint8_t inx, lv_color_t color)
{
    if (CmdPath.length() == 0)
        return;
    // actually changed?
    if (lv_color_eq(Colors[inx], color))
        return;
    // save color
    Colors[inx] = color;
    if (inx == ColorInx)
    {
        // active color, set UI
        pickerHSV.setColor(color);
        // send change
        lv_obj_set_style_bg_color(panelSample, color, 0);
        SendCmd("=" + CmdPath + (ColorInx == 0 ? 'c' : 'd') + String(lv_color_to_int(color), 16));
    }
}

void lvexColorPicker::ColorChanged(lv_color_t color)
{
    // flogv("color changed %X", lv_color_to_int(color));
    setColor(ColorInx, color);
}

lvexColorPicker lvexColorPicker::colorPicker;
