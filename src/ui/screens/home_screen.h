#pragma once

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t *home_screen_create(lv_obj_t *parent);
void home_screen_navigate(int direction);
void home_navigate_left(void);
void home_navigate_right(void);

#ifdef __cplusplus
}
#endif
