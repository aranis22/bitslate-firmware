#pragma once

#include "app_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  APP_MENU_HOME,
  APP_MENU_PHYSICS,
  APP_MENU_CHEMISTRY,
  APP_MENU_MATH,
  APP_MENU_GEOGRAPHY,
} app_menu_t;

void app_open_menu(app_menu_t menu);
void app_launch(app_id_t id);
void app_exit_to_previous_menu(void);
void app_exit_to_home(void);

#ifdef __cplusplus
}
#endif
