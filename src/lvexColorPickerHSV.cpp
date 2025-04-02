#include "lvexColorPickerHSV.h"
#include "FLogger.h"
#include <cmath>

// HSV needs better precision and better rounding to be able to roundtrip RGB values
// in and out of the UI.
// The LVGL hsv struct is based on byte values and the conversions perform no rounding.
// The following conversion routines, based on doubles, are dervied from the following article:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

fhsv rgb_to_fhsv(lv_color_t in)
{
    double  r = in.red / 255.0;
    double  g = in.green / 255.0;
    double  b = in.blue / 255.0;

    fhsv        out;
    double      min, max, delta;

    min = r < g ? r : g;
    min = min < b ? min : b;

    max = r > g ? r : g;
    max = max > b ? max : b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( g - b ) / delta;        // between yellow & magenta
    else
    if( g >= max )
        out.h = 2.0 + ( b - r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( r - g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

lv_color_t fhsv_to_rgb(fhsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    double      r, g, b;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        r = in.v;
        g = in.v;
        b = in.v;
    }
    else
    {
        hh = in.h;
        if(hh >= 360.0) hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = in.v * (1.0 - in.s);
        q = in.v * (1.0 - (in.s * ff));
        t = in.v * (1.0 - (in.s * (1.0 - ff)));

        switch(i) {
        case 0:
            r = in.v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = in.v;
            b = p;
            break;
        case 2:
            r = p;
            g = in.v;
            b = t;
            break;

        case 3:
            r = p;
            g = q;
            b = in.v;
            break;
        case 4:
            r = t;
            g = p;
            b = in.v;
            break;
        case 5:
        default:
            r = in.v;
            g = p;
            b = q;
            break;
        }
    }

    // flogv("r: %f  g: %f  b: %f", r, g, b);

    lv_color_t out;
    out.red   = (uint8_t)round(r * 255);
    out.green = (uint8_t)round(g * 255);
    out.blue  = (uint8_t)round(b * 255);
    return out;     
}

#define dumpRGB(label, rgb) flogv("%s  red: %d  green: %d  blue: %d", label, rgb.red, rgb.green, rgb.blue)

#define dumpHSV(label, hsv) flogv("%s  hue: %f  sat: %f  val: %f", label, hsv.h, hsv.s, hsv.v)

lv_obj_t* lvexColorPickerHSV::Create(lv_obj_t* parent, ColorClient* client)
{
    colorClient = client;

    static int32_t col_dsc[] = {70, 70, 340, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    // Create a grid container
    grid = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_size(grid, 540, 160);
    lv_obj_set_style_bg_color(grid, lv_color_black(), 0);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 10, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);

    for (int row = 0; row < 3; row++)
    {
        auto lbl = lv_label_create(grid);
        lv_label_set_text(lbl, (row == hueInx) ? "Hue:" : (row == satInx) ? "Sat:" : "Brt:");
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, row, 1);

        lbl = lv_label_create(grid);
        lv_label_set_text(lbl, "0.0");
        lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(lbl, (void*)(row * 100 + 1));

        auto slider = hsvSliders[row] = lv_slider_create(grid);
        lv_obj_set_size(slider, 300, 30);
        lv_slider_set_range(slider, 0, 300);
        lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_id(slider, (void*)(row * 100 + 2));
        lv_obj_set_style_bg_opa(slider, LV_OPA_100, LV_PART_MAIN);
        // lv_obj_set_style_bg_color(slider, lv_palette_main(LV_PALETTE_GREY), LV_PART_KNOB);
        AddEvent(slider, LV_EVENT_VALUE_CHANGED);
    }
    return grid;
}

bool lvexColorPickerHSV::isBusy()
{
    for (int i = hueInx; i <= valInx; i++)
    {
        if (lv_slider_is_dragged(hsvSliders[i]))
            return true;
    }
    return false;
}

void lvexColorPickerHSV::ChangeColor()
{
    // dumpHSV("change: ", HSV);
    // dumpRGB("change: ", RGB);
    colorClient->ColorChanged(RGB);
}

void lvexColorPickerHSV::setSliderColor(HSVinx inx)
{
    auto slider = hsvSliders[inx];
    switch (inx)
    {
    case hueInx: // hue
        {
            // hue slider solid color of fully saturated/bright hue
            lv_color_t color = fhsv_to_rgb({ HSV.h, 1.0, 1.0 });
            lv_obj_set_style_bg_color(slider, color, LV_PART_MAIN);
            lv_obj_set_style_bg_color(slider, color, LV_PART_INDICATOR);
        }
        break;

    case valInx: // val
        {
            // value slider color gradient of hue/sat from 0 to 1 value
            auto color1 = fhsv_to_rgb({ HSV.h, HSV.s, 0.0 });
            auto color2 = fhsv_to_rgb({ HSV.h, HSV.s, 1.0 });
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
            // saturation slider color gradient of hue/val from 0 to 1 saturation
            auto color1 = fhsv_to_rgb({ HSV.h, 0.0, HSV.v });
            auto color2 = fhsv_to_rgb({ HSV.h, 1.0, HSV.v });
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

void lvexColorPickerHSV::setSliderValue(HSVinx inx)
{
    auto fv = inx == hueInx ? HSV.h : (inx == satInx ? HSV.s : HSV.v);
    // slider range is [0,300]
    // hue angle is in degrees [0,360]
    // saturation and value are [0,1]
    auto v = (int32_t)round(fv / (inx == hueInx ? 360.0 : 1) * 300.0);
    // flogv("set slider: %d", v);
    lv_slider_set_value(hsvSliders[inx], v, LV_ANIM_OFF);
    setSliderUI(inx);
}

void lvexColorPickerHSV::setSliderUI(HSVinx inx)
{
    if (inx == hueInx)
        setSliderColor(hueInx);     // hue color only depends on itself
    setSliderColor(satInx);         // others depend on all HSV
    setSliderColor(valInx);
    auto fv = inx == hueInx ? HSV.h : (inx == satInx ? HSV.s : HSV.v);
    lv_obj_t* lbl = lv_obj_get_child_by_id(grid, (void*)(inx * 100 + 1));
    lv_label_set_text(lbl, String(fv, inx == hueInx ? 1 : 3).c_str());
}

void lvexColorPickerHSV::EventFired(lv_event_t * e)
{
    auto code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t* slider = lv_event_get_target_obj(e);
        auto val = lv_slider_get_value(slider) / 300.0;
        auto id = (int)lv_obj_get_id(slider);
        auto row = (HSVinx)(id / 100);
        switch (row)
        {
        case hueInx:
            HSV.h = val * 360;  // [0..360]
            break;
        case satInx: // sat
            HSV.s = val;        // [0..1]
            break;
        case valInx: // val
            HSV.v = val;        // [0..1]
            break;
        }
        RGB = fhsv_to_rgb(HSV);
        setSliderUI(row);
        ChangeColor();
    }
}

void lvexColorPickerHSV::setColor(lv_color_t color)
{
    if (lv_color_eq(RGB, color))
        return;

    RGB = color;
    HSV = rgb_to_fhsv(color);

    // dumpRGB("rgb in:  ", RGB);    
    // dumpHSV("hsv in:  ", HSV);
    lv_color_t rgb2 = fhsv_to_rgb(HSV);
    // dumpRGB("rgb out: ", rgb2);

    setSliderValue(hueInx);
    setSliderValue(satInx);
    setSliderValue(valInx);
    ChangeColor();
}
