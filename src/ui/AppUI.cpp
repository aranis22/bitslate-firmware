#include "AppUI.h"
#include <lvgl.h>

namespace AppUI {

static void makeCard(lv_obj_t* parent, const char* title, const char* subtitle) {
    lv_obj_t* card = lv_obj_create(parent);
    lv_obj_set_size(card, 135, 95);
    lv_obj_set_style_radius(card, 18, 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_pad_all(card, 10, 0);

    lv_obj_t* label = lv_label_create(card);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t* sub = lv_label_create(card);
    lv_label_set_text(sub, subtitle);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
    lv_obj_align(sub, LV_ALIGN_BOTTOM_LEFT, 0, 0);
}

void createHomeScreen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_scr_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0xF7F3E8), 0);

    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "BitSlate");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 18);

    lv_obj_t* subtitle = lv_label_create(screen);
    lv_label_set_text(subtitle, "learn · code · draw · play");
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_18, 0);
    lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

    lv_obj_t* grid = lv_obj_create(screen);
    lv_obj_set_size(grid, 300, 250);
    lv_obj_align(grid, LV_ALIGN_CENTER, 0, 35);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(grid, 12, 0);

    makeCard(grid, "Lessons", "guided labs");
    makeCard(grid, "Code", "blocks + rules");
    makeCard(grid, "Draw", "stylus canvas");
    makeCard(grid, "Play", "mini games");
}

}