#include "lvexColorPickerHSV.h"
#include "FLogger.h"

void dumpRGB(const char* label, lv_color_t rgb)
{
    flogv("%s  red: %d  green: %d  blue: %d", label, rgb.red, rgb.green, rgb.blue);
}

void dumpHSV(const char* label, lv_color_hsv_t hsv)
{
    flogv("%s  hue: %d  sat: %d  val: %d", label, hsv.h, hsv.s, hsv.v);
}

lv_obj_t* lvexColorPickerHSV::Create(lv_obj_t* parent, ColorClient* client)
{
    colorClient = client;

    static int32_t col_dsc[] = {LV_GRID_CONTENT, 50, 340, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    // Create a grid container
    grid = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_size(grid, 500, 160);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    //lv_obj_set_style_pad_column(grid, 30, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);

    for (int row = 0; row < 3; row++)
    {
        auto lbl = lv_label_create(grid);
        lv_label_set_text(lbl, (row == hueInx) ? "Hue:" : (row == satInx) ? "Sat:" : "Brt:");
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, row, 1);

        lbl = lv_label_create(grid);
        lv_label_set_text(lbl, "000");
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(row * 100 + 1));

        auto slider = hsvSliders[row] = lv_slider_create(grid);
        lv_obj_set_size(slider, 300, 30);
        lv_slider_set_range(slider, 0, row == hueInx ? 359 : 100);
        lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(slider, (void*)(row * 100 + 2));
        lv_obj_set_style_bg_opa(slider, LV_OPA_100, LV_PART_MAIN);
        // lv_obj_set_style_bg_color(slider, lv_palette_main(LV_PALETTE_GREY), LV_PART_KNOB);
        AddEvent(slider, LV_EVENT_VALUE_CHANGED);
    }
    return grid;
}

uint16_t lvexColorPickerHSV::getHue() { return (uint16_t)lv_slider_get_value(hsvSliders[hueInx]); };
uint8_t  lvexColorPickerHSV::getSat() { return (uint8_t )lv_slider_get_value(hsvSliders[satInx]); };
uint8_t  lvexColorPickerHSV::getVal() { return (uint8_t )lv_slider_get_value(hsvSliders[valInx]); };

void lvexColorPickerHSV::ChangeColor()
{
    if (suppressColorChanged)
        return;
    auto color = lv_color_hsv_to_rgb(getHue(), getSat(), getVal());
    currentColor = color;
    colorClient->ColorChanged(color);
    // lv_obj_set_style_bg_color(panelSample2, color, 0);
    // flogv("color: %X  colorHue: %d  colorSat: %d  colorVal: %d", lv_color_to_int(color), colorHue, colorSaturation, colorValue);
    // auto hsv = rgb_to_hsv(color);
    // dumpHSV("color to HSV:", hsv);
    // dumpRGB("back to color:", hsv_to_rgb(hsv));
    // if (colorPickerControl)
    //     colorPickerControl->color = color;
}

void lvexColorPickerHSV::setSliderColor(HSVinx inx)
{
    auto slider = hsvSliders[inx];
    switch (inx)
    {
    case hueInx: // hue
        {
            lv_color_t color = lv_color_hsv_to_rgb(getHue(), 100, 100);
            lv_obj_set_style_bg_color(slider, color, LV_PART_MAIN);
            lv_obj_set_style_bg_color(slider, color, LV_PART_INDICATOR);
        }
        break;

    case valInx: // val
        {
            auto color1 = lv_color_hsv_to_rgb(getHue(), getSat(), 0);
            auto color2 = lv_color_hsv_to_rgb(getHue(), getSat(), 100);
            lv_obj_set_style_bg_color(slider, color1, LV_PART_MAIN);
            lv_obj_set_style_bg_grad_color(slider, color2, LV_PART_MAIN);
            lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_HOR, LV_PART_MAIN);
            lv_obj_set_style_bg_color(slider, color1, LV_PART_INDICATOR);
            lv_obj_set_style_bg_grad_color(slider, color2, LV_PART_INDICATOR);
            lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        }
        break;
    
    case satInx: // sat
        {
            auto color1 = lv_color_hsv_to_rgb(getHue(), 0, getVal());
            auto color2 = lv_color_hsv_to_rgb(getHue(), 100, getVal());
            lv_obj_set_style_bg_color(slider, color1, LV_PART_MAIN);
            lv_obj_set_style_bg_grad_color(slider, color2, LV_PART_MAIN);
            lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_HOR, LV_PART_MAIN);
            lv_obj_set_style_bg_color(slider, color1, LV_PART_INDICATOR);
            lv_obj_set_style_bg_grad_color(slider, color2, LV_PART_INDICATOR);
            lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        }
        break;;
    }
}

void lvexColorPickerHSV::setHue(uint16_t h)
{
    lv_slider_set_value(hsvSliders[hueInx], h, LV_ANIM_OFF);
    setSliderColor(hueInx);
    setSliderColor(satInx);
    setSliderColor(valInx);
    lv_obj_t* lbl = lv_obj_get_child_by_id(grid, (void*)(hueInx * 100 + 1));
    lv_label_set_text_fmt(lbl, "%d", (int)h);
    ChangeColor();
}

void lvexColorPickerHSV::setSat(uint8_t s)
{
    lv_slider_set_value(hsvSliders[satInx], s, LV_ANIM_OFF);
    setSliderColor(satInx);
    setSliderColor(valInx);
    lv_obj_t* lbl = lv_obj_get_child_by_id(grid, (void*)(satInx * 100 + 1));
    lv_label_set_text_fmt(lbl, "%d", (int)s);
    ChangeColor();
}

void lvexColorPickerHSV::setVal(uint8_t v)
{
    lv_slider_set_value(hsvSliders[valInx], v, LV_ANIM_OFF);
    setSliderColor(satInx);
    setSliderColor(valInx);
    lv_obj_t* lbl = lv_obj_get_child_by_id(grid, (void*)(valInx * 100 + 1));
    lv_label_set_text_fmt(lbl, "%d", (int)v);
    ChangeColor();
}

void lvexColorPickerHSV::EventFired(lv_event_t * e)
{
    auto code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t* slider = lv_event_get_target_obj(e);
        auto val = lv_slider_get_value(slider);
        auto id = (int)lv_obj_get_id(slider);
        int row = id / 100;
        switch (row)
        {
        case hueInx:
            setHue((uint16_t)val);
            break;
        
        case satInx: // sat
            setSat((uint8_t)val);
            break;

        case valInx: // val
            setVal((uint8_t)val);
            break;
        }
    }
}

void lvexColorPickerHSV::setColor(lv_color_t color)
{
    if (lv_color_eq(currentColor, color))
        return;
    // flogv("color in: %X", lv_color_to_int(color));
    auto hsv = lv_color_to_hsv(color);
    // dumpHSV("hsv in:", hsv);
    // hold notifications until full color is set
    suppressColorChanged = true;
    setHue(hsv.h);
    setSat(hsv.s);
    setVal(hsv.v);
    suppressColorChanged = false;
    ChangeColor();
}
