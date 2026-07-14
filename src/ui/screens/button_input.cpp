#include "button_input.h"

#include <Arduino.h>

#include "bitslate_config.h"
#include "home_screen.h"
#include "ui/navigation/app_manager.h"

namespace {

constexpr uint32_t DEBOUNCE_MS = 30;

struct Button {
  uint8_t pin;
  bool raw_pressed;
  bool stable_pressed;
  uint32_t changed_at;

  void begin(uint32_t now) {
    pinMode(pin, INPUT_PULLUP);
    raw_pressed = digitalRead(pin) == LOW;
    stable_pressed = raw_pressed;
    changed_at = now;
  }

  void reset(uint32_t now) {
    raw_pressed = digitalRead(pin) == LOW;
    stable_pressed = raw_pressed;
    changed_at = now;
  }

  bool pressed_once(uint32_t now) {
    const bool pressed = digitalRead(pin) == LOW;
    if (pressed != raw_pressed) {
      raw_pressed = pressed;
      changed_at = now;
    }
    if (stable_pressed != raw_pressed && now - changed_at >= DEBOUNCE_MS) {
      stable_pressed = raw_pressed;
      return stable_pressed;
    }
    return false;
  }
};

Button right_button{PIN_BUTTON_RIGHT, false, false, 0};
Button left_button{PIN_BUTTON_LEFT, false, false, 0};
Button back_button{PIN_BUTTON_BACK, false, false, 0};
ui_state_t ui_state = UI_STATE_HOME;
lv_obj_t *active_app_screen = nullptr;
app_destroy_cb_t active_app_destroy = nullptr;
bool exit_scheduled = false;

void exit_to_home_async(void *) {
  lv_obj_t *app_screen = active_app_screen;
  app_destroy_cb_t destroy = active_app_destroy;
  active_app_screen = nullptr;
  active_app_destroy = nullptr;

  lv_obj_t *home = home_screen_create(nullptr);
  lv_screen_load(home);
  if (app_screen != nullptr) lv_obj_delete(app_screen);
  if (destroy != nullptr) destroy();
  exit_scheduled = false;
}

}  // namespace

void button_input_init(void) {
  const uint32_t now = millis();
  right_button.begin(now);
  left_button.begin(now);
  back_button.begin(now);
}

void button_input_poll(uint32_t now_ms) {
  const bool right_pressed = right_button.pressed_once(now_ms);
  const bool left_pressed = left_button.pressed_once(now_ms);
  const bool back_pressed = back_button.pressed_once(now_ms);

  if (back_pressed) app_exit_to_previous_menu();
  if (ui_state != UI_STATE_HOME) return;
  if (right_pressed) home_navigate_right();
  if (left_pressed) home_navigate_left();
}

void button_input_set_ui_state(ui_state_t state) {
  ui_state = state;
  const uint32_t now = millis();
  right_button.reset(now);
  left_button.reset(now);
  back_button.reset(now);
}

ui_state_t button_input_get_ui_state(void) {
  return ui_state;
}

void button_input_register_active_app(lv_obj_t *screen, ui_state_t state, app_destroy_cb_t destroy) {
  active_app_screen = screen;
  active_app_destroy = destroy;
  button_input_set_ui_state(state);
}

void app_request_exit_to_home(void) {
  app_exit_to_home();
}
