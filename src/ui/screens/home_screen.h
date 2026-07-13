#pragma once

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t *home_screen_create(lv_obj_t *parent);
void home_screen_navigate(int direction);

#ifdef __cplusplus
}
#endif
