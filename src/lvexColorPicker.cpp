#include "lvexColorPicker.h"
#include "FLogger.h"

void lvexColorPicker::Create()
{
    window = lv_win_create(lv_screen_active());
    lv_obj_set_scroll_dir(window, LV_DIR_NONE);
    auto hdr = lv_win_get_header(window);
    lv_obj_set_height(hdr, 40);
    lblTitle = lv_win_add_title(window, "");

    lv_obj_t* btn = lv_win_add_button(window, LV_SYMBOL_CLOSE, 60);
    AddEvent(btn, LV_EVENT_CLICKED);

    lv_obj_t * cont = lv_win_get_content(window);  /*Content can be added here*/
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
    //lv_obj_set_style_pad_column(grid, 30, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_size(grid, 800, 440);
    lv_obj_set_align(grid, LV_ALIGN_CENTER);

    lv_obj_set_grid_cell(picker, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(pal, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(panelSample, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    setColor(lv_color_make(0x50, 0x50, 0x50));
}

void lvexColorPicker::Show(lv_color_t color, const char*title, ColorClient* client)
{
    if (!colorPicker.window)
        colorPicker.Create();
    colorPicker.colorClient = client;
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);

    // attach the dialog window to current active screen
    lv_obj_set_parent(colorPicker.window, active_screen);

    lv_label_set_text(colorPicker.lblTitle, title);
    colorPicker.setColor(color);
    lv_obj_remove_flag(colorPicker.window, LV_OBJ_FLAG_HIDDEN); /* show the dialog */
    lv_obj_scroll_to_y(colorPicker.window, 0, LV_ANIM_ON);
}

void lvexColorPicker::EventFired(lv_event_t* e)
{
    auto code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        lv_obj_add_flag(window, LV_OBJ_FLAG_HIDDEN);
        colorClient = nullptr;
        break;
    }
}

void lvexColorPicker::setColor(lv_color_t color)
{
    if (lv_color_eq(currentColor, color))
        return;
    currentColor = color;
    pickerHSV.setColor(color);
    lv_obj_set_style_bg_color(panelSample, color, 0);
    if (colorClient)
        colorClient->ColorChanged(color);
}

void lvexColorPicker::ColorChanged(lv_color_t color)
{
    setColor(color);
}

lvexColorPicker lvexColorPicker::colorPicker;
