#include <lvgl.h>
#include "xmit.h"
#include "SystemUI.h"
#include "SoundUI.h"
#include "FLogger.h"
#include "SD.h"

SystemUI SystemUI::systemUI;

enum controlIds
{
    noZero,
    btnSave,
    btnRemove,
    ddFiles,
    btnDnld,
    btnDeleteFile,
    lblFreeSpace,
    btnRestart,
};

std::vector<String> SoundFiles()
{
    std::vector<String> files;
    File root = SD.open("/", 0);
    if (!root)
    {
        floge("Failed to open root directory");
        return files;
    }
    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            String name(file.name());
            if (name.endsWith(".mp3"))
            {
                name.remove(name.length() - 4);
                files.push_back(name);
            }
        }
        file = root.openNextFile();
    }
    std::sort(files.begin(), files.end());
    return files;
}

void SystemUI::Create(lv_obj_t* parent, Root& root)
{
    pRoot = &root;
    //
    // System grid
    //
    static int32_t col_dsc[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };
    static int32_t row_dsc[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };

    // Create a grid container
    grid = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_size(grid, 800, 330);
    lv_obj_align(grid, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(grid, 20, 0);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_scroll_dir(grid, LV_DIR_NONE);
    lv_obj_set_style_bg_opa(grid, LV_OPA_70, 0);

    // Save startup state button
    auto lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Save startup state");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    auto btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_id(btn, (void*)(btnSave));
    AddEvent(btn, LV_EVENT_CLICKED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_SAVE);
    lv_obj_center(lbl);
    // Remove startup state button
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Remove startup state");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_id(btn, (void*)(btnRemove));
    AddEvent(btn, LV_EVENT_CLICKED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_TRASH);
    lv_obj_center(lbl);

    // Sound files dropdown and buttons
    auto ddf = lv_dropdown_create(grid);
    lv_obj_set_id(ddf, (void*)ddFiles);
    lv_obj_set_size(ddf, 400, 30);
    lv_obj_set_grid_cell(ddf, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    // Set the text alignment to center
    auto list = lv_dropdown_get_list(ddf);
    lv_obj_set_style_text_align(list, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_opa(ddf, LV_OPA_0, 0);
    auto files = SoundFiles();
    lv_dropdown_clear_options(ddf);
    for (auto file : files)
    {
        lv_dropdown_add_option(ddf, file.c_str(), LV_DROPDOWN_POS_LAST);
    }
    lv_dropdown_set_selected(ddf, 0);
    // download button
    btn = lv_btn_create(grid);
    lv_obj_set_id(btn, (void*)btnDnld);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_DOWNLOAD);
    lv_obj_center(lbl);
    AddEvent(btn, LV_EVENT_CLICKED);
    
    // delete button
    btn = lv_btn_create(grid);
    lv_obj_set_id(btn, (void*)btnDeleteFile);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_TRASH);
    lv_obj_center(lbl);
    AddEvent(btn, LV_EVENT_LONG_PRESSED);

    // free space label and value
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "File Free Space");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "00000000");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 1, 2, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_id(lbl, (void*)lblFreeSpace);

    // restart label and button
    lbl = lv_label_create(grid);
    lv_label_set_text(lbl, "Restart");
    lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    btn = lv_button_create(grid);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_id(btn, (void*)(btnRestart));
    AddEvent(btn, LV_EVENT_LONG_PRESSED);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_POWER);
    lv_obj_center(lbl);
}

void SystemUI::EventFired(lv_event_t * e)
{
    auto obj = lv_event_get_target_obj(e);
    auto id = (int)lv_obj_get_id(obj);
    auto code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        switch (id)
        {
            case btnSave:
                {
                    pRoot->SendCmd(">R");
                }
                break;
            case btnRemove:
                {
                    pRoot->SendCmd("-R");
                }
                break;
            case btnDnld:   // download selected sound file to rcvr
                {
                    auto dd = lv_obj_get_child_by_id(grid, (void*)ddFiles);
                    auto value = lv_dropdown_get_selected(dd);
                    // file transfer
                    auto files = SoundFiles();
                    if (files.size() == 0)
                        return;
                    flogv("downloading file: %s", files[value].c_str());
                    pRoot->GetAgent()->StartFileTransfer("/" + files[value] + ".mp3");
                }
                break;
        }
    case LV_EVENT_LONG_PRESSED:
        switch (id)
        {
            case btnDeleteFile:   // delete selected sound file
                {
                    auto dd = lv_obj_get_child_by_id(grid, (void*)ddFiles);
                    auto value = lv_dropdown_get_selected(dd);
                    auto files = SoundFiles();
                    if (value >= files.size())
                        return;
                    flogv("deleting file: %s", files[value].c_str());
                    SD.remove("/" + files[value] + ".mp3");
                    files = SoundFiles();
                    lv_dropdown_clear_options(dd);
                    for (auto file : files)
                    {
                        lv_dropdown_add_option(dd, file.c_str(), LV_DROPDOWN_POS_LAST);
                    }
                    lv_dropdown_set_selected(dd, 0);
                }
                break;
            case btnRestart:
                {
                    // send set to restart property with 'PIN' value 1234
                    pRoot->SendCmd("=x1234");
                }
                break;
        }
        break;
    }
}

void SystemUI::PropertyUpdate(OMProperty* prop)
{
    switch (prop->Id)
    {
        case 'f':   // free space
            {
                auto value = ((OMPropertyLong*)prop)->Value;
                auto lbl = lv_obj_get_child_by_id(grid, (void*)lblFreeSpace);
                lv_label_set_text(lbl, String(value).c_str());
            }
            break;
    }
}
