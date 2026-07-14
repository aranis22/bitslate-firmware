#include "geography_menu.h"

#include <lvgl.h>

#include "assets/UI/home/ui_home_assets.h"
#include "button_input.h"
#include "ui/navigation/app_manager.h"

LV_FONT_DECLARE(monogram_16);
LV_FONT_DECLARE(monogram_24);
LV_FONT_DECLARE(monogram_28);
LV_FONT_DECLARE(monogram_32);

static lv_color_t color(unsigned int hex) { return lv_color_hex(hex); }

static void clear_obj(lv_obj_t *obj) {
  lv_obj_remove_style_all(obj);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t *rect(lv_obj_t *parent, int x, int y, int w, int h, unsigned int fill) {
  lv_obj_t *obj = lv_obj_create(parent);
  clear_obj(obj);
  lv_obj_set_pos(obj, x, y); lv_obj_set_size(obj, w, h);
  lv_obj_set_style_bg_color(obj, color(fill), 0); lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(obj, 0, 0); lv_obj_set_style_radius(obj, 0, 0);
  return obj;
}

static lv_obj_t *outlined_rect(lv_obj_t *parent, int x, int y, int w, int h, unsigned int fill, unsigned int border, int bw) {
  lv_obj_t *obj = rect(parent, x, y, w, h, fill);
  lv_obj_set_style_border_color(obj, color(border), 0); lv_obj_set_style_border_width(obj, bw, 0);
  return obj;
}

static lv_obj_t *label(lv_obj_t *parent, const char *text, const lv_font_t *font, int x, int y, int w, int h) {
  lv_obj_t *obj = lv_label_create(parent);
  clear_obj(obj); lv_label_set_text(obj, text); lv_obj_set_pos(obj, x, y); lv_obj_set_size(obj, w, h);
  lv_obj_set_style_text_font(obj, font, 0); lv_obj_set_style_text_color(obj, color(0xFFFFFF), 0);
  lv_obj_set_style_text_letter_space(obj, 2, 0); lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
  return obj;
}

static lv_obj_t *image(lv_obj_t *parent, const lv_image_dsc_t *source, int x, int y, bool clickable) {
  lv_obj_t *obj = lv_image_create(parent);
  clear_obj(obj); lv_image_set_src(obj, source); lv_obj_set_pos(obj, x, y);
  if(clickable) lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
  return obj;
}

static void app_card_clicked(lv_event_t *event) {
  if(lv_event_get_code(event) == LV_EVENT_CLICKED) app_launch((app_id_t)(intptr_t)lv_event_get_user_data(event));
}

static void draw_status_bar(lv_obj_t *screen) {
  outlined_rect(screen, 0, 0, 480, 45, 0x25368E, 0xFFFFFF, 3);
  rect(screen, 137, 0, 3, 45, 0xFFFFFF); rect(screen, 207, 0, 3, 45, 0xFFFFFF);
  rect(screen, 290, 0, 3, 45, 0xFFFFFF); rect(screen, 392, 0, 3, 45, 0xFFFFFF);
  image(screen, &ui_home_capybara, 0, -1, false); label(screen, "test", &monogram_32, 53, 13, 80, 26);
  image(screen, &ui_home_mail, 145, 3, false); image(screen, &ui_home_battery, 400, 6, false);
  lv_obj_t *badge = outlined_rect(screen, 188, -1, 18, 18, 0xEAF06A, 0x111111, 2);
  lv_obj_set_style_radius(badge, 9, 0);
  lv_obj_t *badge_label = lv_label_create(badge); clear_obj(badge_label); lv_label_set_text(badge_label, "1");
  lv_obj_set_style_text_font(badge_label, &monogram_16, 0); lv_obj_set_style_text_color(badge_label, color(0x111111), 0); lv_obj_center(badge_label);
  label(screen, "1/13", &monogram_28, 229, 12, 57, 24); label(screen, "7:33pm", &monogram_28, 309, 12, 76, 24);
}

void geography_menu_create(void) {
  lv_obj_t *screen = lv_obj_create(NULL);
  clear_obj(screen); lv_obj_set_size(screen, 480, 320);
  lv_obj_set_style_bg_color(screen, color(0x202F8A), 0); lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  image(screen, &ui_home_background_1, 0, 0, false); draw_status_bar(screen);
  lv_obj_t *states = image(screen, &ui_geography_states_quiz, 100, 108, true);
  lv_obj_t *flags = image(screen, &ui_geography_flag_quiz, 250, 108, true);
  lv_obj_add_event_cb(states, app_card_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)APP_ID_US_STATES_QUIZ);
  lv_obj_add_event_cb(flags, app_card_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)APP_ID_WORLD_FLAGS_QUIZ);
  label(screen, "US states\nquiz", &monogram_24, 90, 242, 150, 52);
  label(screen, "World Flags\nquiz", &monogram_24, 240, 242, 150, 52);
  button_input_register_active_app(screen, UI_STATE_GEOGRAPHY_MENU, geography_menu_destroy);
  lv_screen_load(screen);
}

void geography_menu_destroy(void) {
}
