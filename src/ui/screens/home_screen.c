#include "home_screen.h"

#include "assets/UI/home/ui_home_assets.h"
#include "button_input.h"
#include "ui/navigation/app_manager.h"

LV_FONT_DECLARE(monogram_16);
LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);
LV_FONT_DECLARE(monogram_28);
LV_FONT_DECLARE(monogram_32);

enum {
  SCREEN_W = 480,
  SCREEN_H = 320,

  STATUS_BAR_X = 0,
  STATUS_BAR_Y = 0,
  STATUS_BAR_W = 480,
  STATUS_BAR_H = 45,
  STATUS_BORDER_W = 3,

  PROFILE_PANEL_X = 0,
  PROFILE_PANEL_W = 138,
  MAIL_PANEL_X = 138,
  MAIL_PANEL_W = 70,
  PAGE_PANEL_X = 208,
  PAGE_PANEL_W = 83,
  TIME_PANEL_X = 291,
  TIME_PANEL_W = 102,
  BATTERY_PANEL_X = 393,
  BATTERY_PANEL_W = 87,

  CAPYBARA_X = 0,
  CAPYBARA_Y = -1,
  USERNAME_X = 53,
  USERNAME_Y = 13,

  MAIL_X = 145,
  MAIL_Y = 3,
  BADGE_X = 188,
  BADGE_Y = -1,
  BADGE_SIZE = 18,

  PAGE_X = 229,
  PAGE_Y = 12,
  TIME_X = 309,
  TIME_Y = 12,
  BATTERY_X = 400,
  BATTERY_Y = 6,

  CARD_REST_OFFSET_Y = 23,
  CARD_WIDTH = 220,
  CARD_HEIGHT = 220,
  CARD_VIEWPORT_X = (SCREEN_W - CARD_WIDTH) / 2,
  CARD_VIEWPORT_REST_Y = (SCREEN_H - CARD_HEIGHT) / 2 + CARD_REST_OFFSET_Y,
  CARD_STRIP_WIDTH = CARD_WIDTH * 2,
  CARD_IDLE_RANGE_Y = 4,
  CARD_IDLE_HALF_CYCLE_MS = 400,
  CARD_LABEL_X = 170,
  CARD_LABEL_Y = 288,
  CARD_LABEL_W = 140,
  CARD_LABEL_H = 32,
  CARD_TRANSITION_MS = 250,

  ARROW_SCALE = 333,
  LEFT_ARROW_X = 17,
  LEFT_ARROW_Y = 142,
  RIGHT_ARROW_X = 368,
  RIGHT_ARROW_Y = 142,

  FLOOR_Y = 303,
  FLOOR_H = 17,

  LEFT_TOWER_X = 0,
  LEFT_TOWER_Y = 46,
  LEFT_TOWER_W = 92,
  LEFT_TOWER_H = 257,
  LEFT_WALL_X = 92,
  LEFT_WALL_Y = 46,
  LEFT_WALL_W = 64,
  LEFT_WALL_H = 257,

  RIGHT_KEEP_X = 357,
  RIGHT_KEEP_Y = 105,
  RIGHT_KEEP_W = 48,
  RIGHT_KEEP_H = 198,
  RIGHT_CASTLE_X = 405,
  RIGHT_CASTLE_Y = 151,
  RIGHT_CASTLE_W = 75,
  RIGHT_CASTLE_H = 152,
};

typedef void (*launcher_launch_cb_t)(void);

typedef struct {
  const lv_image_dsc_t *image;
  const char *label;
  launcher_launch_cb_t launch;
} launcher_card_dsc_t;

typedef struct {
  lv_obj_t *viewport;
  lv_obj_t *strip;
  lv_obj_t *left_slot;
  lv_obj_t *right_slot;
  lv_obj_t *card_label;
  lv_obj_t *page_counter;
  lv_obj_t *left_arrow;
  lv_obj_t *right_arrow;
  uint32_t selected_index;
  uint32_t pending_index;
  bool transition_in_progress;
  bool launch_in_progress;
} launcher_state_t;

static void launch_chess(void);
static void launch_world_map(void);
static void launch_physics_menu(void);
static void launch_chemistry_menu(void);
static void launch_math_menu(void);
static void launch_geography_menu(void);
static void set_strip_x(void *obj, int32_t x);

static const launcher_card_dsc_t launcher_cards[] = {
  {&ui_home_chess_icon, "Chess", launch_chess},
  {&ui_home_world_map_card, "World map", launch_world_map},
  {&ui_home_physics_card, "Physics", launch_physics_menu},
  {&ui_home_chemistry_card, "Chemistry", launch_chemistry_menu},
  {&ui_home_math_card, "Math", launch_math_menu},
  {&ui_home_geography_card, "Geography", launch_geography_menu},
  {&ui_home_settings_card, "Settings", NULL},
};

static launcher_state_t launcher = {0};

static lv_color_t color(unsigned int hex) {
  return lv_color_hex(hex);
}

static void clear_obj(lv_obj_t *obj) {
  lv_obj_remove_style_all(obj);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t *rect(lv_obj_t *parent, int x, int y, int w, int h, unsigned int fill) {
  lv_obj_t *obj = lv_obj_create(parent);
  clear_obj(obj);
  lv_obj_set_pos(obj, x, y);
  lv_obj_set_size(obj, w, h);
  lv_obj_set_style_bg_color(obj, color(fill), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
  lv_obj_set_style_radius(obj, 0, 0);
  return obj;
}

static lv_obj_t *outlined_rect(lv_obj_t *parent, int x, int y, int w, int h, unsigned int fill, unsigned int border, int bw) {
  lv_obj_t *obj = rect(parent, x, y, w, h, fill);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, bw, 0);
  return obj;
}

static lv_obj_t *label(lv_obj_t *parent, const char *text, const lv_font_t *font, int x, int y, int w, int h) {
  lv_obj_t *obj = lv_label_create(parent);
  clear_obj(obj);
  lv_label_set_text(obj, text);
  lv_obj_set_pos(obj, x, y);
  lv_obj_set_size(obj, w, h);
  lv_obj_set_style_text_font(obj, font, 0);
  lv_obj_set_style_text_color(obj, color(0xFFFFFF), 0);
  lv_obj_set_style_text_letter_space(obj, 2, 0);
  lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
  return obj;
}

static lv_obj_t *image(lv_obj_t *parent, const lv_image_dsc_t *src, int x, int y) {
  lv_obj_t *obj = lv_image_create(parent);
  clear_obj(obj);
  lv_image_set_src(obj, src);
  lv_obj_set_pos(obj, x, y);
  return obj;
}

static lv_obj_t *scaled_image(lv_obj_t *parent, const lv_image_dsc_t *src, int x, int y, int scale) {
  lv_obj_t *obj = image(parent, src, x, y);
  lv_image_set_scale(obj, scale);
  return obj;
}

static void set_viewport_y(void *obj, int32_t y) {
  lv_obj_set_y((lv_obj_t *)obj, y);
}

static void start_card_idle_animation(void) {
  lv_anim_t animation;
  lv_anim_init(&animation);
  lv_anim_set_var(&animation, launcher.viewport);
  lv_anim_set_exec_cb(&animation, set_viewport_y);
  lv_anim_set_values(&animation,
                     CARD_VIEWPORT_REST_Y - CARD_IDLE_RANGE_Y,
                     CARD_VIEWPORT_REST_Y + CARD_IDLE_RANGE_Y);
  lv_anim_set_duration(&animation, CARD_IDLE_HALF_CYCLE_MS);
  lv_anim_set_reverse_duration(&animation, CARD_IDLE_HALF_CYCLE_MS);
  lv_anim_set_repeat_count(&animation, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
  lv_anim_start(&animation);
}

static void stop_card_idle_animation(void) {
  if(launcher.viewport == NULL) return;
  lv_anim_delete(launcher.viewport, set_viewport_y);
  lv_obj_set_y(launcher.viewport, CARD_VIEWPORT_REST_Y);
}

static void launch_chess(void) {
  app_launch(APP_ID_CHESS);
}

static void launch_world_map(void) {
  app_launch(APP_ID_WORLD_MAP);
}

static void launch_physics_menu(void) {
  app_open_menu(APP_MENU_PHYSICS);
}

static void launch_chemistry_menu(void) {
  app_open_menu(APP_MENU_CHEMISTRY);
}

static void launch_math_menu(void) {
  app_open_menu(APP_MENU_MATH);
}

static void launch_geography_menu(void) {
  app_open_menu(APP_MENU_GEOGRAPHY);
}

static void launcher_card_clicked(lv_event_t *event) {
  if(lv_event_get_code(event) != LV_EVENT_CLICKED ||
     launcher.transition_in_progress || launcher.launch_in_progress) return;

  const launcher_card_dsc_t *card = &launcher_cards[launcher.selected_index];
  if(card->launch == NULL) return;

  launcher.launch_in_progress = true;
  stop_card_idle_animation();
  lv_obj_clear_flag(launcher.viewport, LV_OBJ_FLAG_CLICKABLE);
  launcher.viewport = NULL;
  card->launch();
}

static void update_page_counter(void) {
  if(launcher.page_counter != NULL) {
    lv_label_set_text_fmt(launcher.page_counter, "%lu/13", (unsigned long)(launcher.selected_index + 1));
  }
}

static void card_transition_finished(lv_anim_t *animation) {
  (void)animation;
  launcher.selected_index = launcher.pending_index;
  lv_image_set_src(launcher.left_slot, launcher_cards[launcher.selected_index].image);
  lv_image_set_src(launcher.right_slot, launcher_cards[launcher.selected_index].image);
  lv_obj_set_x(launcher.strip, 0);
  lv_label_set_text(launcher.card_label, launcher_cards[launcher.selected_index].label);
  launcher.transition_in_progress = false;
  lv_obj_add_flag(launcher.left_arrow, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(launcher.right_arrow, LV_OBJ_FLAG_CLICKABLE);
  update_page_counter();
  lv_obj_invalidate(launcher.viewport);
  start_card_idle_animation();
  button_input_set_ui_state(UI_STATE_HOME);
}

static void set_strip_x(void *obj, int32_t x) {
  lv_obj_set_x((lv_obj_t *)obj, x);
}

static void start_card_transition_animation(int start_x, int end_x) {
  lv_anim_delete(launcher.strip, set_strip_x);
  lv_anim_t animation;
  lv_anim_init(&animation);
  lv_anim_set_var(&animation, launcher.strip);
  lv_anim_set_exec_cb(&animation, set_strip_x);
  lv_anim_set_values(&animation, start_x, end_x);
  lv_anim_set_duration(&animation, CARD_TRANSITION_MS);
  lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
  lv_anim_set_completed_cb(&animation, card_transition_finished);
  lv_anim_start(&animation);
}

void home_screen_navigate(int direction) {
  if(launcher.viewport == NULL || launcher.transition_in_progress ||
     launcher.launch_in_progress || (direction != -1 && direction != 1)) return;

  int card_count = (int)(sizeof(launcher_cards) / sizeof(launcher_cards[0]));
  int next_index = ((int)launcher.selected_index + direction + card_count) % card_count;

  launcher.transition_in_progress = true;
  button_input_set_ui_state(UI_STATE_TRANSITIONING);
  launcher.pending_index = (uint32_t)next_index;
  lv_obj_clear_flag(launcher.left_arrow, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(launcher.right_arrow, LV_OBJ_FLAG_CLICKABLE);
  stop_card_idle_animation();

  if(direction > 0) {
    lv_image_set_src(launcher.left_slot, launcher_cards[launcher.selected_index].image);
    lv_image_set_src(launcher.right_slot, launcher_cards[launcher.pending_index].image);
    lv_obj_set_x(launcher.strip, 0);
    start_card_transition_animation(0, -CARD_WIDTH);
  }
  else {
    lv_image_set_src(launcher.left_slot, launcher_cards[launcher.pending_index].image);
    lv_image_set_src(launcher.right_slot, launcher_cards[launcher.selected_index].image);
    lv_obj_set_x(launcher.strip, -CARD_WIDTH);
    start_card_transition_animation(-CARD_WIDTH, 0);
  }
}

void home_navigate_left(void) {
  home_screen_navigate(-1);
}

void home_navigate_right(void) {
  home_screen_navigate(1);
}

static void launcher_arrow_clicked(lv_event_t *event) {
  if(lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  if((int)(intptr_t)lv_event_get_user_data(event) > 0) home_navigate_right();
  else home_navigate_left();
}

static void draw_bricks(lv_obj_t *screen) {
  rect(screen, 0, FLOOR_Y, SCREEN_W, FLOOR_H, 0x253A9B);
  for(int y = FLOOR_Y + 1; y < SCREEN_H; y += 6) {
    rect(screen, 0, y, SCREEN_W, 2, 0x182B78);
  }
  for(int row = 0; row < 3; ++row) {
    int y = FLOOR_Y + row * 6;
    int offset = (row % 2) ? 12 : 0;
    for(int x = -offset; x < SCREEN_W; x += 24) {
      rect(screen, x, y, 2, 6, 0x182B78);
    }
  }
}

static void draw_left_castle(lv_obj_t *screen) {
  rect(screen, LEFT_TOWER_X, LEFT_TOWER_Y, LEFT_TOWER_W, LEFT_TOWER_H, 0x253A9B);
  rect(screen, LEFT_WALL_X, LEFT_WALL_Y, LEFT_WALL_W, LEFT_WALL_H, 0x21358D);

  rect(screen, 10, 60, 18, 6, 0x3D55CE);
  rect(screen, 22, 122, 44, 6, 0x3D55CE);
  rect(screen, 19, 145, 52, 5, 0x3D55CE);
  rect(screen, 20, 251, 56, 5, 0x3D55CE);
  rect(screen, 29, 282, 43, 4, 0x3D55CE);
  rect(screen, 92, 69, 36, 4, 0x3D55CE);
  rect(screen, 99, 230, 34, 5, 0x3D55CE);

  outlined_rect(screen, 31, 72, 35, 80, 0x1D2F83, 0x3C55CF, 3);
  rect(screen, 47, 75, 3, 74, 0x3C55CF);
  rect(screen, 34, 110, 29, 3, 0x3C55CF);
  outlined_rect(screen, 31, 190, 35, 67, 0x1D2F83, 0x3C55CF, 3);
  rect(screen, 47, 193, 3, 61, 0x3C55CF);
  rect(screen, 34, 223, 29, 3, 0x3C55CF);

  for(int y = 52; y < 288; y += 16) {
    rect(screen, 86, y, 10, 8, 0x3D55CE);
  }
}

static void draw_right_castle(lv_obj_t *screen) {
  rect(screen, RIGHT_KEEP_X, RIGHT_KEEP_Y, RIGHT_KEEP_W, RIGHT_KEEP_H, 0x3048B5);
  rect(screen, RIGHT_KEEP_X + 14, RIGHT_KEEP_Y - 38, 20, 38, 0x405BD2);
  rect(screen, RIGHT_KEEP_X + 7, RIGHT_KEEP_Y - 15, 34, 18, 0x405BD2);
  rect(screen, RIGHT_KEEP_X + 20, RIGHT_KEEP_Y + 58, 7, 29, 0x1B2A72);
  rect(screen, RIGHT_KEEP_X + 19, RIGHT_KEEP_Y + 143, 10, 38, 0x1B2A72);

  rect(screen, RIGHT_CASTLE_X, RIGHT_CASTLE_Y, RIGHT_CASTLE_W, RIGHT_CASTLE_H, 0x263B9F);
  for(int x = RIGHT_CASTLE_X; x < RIGHT_CASTLE_X + RIGHT_CASTLE_W; x += 22) {
    rect(screen, x, RIGHT_CASTLE_Y - 16, 13, 18, 0x3E58CF);
  }
  rect(screen, RIGHT_CASTLE_X, RIGHT_CASTLE_Y + 33, RIGHT_CASTLE_W, 8, 0x3E58CF);
  outlined_rect(screen, RIGHT_CASTLE_X + 22, RIGHT_CASTLE_Y + 84, 31, 68, 0x1C2C78, 0x3E58CF, 3);
  rect(screen, RIGHT_CASTLE_X + 6, RIGHT_CASTLE_Y + 49, 34, 4, 0x3E58CF);
  rect(screen, RIGHT_CASTLE_X + 6, RIGHT_CASTLE_Y + 62, 34, 5, 0x3E58CF);
}

static void draw_background(lv_obj_t *screen) {
  lv_obj_set_style_bg_color(screen, color(0x202F8A), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  image(screen, &ui_home_background_1, 0, 0);
}

static void draw_status_bar(lv_obj_t *screen) {
  outlined_rect(screen, STATUS_BAR_X, STATUS_BAR_Y, STATUS_BAR_W, STATUS_BAR_H, 0x25368E, 0xFFFFFF, STATUS_BORDER_W);
  rect(screen, MAIL_PANEL_X - 1, 0, STATUS_BORDER_W, STATUS_BAR_H, 0xFFFFFF);
  rect(screen, PAGE_PANEL_X - 1, 0, STATUS_BORDER_W, STATUS_BAR_H, 0xFFFFFF);
  rect(screen, TIME_PANEL_X - 1, 0, STATUS_BORDER_W, STATUS_BAR_H, 0xFFFFFF);
  rect(screen, BATTERY_PANEL_X - 1, 0, STATUS_BORDER_W, STATUS_BAR_H, 0xFFFFFF);

  image(screen, &ui_home_capybara, CAPYBARA_X, CAPYBARA_Y);
  label(screen, "test", &monogram_32, USERNAME_X, USERNAME_Y, 80, 26);

  image(screen, &ui_home_mail, MAIL_X, MAIL_Y);
  image(screen, &ui_home_battery, BATTERY_X, BATTERY_Y);

  lv_obj_t *badge = outlined_rect(screen, BADGE_X, BADGE_Y, BADGE_SIZE, BADGE_SIZE, 0xEAF06A, 0x111111, 2);
  lv_obj_set_style_radius(badge, BADGE_SIZE / 2, 0);
  lv_obj_t *badge_label = lv_label_create(badge);
  clear_obj(badge_label);
  lv_label_set_text(badge_label, "1");
  lv_obj_set_style_text_font(badge_label, &monogram_16, 0);
  lv_obj_set_style_text_color(badge_label, color(0x111111), 0);
  lv_obj_set_style_text_letter_space(badge_label, 0, 0);
  lv_obj_center(badge_label);

  launcher.page_counter = label(screen, "1/13", &monogram_28, PAGE_X, PAGE_Y, 57, 24);
  label(screen, "7:33pm", &monogram_28, TIME_X, TIME_Y, 76, 24);
}

static void draw_launcher(lv_obj_t *screen) {
  launcher.viewport = lv_obj_create(screen);
  clear_obj(launcher.viewport);
  lv_obj_set_pos(launcher.viewport, CARD_VIEWPORT_X, CARD_VIEWPORT_REST_Y);
  lv_obj_set_size(launcher.viewport, CARD_WIDTH, CARD_HEIGHT);
  lv_obj_clear_flag(launcher.viewport, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  lv_obj_add_flag(launcher.viewport, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(launcher.viewport, launcher_card_clicked, LV_EVENT_CLICKED, NULL);

  launcher.strip = lv_obj_create(launcher.viewport);
  clear_obj(launcher.strip);
  lv_obj_set_pos(launcher.strip, 0, 0);
  lv_obj_set_size(launcher.strip, CARD_STRIP_WIDTH, CARD_HEIGHT);

  launcher.left_slot = image(launcher.strip, launcher_cards[launcher.selected_index].image, 0, 0);
  launcher.right_slot = image(launcher.strip, launcher_cards[launcher.selected_index].image, CARD_WIDTH, 0);
  launcher.card_label = label(screen, launcher_cards[launcher.selected_index].label,
                              &monogram_32, CARD_LABEL_X, CARD_LABEL_Y, CARD_LABEL_W, CARD_LABEL_H);
  start_card_idle_animation();

  launcher.left_arrow = scaled_image(screen, &ui_home_left_arrow, LEFT_ARROW_X, LEFT_ARROW_Y, ARROW_SCALE);
  lv_obj_add_flag(launcher.left_arrow, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(launcher.left_arrow, launcher_arrow_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)-1);

  launcher.right_arrow = scaled_image(screen, &ui_home_right_arrow, RIGHT_ARROW_X, RIGHT_ARROW_Y, ARROW_SCALE);
  lv_obj_add_flag(launcher.right_arrow, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(launcher.right_arrow, launcher_arrow_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)1);
}

lv_obj_t *home_screen_create(lv_obj_t *parent) {
  lv_obj_t *screen = parent != NULL ? parent : lv_obj_create(NULL);
  clear_obj(screen);
  lv_obj_clean(screen);
  launcher = (launcher_state_t){0};
  button_input_set_ui_state(UI_STATE_HOME);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);

  draw_background(screen);
  draw_status_bar(screen);
  draw_launcher(screen);
  return screen;
}
