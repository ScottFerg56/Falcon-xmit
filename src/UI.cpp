#include <lvgl.h>
#include "..\.pio\libdeps\falcon-xmit\lvgl\src\themes\lv_theme_private.h"
#include "xmit.h"
#include "lvexColorPicker.h"
#include "LightsUI.h"
#include "MechUI.h"
#include "SoundUI.h"
#include "SystemUI.h"
#include "FLogger.h"

static lv_theme_t theme_custom;
static lv_style_t style_sw_main;
static lv_style_t style_sw_knob;
static lv_style_t style_sw_indicator;
static lv_style_t style_btn_main;
static lv_style_t style_sl_knob;

static void custom_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if (lv_obj_check_type(obj, &lv_switch_class))
    {
        lv_obj_add_style(obj, &style_sw_main, LV_PART_MAIN);
        lv_obj_add_style(obj, &style_sw_knob, LV_PART_KNOB);
        lv_obj_add_style(obj, &style_sw_indicator, LV_PART_INDICATOR | LV_STATE_CHECKED);
    }
    else if (lv_obj_check_type(obj, &lv_button_class))
    {
        lv_obj_add_style(obj, &style_btn_main, LV_PART_MAIN);
    }
    else if (lv_obj_check_type(obj, &lv_slider_class))
    {
        lv_obj_add_style(obj, &style_sl_knob, LV_PART_KNOB);
    }
}

void createCustomTheme(void)
{
    lv_style_init(&style_sw_main);
    lv_style_set_bg_color(&style_sw_main, lv_color_make(0x50, 0x50, 0x50));
    lv_style_set_bg_grad_color(&style_sw_main, lv_color_make(0xA0, 0xA0, 0xA0));
    lv_style_set_bg_grad_dir(&style_sw_main, LV_GRAD_DIR_VER);

    lv_style_init(&style_sw_knob);
    lv_style_set_bg_color(&style_sw_knob, lv_color_make(0xB0, 0xB0, 0xB0));
    lv_style_set_bg_grad_color(&style_sw_knob, lv_color_make(0x30, 0x30, 0x30));
    lv_style_set_bg_grad_dir(&style_sw_knob, LV_GRAD_DIR_VER);

    lv_style_init(&style_sw_indicator);
    lv_style_set_bg_color(&style_sw_indicator, lv_color_make(0x40, 0x40, 0x70));
    lv_style_set_bg_grad_color(&style_sw_indicator, lv_color_make(0x70, 0x70, 0xC0));
    lv_style_set_bg_grad_dir(&style_sw_indicator, LV_GRAD_DIR_VER);

    lv_style_init(&style_btn_main);
    lv_style_set_bg_color(&style_btn_main, lv_color_make(0xB0, 0xB0, 0xB0));
    lv_style_set_bg_grad_color(&style_btn_main, lv_color_make(0x50, 0x50, 0x50));
    lv_style_set_bg_grad_dir(&style_btn_main, LV_GRAD_DIR_VER);

    lv_style_init(&style_sl_knob);
    lv_style_set_bg_color(&style_sl_knob, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_color(&style_sl_knob, lv_color_make(0x30, 0x30, 0x30));
    lv_style_set_bg_grad_dir(&style_sl_knob, LV_GRAD_DIR_VER);

    // Initialize the new theme from the current theme
    lv_theme_t * th_act = lv_display_get_theme(NULL);
    theme_custom = *th_act;

    // Set the parent theme and the style apply callback for the new theme
    lv_theme_set_parent(&theme_custom, th_act);
    lv_theme_set_apply_cb(&theme_custom, custom_theme_apply_cb);

    // Assign the new theme to the current display
    lv_display_set_theme(NULL, &theme_custom);
}

void createUI(Root& root)
{
    createCustomTheme();
    
    // Create a Tab view
    auto tabview = lv_tabview_create(lv_screen_active());
    LV_IMG_DECLARE(FalconBG);
    lv_obj_set_style_bg_image_src(tabview, &FalconBG, 0);
    auto bar = lv_tabview_get_tab_bar(tabview);
    lv_obj_set_style_bg_opa(bar, LV_OPA_70, 0);

    lv_tabview_set_tab_bar_size(tabview, 40);
    lv_obj_remove_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(tabview, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    
    // Add tabs
    auto tabLights = lv_tabview_add_tab(tabview, "Lights");
    // lv_obj_set_style_bg_opa(tabLights, LV_OPA_0, 0);
    
    // scrolling the tabs makes UI interaction tedious
    lv_obj_remove_flag(tabLights, LV_OBJ_FLAG_SCROLLABLE);
    LightsUI::GetInstance().Create(tabLights, root);

    // pre-create the color picker to preload memory
    lvexColorPicker::GetInstance().Create();

    auto tabMech = lv_tabview_add_tab(tabview, "Mech");
    lv_obj_remove_flag(tabMech, LV_OBJ_FLAG_SCROLLABLE);
    MechUI::GetInstance().Create(tabMech, root);

    auto tabSound = lv_tabview_add_tab(tabview, "Sound");
    lv_obj_remove_flag(tabSound, LV_OBJ_FLAG_SCROLLABLE);
    SoundUI::GetInstance().Create(tabSound, root);

    auto tabSystem = lv_tabview_add_tab(tabview, "System");
    lv_obj_remove_flag(tabSystem, LV_OBJ_FLAG_SCROLLABLE);
    SystemUI::GetInstance().Create(tabSystem, root);

    lv_tabview_set_active(tabview, 0, LV_ANIM_OFF);
}
