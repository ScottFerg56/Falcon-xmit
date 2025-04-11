#include "lvexColorPicker.h"
#include "FLogger.h"
#include "xmit.h"

enum controlIds
{
    noZero,
    btnPrevAnim,
    ddAnim,
    btnNextAnim,
    btnClose,
    lblOn,
    lblColor,
    lblRev,
    lblSpeed,
    swOn,
    btnColor,
    btnlblColor,
    swRev,
    slSpeed,
};

const char* stdEffects [] =
{
    "Static",
    "Blink",
    "Strobe",
    "Blink Rainbow",
    "Wipe",
    "Wipe Rev",
    "Wipe Random",
    "Scan",
    "Dual Scan",
    "Breathe",
    "Random Color",
    "Single Dynamic",
    "Multi Dynamic",
    "Rainbow",
    "Rainbow Cycle",
    "Fade",
    "Theater Chase",
    "Theater Chase Rainbow",
    "Running Lights",
    "Twinkle",
    "Twinkle Random",
    "Twinkle Fade",
    "Twinkle Fade Random",
    "Sparkle",
    "Chase",
    "Chase Rainbow",
    "Chase Flash",
    "Running",
    "Cylon",
    "Comet",
    "FireWorks",
    "Fireworks Random",
    "Fire Flicker",
    "Fire Flicker Intense",
};

const int SpeedSliderMin = 1;
const int SpeedSliderMax = 100;
const int SpeedSliderScale = 100;

void lvexColorPicker::Create()
{
    window = lv_win_create(lv_screen_active());

    LV_IMG_DECLARE(FalconBG);
    lv_obj_set_style_bg_image_src(window, &FalconBG, 0);

    lv_obj_set_scroll_dir(window, LV_DIR_NONE);
    auto hdr = lv_win_get_header(window);
    lv_obj_set_height(hdr, 50);
    lv_obj_set_style_bg_opa(hdr, LV_OPA_70, 0);
    lblTitle = lv_win_add_title(window, "");

    auto abtn = lv_button_create(hdr);
    lv_obj_set_size(abtn, 60, LV_PCT(100));
    lv_obj_set_id(abtn, (void*)btnPrevAnim);
    AddEvent(abtn, LV_EVENT_CLICKED);
    auto albl = lv_label_create(abtn);
    lv_label_set_text(albl, LV_SYMBOL_LEFT);
    lv_obj_center(albl);

    // Anim selection dropdown
    auto dda = lv_dropdown_create(hdr);
    lv_obj_set_id(dda, (void*)ddAnim);
    lv_dropdown_clear_options(dda);
    for (uint16_t i = 0; i < ARRAY_LENGTH(stdEffects); i++)
        lv_dropdown_add_option(dda, stdEffects[i], i);
    lv_obj_set_size(dda, 400, LV_PCT(100));
    // Set the text alignment to center
    auto list = lv_dropdown_get_list(dda);
    lv_obj_set_style_text_align(list, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_opa(dda, LV_OPA_0, 0);
    AddEvent(dda, LV_EVENT_VALUE_CHANGED);

    abtn = lv_button_create(hdr);
    lv_obj_set_size(abtn, 60, LV_PCT(100));
    lv_obj_set_id(abtn, (void*)btnNextAnim);
    AddEvent(abtn, LV_EVENT_CLICKED);
    albl = lv_label_create(abtn);
    lv_label_set_text(albl, LV_SYMBOL_RIGHT);
    lv_obj_center(albl);

    auto btn = lv_win_add_button(window, LV_SYMBOL_CLOSE, 60);
    lv_obj_set_id(btn, (void*)btnClose);
    AddEvent(btn, LV_EVENT_CLICKED);

    auto cont = lv_win_get_content(window);
    lv_obj_set_scroll_dir(cont, LV_DIR_NONE);
    lv_obj_set_style_bg_opa(cont, LV_OPA_0, 0);

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
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

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
    lv_obj_set_style_bg_opa(grid2, LV_OPA_70, 0);

    const char* labels[] = { "On", "Color", "Rev", "Delay" };
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
    btn = lv_button_create(grid2);
    lv_obj_set_size(btn, 150, 40);
    lv_obj_set_id(btn, (void*)btnColor);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    AddEvent(btn, LV_EVENT_VALUE_CHANGED);
    auto lbl = lv_label_create(btn);
    lv_obj_set_id(lbl, (void*)btnlblColor);
    lv_label_set_text(lbl, "Color1");
    lv_obj_center(lbl);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    // don't need the checked highlight since we're toggling the text
    auto bgclr = lv_obj_get_style_bg_color(btn, 0);
    lv_obj_set_style_bg_color(btn, bgclr, LV_STATE_CHECKED);

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

void lvexColorPicker::Show(OMObject* light)
{
    if (!colorPicker.window)
        colorPicker.Create();
    colorPicker.Light = light;
    lv_disp_t * display = lv_display_get_default();
    auto active_screen = lv_display_get_screen_active(display);

    // attach the dialog window to current active screen
    lv_obj_set_parent(colorPicker.window, active_screen);

    lv_label_set_text(colorPicker.lblTitle, light->Name);
    lv_obj_remove_flag(colorPicker.window, LV_OBJ_FLAG_HIDDEN); /* show the dialog */
    lv_obj_scroll_to_y(colorPicker.window, 0, LV_ANIM_OFF);
    // update the UI as if properties were coming in from the device
    light->TraverseProperties([](OMProperty* p) { colorPicker.PropertyUpdate(p); });
}

void lvexColorPicker::setSpeedLabel(int speed)
{
    auto lbl = lv_obj_get_child_by_id(window, (void*)lblSpeed);
    if (lbl)
    {
        String txt = "Delay: " + String(speed);
        lv_label_set_text(lbl, txt.c_str());
    }
}

void lvexColorPicker::EventFired(lv_event_t* e)
{
    if (!Light)
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
            Light = nullptr;
            break;
        case btnPrevAnim:
        case btnNextAnim:
            {
                auto dd = lv_obj_get_child_by_id(window, (void*)ddAnim);
                int32_t inx = lv_dropdown_get_selected(dd);
                if (id == btnPrevAnim)
                    --inx;
                else
                    ++inx;
                if (inx < 0)
                    inx = ARRAY_LENGTH(stdEffects) - 1;
                else if (inx >= ARRAY_LENGTH(stdEffects))
                    inx = 0;
                lv_dropdown_set_selected(dd, inx);
                ((OMPropertyLong*)Light->GetProperty('a'))->SetSend(inx);
            }
            break;
        }
        break;
    case LV_EVENT_VALUE_CHANGED:
        switch (id)
        {
        case swOn:  // On switch
            {
                auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
                ((OMPropertyBool*)Light->GetProperty('o'))->SetSend(checked);
            }
            break;
        case btnColor:  // Color 1/2 button toggle
            {
                auto inx = lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0;
                ColorInx = inx;
                auto lbl = lv_obj_get_child_by_id(obj, (void*)btnlblColor);
                lv_label_set_text(lbl, inx == 0 ? "Color1" : "Color2");
                pickerHSV.setColor(Colors[inx]);
                lv_obj_set_style_bg_color(panelSample, Colors[inx], 0);
            }
            break;
        case ddAnim:  // Anim list
            {
                auto inx = lv_dropdown_get_selected(obj);
                ((OMPropertyLong*)Light->GetProperty('a'))->SetSend(inx);
            }
            break;
        case slSpeed:  // Speed slider
            {
                // slider values are in tenths of seconds
                auto speed = lv_slider_get_value(obj) * SpeedSliderScale;
                setSpeedLabel(speed);
                ((OMPropertyLong*)Light->GetProperty('s'))->SetSend(speed);
            }
            break;
        case swRev:  // Reverse switch
            {
                auto checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
                ((OMPropertyBool*)Light->GetProperty('r'))->SetSend(checked);
            }
            break;
        }
        break;
    }
}

void lvexColorPicker::PropertyUpdate(OMProperty* prop)
{
    if (prop->Parent != Light)
        return;
    switch (prop->Id)
    {
    case 'o':
        {
            auto sw = lv_obj_get_child_by_id(window, (void*)swOn);
            if (sw)
                lv_obj_set_state(sw, LV_STATE_CHECKED, ((OMPropertyBool*)prop)->Value);
        }
        break;
    case 'a':
        {
            auto dd = lv_obj_get_child_by_id(window, (void*)ddAnim);
            if (dd)
                lv_dropdown_set_selected(dd, (uint32_t)((OMPropertyLong*)prop)->Value);
        }
        break;
    case 'c':
    case 'd':
        {
            // which color
            uint8_t ix = prop->Id == 'c' ? 0 : 1;
            auto value = ((OMPropertyLong*)prop)->Value;
            flogv("received Color%d value %X", ix+1, value);
            // color value
            auto color = lv_color_hex(value);
            // if current color, set and reflect in UI, otherwise just set
            setColor(ix, color);
        }
        break;
    case 's':
        {
            // string speed value
            auto speed = ((OMPropertyLong*)prop)->Value;
            auto v = (speed + SpeedSliderScale / 2) / SpeedSliderScale;
            auto slider = lv_obj_get_child_by_id(window, (void*)slSpeed);
            if (slider)
                lv_slider_set_value(slider, v, LV_ANIM_OFF);
            setSpeedLabel(speed);
        }
        break;
    case 'r':
        {
            auto sw = lv_obj_get_child_by_id(window, (void*)swRev);
            if (sw)
                lv_obj_set_state(sw, LV_STATE_CHECKED, ((OMPropertyBool*)prop)->Value);
        }
        break;
    default:
        break;
    }
}

void lvexColorPicker::setColor(uint8_t inx, lv_color_t color)
{
    if (!Light)
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
        // send color change to device
        ((OMPropertyLong*)Light->GetProperty(ColorInx == 0 ? 'c' : 'd'))->SetSend(lv_color_to_int(color));
    }
}

void lvexColorPicker::ColorChanged(lv_color_t color)
{
    // flogv("color changed %X", lv_color_to_int(color));
    setColor(ColorInx, color);
}

lvexColorPicker lvexColorPicker::colorPicker;
