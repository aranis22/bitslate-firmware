#include "app_manager.h"

#include <lvgl.h>

#include "ui/screens/button_input.h"
#include "ui/screens/chemistry_menu.h"
#include "ui/screens/geography_menu.h"
#include "ui/screens/home_screen.h"
#include "ui/screens/math_menu.h"
#include "ui/screens/physics_menu.h"

namespace {
app_menu_t active_menu = APP_MENU_HOME;
bool active_is_leaf = false;
const app_descriptor_t *active_app = nullptr;
bool transition_pending = false;

void create_menu(app_menu_t menu) {
  switch(menu) {
    case APP_MENU_PHYSICS: physics_menu_create(); break;
    case APP_MENU_CHEMISTRY: chemistry_menu_create(); break;
    case APP_MENU_MATH: math_menu_create(); break;
    case APP_MENU_GEOGRAPHY: geography_menu_create(); break;
    case APP_MENU_HOME: {
      lv_obj_t *home = home_screen_create(nullptr);
      lv_screen_load(home);
      break;
    }
  }
}

ui_state_t menu_state(app_menu_t menu) {
  switch(menu) {
    case APP_MENU_PHYSICS: return UI_STATE_PHYSICS_MENU;
    case APP_MENU_CHEMISTRY: return UI_STATE_CHEMISTRY_MENU;
    case APP_MENU_MATH: return UI_STATE_MATH_MENU;
    case APP_MENU_GEOGRAPHY: return UI_STATE_GEOGRAPHY_MENU;
    case APP_MENU_HOME: return UI_STATE_HOME;
  }
  return UI_STATE_HOME;
}

void finish_menu_transition(lv_obj_t *old_screen, app_menu_t menu) {
  create_menu(menu);
  if(old_screen != nullptr) lv_obj_delete(old_screen);
  active_menu = menu;
  active_is_leaf = false;
  active_app = nullptr;
  transition_pending = false;
  button_input_set_ui_state(menu_state(menu));
}

void open_menu_async(void *data) {
  const app_menu_t menu = static_cast<app_menu_t>(reinterpret_cast<intptr_t>(data));
  finish_menu_transition(lv_screen_active(), menu);
}

void launch_app_async(void *data) {
  const app_descriptor_t *app = static_cast<const app_descriptor_t *>(data);
  lv_obj_t *old_screen = lv_screen_active();
  app->start();
  if(old_screen != nullptr) lv_obj_delete(old_screen);
  active_is_leaf = true;
  active_app = app;
  transition_pending = false;
  button_input_set_ui_state(UI_STATE_APP);
}

void back_async(void *) {
  lv_obj_t *old_screen = lv_screen_active();
  if(active_is_leaf && active_app != nullptr && active_app->cleanup != nullptr) active_app->cleanup();
  finish_menu_transition(old_screen, active_is_leaf ? active_menu : APP_MENU_HOME);
}

void home_async(void *) {
  lv_obj_t *old_screen = lv_screen_active();
  if(active_is_leaf && active_app != nullptr && active_app->cleanup != nullptr) active_app->cleanup();
  finish_menu_transition(old_screen, APP_MENU_HOME);
}
}

extern "C" void app_open_menu(app_menu_t menu) {
  if(transition_pending) return;
  transition_pending = true;
  button_input_set_ui_state(UI_STATE_TRANSITIONING);
  lv_async_call(open_menu_async, reinterpret_cast<void *>(static_cast<intptr_t>(menu)));
}

extern "C" void app_launch(app_id_t id) {
  const app_descriptor_t *app = app_registry_find(id);
  if(app == nullptr || transition_pending) return;
  transition_pending = true;
  button_input_set_ui_state(UI_STATE_TRANSITIONING);
  lv_async_call(launch_app_async, const_cast<app_descriptor_t *>(app));
}

extern "C" void app_exit_to_previous_menu(void) {
  if(transition_pending || button_input_get_ui_state() == UI_STATE_HOME) return;
  transition_pending = true;
  button_input_set_ui_state(UI_STATE_TRANSITIONING);
  lv_async_call(back_async, nullptr);
}

extern "C" void app_exit_to_home(void) {
  if(transition_pending || button_input_get_ui_state() == UI_STATE_HOME) return;
  transition_pending = true;
  button_input_set_ui_state(UI_STATE_TRANSITIONING);
  lv_async_call(home_async, nullptr);
}
