#include "UsStatesQuizApp.h"

#include <cstdio>

#include <esp_heap_caps.h>
#include <lvgl.h>

#include "../core/UsStatesQuizCore.h"
#include "../generated/UsStatesVectorData.h"

LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);
LV_FONT_DECLARE(monogram_28);

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int MAP_DRAW_W = 430;
constexpr int MAP_DRAW_H = 282;
constexpr int MAP_PIXELS = MAP_DRAW_W * MAP_DRAW_H;
constexpr int MAP_BYTES = MAP_PIXELS * 2;
constexpr int MAP_X = (SCREEN_W - MAP_DRAW_W) / 2;
constexpr int MAP_Y = 34;

bitslate::geo::UsStatesQuizCore quiz;
lv_obj_t* promptLabel = nullptr;
lv_obj_t* statusLabel = nullptr;
lv_obj_t* timerLabel = nullptr;
lv_obj_t* mapArea = nullptr;
lv_obj_t* mapImage = nullptr;

uint16_t* mapPixels = nullptr;
lv_image_dsc_t mapImageDsc = {};
lv_timer_t* quizTimer = nullptr;
const char* statusText = "";
uint32_t stateStartTick = 0;

lv_color_t color(unsigned int hex) {
  return lv_color_hex(hex);
}

uint16_t rgb565(unsigned int hex) {
  const uint8_t r = (hex >> 16) & 0xFF;
  const uint8_t g = (hex >> 8) & 0xFF;
  const uint8_t b = hex & 0xFF;
  return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void styleBox(lv_obj_t* obj, unsigned int bg, unsigned int border, int borderWidth, int radius) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(bg), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, radius, 0);
  lv_obj_set_style_shadow_width(obj, 0, 0);
  lv_obj_set_style_outline_width(obj, 0, 0);
  clearInteraction(obj);
}

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font, unsigned int textColor) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, color(textColor), 0);
  clearInteraction(label);
  return label;
}

uint16_t completedColor(uint8_t stateId) {
  static constexpr unsigned int COLORS[] = {
      0x73B86D,
      0xE89A47,
      0xB5A34E,
      0x5E80C9,
      0x77B45F,
  };
  return rgb565(COLORS[stateId % (sizeof(COLORS) / sizeof(COLORS[0]))]);
}

void setRenderPixel(int x, int y, uint16_t value) {
  if (mapPixels == nullptr) {
    return;
  }
  if (x < 0 || y < 0 || x >= MAP_DRAW_W || y >= MAP_DRAW_H) {
    return;
  }
  mapPixels[y * MAP_DRAW_W + x] = value;
}

void drawGridCell(uint16_t gridX, uint16_t gridY, uint16_t value) {
  const int x0 = (static_cast<int>(gridX) * MAP_DRAW_W) / bitslate::geo::US_GRID_W;
  const int x1 = ((static_cast<int>(gridX) + 1) * MAP_DRAW_W) / bitslate::geo::US_GRID_W;
  const int y0 = (static_cast<int>(gridY) * MAP_DRAW_H) / bitslate::geo::US_GRID_H;
  const int y1 = ((static_cast<int>(gridY) + 1) * MAP_DRAW_H) / bitslate::geo::US_GRID_H;

  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      setRenderPixel(x, y, value);
    }
  }
}

void renderMapBuffer() {
  if (mapPixels == nullptr) {
    return;
  }

  const uint16_t background = rgb565(0xF1F4F7);
  const uint16_t baseMap = rgb565(0x8A8F94);
  const uint16_t stateBorder = rgb565(0xF7F7F7);

  for (uint32_t i = 0; i < MAP_PIXELS; ++i) {
    mapPixels[i] = background;
  }

  for (int y = 0; y < MAP_DRAW_H; ++y) {
    const uint16_t gridY = static_cast<uint16_t>((y * bitslate::geo::US_GRID_H) / MAP_DRAW_H);
    for (int x = 0; x < MAP_DRAW_W; ++x) {
      const uint16_t gridX = static_cast<uint16_t>((x * bitslate::geo::US_GRID_W) / MAP_DRAW_W);
      const int stateId = bitslate::geo::stateAtCell(gridX, gridY);
      if (stateId < 0) {
        continue;
      }
      mapPixels[y * MAP_DRAW_W + x] =
          quiz.isCompleted(static_cast<uint8_t>(stateId)) ? completedColor(static_cast<uint8_t>(stateId)) : baseMap;
    }
  }

  for (uint16_t cellIndex = 0; cellIndex < bitslate::geo::US_BORDER_CELL_COUNT; ++cellIndex) {
    drawGridCell(
        bitslate::geo::US_BORDER_CELLS[cellIndex].x,
        bitslate::geo::US_BORDER_CELLS[cellIndex].y,
        stateBorder
    );
  }

  if (mapImage != nullptr) {
    lv_obj_invalidate(mapImage);
  }
}

void updateLabels() {
  char prompt[64];
  if (quiz.isDone()) {
    std::snprintf(prompt, sizeof(prompt), "Done");
  } else {
    std::snprintf(prompt, sizeof(prompt), "Tap %s", quiz.currentPrompt());
  }
  lv_label_set_text(promptLabel, prompt);

  lv_label_set_text(statusLabel, statusText);

  if (timerLabel != nullptr) {
    const uint32_t elapsedSeconds = (lv_tick_get() - stateStartTick) / 1000;
    char timerText[16];
    std::snprintf(timerText, sizeof(timerText), "%02lu:%02lu", elapsedSeconds / 60, elapsedSeconds % 60);
    lv_label_set_text(timerLabel, timerText);
  }
}

void refresh() {
  renderMapBuffer();
  updateLabels();
}

void mapTapEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }

  lv_indev_t* indev = lv_event_get_indev(event);
  if (indev == nullptr) {
    return;
  }

  lv_point_t point;
  lv_indev_get_point(indev, &point);
  const int localX = point.x - MAP_X;
  const int localY = point.y - MAP_Y;
  if (localX < 0 || localY < 0 || localX >= MAP_DRAW_W || localY >= MAP_DRAW_H) {
    statusText = "Outside map";
    refresh();
    return;
  }

  const uint16_t gridX = static_cast<uint16_t>((localX * bitslate::geo::US_GRID_W) / MAP_DRAW_W);
  const uint16_t gridY = static_cast<uint16_t>((localY * bitslate::geo::US_GRID_H) / MAP_DRAW_H);
  const bitslate::geo::QuizTapResponse response = quiz.handleCellTap(gridX, gridY);
  switch (response.result) {
    case bitslate::geo::QuizTapResult::OutsideMap:
      statusText = "Outside map";
      break;
    case bitslate::geo::QuizTapResult::Correct:
      statusText = "Correct";
      stateStartTick = lv_tick_get();
      break;
    case bitslate::geo::QuizTapResult::Wrong:
      statusText = "Try again";
      break;
    case bitslate::geo::QuizTapResult::Done:
      statusText = "Done";
      break;
  }
  refresh();
}

void timerEvent(lv_timer_t*) {
  updateLabels();
}

void createMap(lv_obj_t* screen) {
  mapArea = lv_obj_create(screen);
  styleBox(mapArea, 0xF1F4F7, 0xF1F4F7, 0, 0);
  lv_obj_set_pos(mapArea, MAP_X, MAP_Y);
  lv_obj_set_size(mapArea, MAP_DRAW_W, MAP_DRAW_H);
  lv_obj_set_style_pad_all(mapArea, 0, 0);
  lv_obj_add_flag(mapArea, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(mapArea, mapTapEvent, LV_EVENT_CLICKED, nullptr);

  if (mapPixels == nullptr) {
    mapPixels = static_cast<uint16_t*>(
        heap_caps_malloc(MAP_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
    );
  }
  if (mapPixels == nullptr) {
    mapPixels = static_cast<uint16_t*>(heap_caps_malloc(MAP_BYTES, MALLOC_CAP_8BIT));
  }
  if (mapPixels == nullptr) {
    lv_obj_t* failureLabel =
        makeLabel(mapArea, "Map buffer failed", 0, (MAP_DRAW_H - 20) / 2, &lv_font_montserrat_14, 0x991B1B);
    lv_obj_set_width(failureLabel, MAP_DRAW_W);
    lv_obj_set_style_text_align(failureLabel, LV_TEXT_ALIGN_CENTER, 0);
    return;
  }

  mapImageDsc.header.magic = LV_IMAGE_HEADER_MAGIC;
  mapImageDsc.header.cf = LV_COLOR_FORMAT_RGB565;
  mapImageDsc.header.flags = 0;
  mapImageDsc.header.w = MAP_DRAW_W;
  mapImageDsc.header.h = MAP_DRAW_H;
  mapImageDsc.header.stride = MAP_DRAW_W * 2;
  mapImageDsc.data_size = MAP_BYTES;
  mapImageDsc.data = reinterpret_cast<const uint8_t*>(mapPixels);

  mapImage = lv_image_create(mapArea);
  lv_image_set_src(mapImage, &mapImageDsc);
  lv_obj_set_pos(mapImage, 0, 0);
  lv_obj_set_size(mapImage, MAP_DRAW_W, MAP_DRAW_H);
  lv_obj_clear_flag(mapImage, LV_OBJ_FLAG_CLICKABLE);
  clearInteraction(mapImage);
}

}  // namespace

void UsStatesQuizApp::create() {
  quiz.reset();
  statusText = "";
  stateStartTick = lv_tick_get();

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xEEF2F7, 0xEEF2F7, 0, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);

  promptLabel = makeLabel(screen, "Tap Washington", 12, 4, &monogram_28, 0x172033);
  lv_obj_set_width(promptLabel, SCREEN_W - 132);
  lv_obj_set_style_text_align(promptLabel, LV_TEXT_ALIGN_CENTER, 0);

  timerLabel = makeLabel(screen, "00:00", SCREEN_W - 108, 5, &monogram_28, 0x172033);
  lv_obj_set_width(timerLabel, 96);
  lv_obj_set_style_text_align(timerLabel, LV_TEXT_ALIGN_RIGHT, 0);

  createMap(screen);

  statusLabel = makeLabel(screen, "", 12, 293, &monogram_24, 0x334155);
  lv_obj_set_width(statusLabel, SCREEN_W - 24);
  lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, 0);

  refresh();
  quizTimer = lv_timer_create(timerEvent, 250, nullptr);
}

void UsStatesQuizApp::destroy() {
  if(quizTimer != nullptr) {
    lv_timer_del(quizTimer);
    quizTimer = nullptr;
  }
  if(mapPixels != nullptr) {
    heap_caps_free(mapPixels);
    mapPixels = nullptr;
  }
  mapImageDsc.data = nullptr;
  mapImage = nullptr;
  mapArea = nullptr;
  promptLabel = nullptr;
  statusLabel = nullptr;
  timerLabel = nullptr;
}
