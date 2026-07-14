#pragma once

#include <stdint.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UI_STATE_HOME,
  UI_STATE_CHESS,
  UI_STATE_WORLD_MAP,
  UI_STATE_PHYSICS_MENU,
  UI_STATE_CHEMISTRY_MENU,
  UI_STATE_MATH_MENU,
  UI_STATE_GEOGRAPHY_MENU,
  UI_STATE_APP,
  UI_STATE_TRANSITIONING,
} ui_state_t;

typedef void (*app_destroy_cb_t)(void);

void button_input_init(void);
void button_input_poll(uint32_t now_ms);
void button_input_set_ui_state(ui_state_t state);
ui_state_t button_input_get_ui_state(void);
void button_input_register_active_app(lv_obj_t *screen, ui_state_t state, app_destroy_cb_t destroy);
void app_request_exit_to_home(void);

#ifdef __cplusplus
}
#endif
