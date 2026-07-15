#include "bitblocks_app.h"

#include <lvgl.h>

LV_FONT_DECLARE(monogram_16);
LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);

namespace {
lv_obj_t* root = nullptr;

lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

void base(lv_obj_t* obj, uint32_t fill, uint32_t border = 0, int borderWidth = 0) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(fill), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, 0, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

lv_obj_t* panel(lv_obj_t* parent, int x, int y, int w, int h, uint32_t fill, uint32_t border, int bw = 2) {
  lv_obj_t* obj = lv_obj_create(parent); base(obj, fill, border, bw); lv_obj_set_pos(obj, x, y); lv_obj_set_size(obj, w, h); return obj;
}

lv_obj_t* text(lv_obj_t* parent, const char* value, int x, int y, const lv_font_t* font, uint32_t ink) {
  lv_obj_t* label = lv_label_create(parent);lv_label_set_text(label, value);lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_font(label, font, 0);lv_obj_set_style_text_color(label, color(ink), 0);return label;
}

void sample_block(lv_obj_t* parent, int x, int y, int w, uint32_t fill, uint32_t outline, const char* label) {
  lv_obj_t* block = panel(parent, x, y, w, 31, fill, outline, 3);
  panel(block, 20, 0, 22, 5, 0x242424, 0x242424, 0);
  panel(block, 24, 25, 20, 6, fill, outline, 2);
  panel(block, 5, 4, w - 10, 2, 0x89A1DD, 0x89A1DD, 0);
  text(block, label, 10, 8, &monogram_20, 0xFFFFFF);
}
}

void BitBlocksApp::create() {
  root = lv_obj_create(nullptr);base(root, 0xD9DBE3);lv_obj_set_size(root, 480, 320);lv_screen_load(root);

  lv_obj_t* sidebar = panel(root, 4, 4, 146, 312, 0xD9DBE3, 0x34302D, 0);
  lv_obj_t* category = panel(sidebar, 3, 3, 140, 37, 0x4D69B2, 0x29365F, 3);
  text(category, "+  MOVEMENT", 10, 8, &monogram_20, 0xFFFFFF);
  lv_obj_t* palette = panel(sidebar, 3, 44, 140, 168, 0x242424, 0x382D27, 3);
  sample_block(palette, 8, 12, 122, 0x4D69B2, 0x29365F, "move [10] steps");
  sample_block(palette, 8, 56, 122, 0x4D69B2, 0x29365F, "turn [15] deg");
  sample_block(palette, 8, 100, 122, 0x4D69B2, 0x29365F, "set dir [90]");
  text(sidebar, "OUTPUT PREVIEW", 5, 217, &monogram_16, 0x30384B);
  lv_obj_t* preview = panel(sidebar, 3, 235, 140, 74, 0xFAFAFA, 0x29231F, 3);
  text(preview, "wilderness + capybara", 8, 27, &monogram_16, 0x30384B);

  text(root, "BITSLATE - CODE WORKSPACE", 161, 5, &monogram_20, 0x27324B);
  lv_obj_t* workspace = panel(root, 156, 31, 320, 285, 0xFFFFFF, 0x34302D, 2);
  sample_block(workspace, 55, 58, 190, 0xE3B43E, 0x836619, "when [PLAY] clicked");
  sample_block(workspace, 55, 90, 155, 0xCE762D, 0x754119, "wait [0.5] secs");
  lv_obj_t* play = panel(workspace, 248, 232, 28, 36, 0x4B843D, 0x171820, 3);
  text(play, ">", 9, 7, &monogram_24, 0xFFFFFF);
  lv_obj_t* stop = panel(workspace, 282, 232, 28, 36, 0xDF244B, 0x171820, 3);
  text(stop, "[]", 6, 8, &monogram_20, 0xFFFFFF);
}

void BitBlocksApp::destroy() { root = nullptr; }
