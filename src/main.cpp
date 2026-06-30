#include <Arduino.h>
#include <lvgl.h>
#include "apps/games/chess/lvgl/ChessRenderSmokeApp.h"
#include "hal/display/LGFX_BitSlate.hpp"

static LGFX_BitSlate display;

static lv_display_t* lvDisplay = nullptr;

static constexpr uint32_t SCREEN_W = 480;
static constexpr uint32_t SCREEN_H = 320;
static constexpr uint32_t DRAW_BUF_LINES = 40;
static constexpr uint32_t DRAW_BUF_PIXELS = SCREEN_W * DRAW_BUF_LINES;

static lv_color_t drawBuf1[DRAW_BUF_PIXELS];
static lv_color_t drawBuf2[DRAW_BUF_PIXELS];

static void lvglFlush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  display.startWrite();
  display.setAddrWindow(area->x1, area->y1, w, h);
  display.writePixels((lgfx::rgb565_t*)px_map, w * h);
  display.endWrite();

  lv_display_flush_ready(disp);
}

static void lvglTouchRead(lv_indev_t* indev, lv_indev_data_t* data) {
  uint16_t x = 0;
  uint16_t y = 0;

  bool touched = display.getTouch(&x, &y);

  if (touched) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("BitSlate Chess LVGL smoke test");
  Serial.println("before display.init");

  display.init();
  display.setRotation(BITSLATE_DISPLAY_ROTATION);

  Serial.println("after display.init");

  lv_init();
  Serial.println("after lv_init");

  lvDisplay = lv_display_create(SCREEN_W, SCREEN_H);
  lv_display_set_flush_cb(lvDisplay, lvglFlush);
  lv_display_set_buffers(
      lvDisplay,
      drawBuf1,
      drawBuf2,
      sizeof(drawBuf1),
      LV_DISPLAY_RENDER_MODE_PARTIAL
  );

  lv_indev_t* touch = lv_indev_create();
  lv_indev_set_type(touch, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(touch, lvglTouchRead);

  Serial.println("before ChessRenderSmokeApp::create");
  ChessRenderSmokeApp::create();
  Serial.println("after ChessRenderSmokeApp::create");

  Serial.println("Chess smoke screen created");
}

void loop() {
  static uint32_t lastTick = millis();
  static uint32_t lastAlive = 0;

  uint32_t now = millis();
  lv_tick_inc(now - lastTick);
  lastTick = now;

  lv_timer_handler();
  if (now - lastAlive >= 2000) {
    Serial.println("alive");
    lastAlive = now;
  }
  delay(5);
}
