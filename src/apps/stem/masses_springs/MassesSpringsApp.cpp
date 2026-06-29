#include "MassesSpringsApp.h"

#include <cstdio>

#include <lvgl.h>

#include "SpringMassModel.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SIM_X = 8;
constexpr int SIM_Y = 8;
constexpr int SIM_W = 304;
constexpr int SIM_H = 304;
constexpr int PANEL_X = 320;
constexpr int PANEL_Y = 8;
constexpr int PANEL_W = 152;
constexpr int PANEL_H = 304;
constexpr int ANCHOR_X = 160;
constexpr int ANCHOR_Y = 60;
constexpr int REST_LENGTH_PX = 74;
constexpr float VIEW_SCALE = 92.0f;
constexpr int SLIDER_W = 122;
constexpr int SPRING_SEGMENTS = 10;
constexpr float START_PULL_M = 0.34f;

SpringMassModel model;
lv_obj_t* playButton = nullptr;
lv_obj_t* playLabel = nullptr;
lv_obj_t* massBlock = nullptr;
lv_obj_t* massText = nullptr;
lv_obj_t* restLine = nullptr;
lv_obj_t* equilibriumLine = nullptr;
lv_obj_t* equilibriumText = nullptr;
lv_obj_t* statusLabel = nullptr;
lv_obj_t* displacementLabel = nullptr;
lv_obj_t* springForceLabel = nullptr;
lv_obj_t* netForceLabel = nullptr;
lv_obj_t* massLabel = nullptr;
lv_obj_t* kLabel = nullptr;
lv_obj_t* dampingLabel = nullptr;
lv_obj_t* gravityLabel = nullptr;
lv_obj_t* massSlider = nullptr;
lv_obj_t* kSlider = nullptr;
lv_obj_t* dampingSlider = nullptr;
lv_obj_t* gravitySlider = nullptr;
lv_timer_t* simTimer = nullptr;
uint32_t lastMs = 0;
lv_obj_t* springLines[SPRING_SEGMENTS + 2] = {};
lv_point_precise_t springPoints[SPRING_SEGMENTS + 2][2] = {};

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
}

float clampf(float value, float low, float high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void styleBox(lv_obj_t* obj, uint32_t bg, uint32_t border, int borderWidth, int radius = 0) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(bg), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, radius, 0);
  clearInteraction(obj);
}

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font = &lv_font_montserrat_14) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_color(label, color(0x152033), 0);
  lv_obj_set_style_text_font(label, font, 0);
  clearInteraction(label);
  return label;
}

int massY() {
  return ANCHOR_Y + REST_LENGTH_PX + static_cast<int>(model.getDisplacement() * VIEW_SCALE);
}

int equilibriumY() {
  const float y = ANCHOR_Y + REST_LENGTH_PX + model.getEquilibriumExtension() * VIEW_SCALE;
  return static_cast<int>(clampf(y, 54.0f, 260.0f));
}

void primeOscillation(bool paused) {
  const float worldY = 34.0f + 88.0f + (model.getEquilibriumExtension() + START_PULL_M) * 140.0f;
  model.startDrag(worldY);
  model.releaseDrag();
  model.setPlaying(!paused);
}

void setPlayActive(bool active) {
  if (playButton == nullptr || playLabel == nullptr) return;
  lv_obj_set_style_bg_color(playButton, color(active ? 0x2E6CE6 : 0xDDEBFF), 0);
  lv_obj_set_style_border_color(playButton, color(active ? 0x174AA9 : 0xA9B9D3), 0);
  lv_obj_set_style_text_color(playLabel, color(active ? 0xFFFFFF : 0x152033), 0);
}

void updateLine(lv_obj_t* line, lv_point_precise_t points[2], int x1, int y1, int x2, int y2) {
  points[0] = {static_cast<lv_coord_t>(x1), static_cast<lv_coord_t>(y1)};
  points[1] = {static_cast<lv_coord_t>(x2), static_cast<lv_coord_t>(y2)};
  lv_line_set_points_mutable(line, points, 2);
}

void updateSpring() {
  int yTop = ANCHOR_Y + 8;
  int yBottom = massY() - 22;
  if (yBottom < yTop + 28) {
    yBottom = yTop + 28;
  }

  int prevX = ANCHOR_X;
  int prevY = yTop;
  for (int i = 0; i < SPRING_SEGMENTS; ++i) {
    int nextX = ANCHOR_X + (i % 2 == 0 ? -14 : 14);
    int nextY = yTop + ((yBottom - yTop) * (i + 1)) / SPRING_SEGMENTS;
    updateLine(springLines[i], springPoints[i], prevX, prevY, nextX, nextY);
    prevX = nextX;
    prevY = nextY;
  }
  updateLine(springLines[SPRING_SEGMENTS], springPoints[SPRING_SEGMENTS], prevX, prevY, ANCHOR_X, yBottom);
}

void updateReadouts() {
  char buf[48];
  std::snprintf(buf, sizeof(buf), "Mass\n%.1f kg", model.getMassKg());
  lv_label_set_text(massLabel, buf);
  std::snprintf(buf, sizeof(buf), "k\n%.0f N/m", model.getSpringK());
  lv_label_set_text(kLabel, buf);
  std::snprintf(buf, sizeof(buf), "Damping\n%.1f", model.getDamping());
  lv_label_set_text(dampingLabel, buf);
  std::snprintf(buf, sizeof(buf), "Gravity\n%.1f m/s2", model.getGravity());
  lv_label_set_text(gravityLabel, buf);

  const bool settling = model.isPlaying() &&
                        (model.getVelocity() < 0.08f && model.getVelocity() > -0.08f) &&
                        (model.getDisplacement() - model.getEquilibriumExtension() < 0.02f) &&
                        (model.getDisplacement() - model.getEquilibriumExtension() > -0.02f);
  lv_label_set_text(statusLabel, settling ? "Status: Settling" : (model.isPlaying() ? "Status: Playing" : "Status: Paused"));
  std::snprintf(buf, sizeof(buf), "Displacement: %.2f m", model.getDisplacement());
  lv_label_set_text(displacementLabel, buf);
  std::snprintf(buf, sizeof(buf), "Spring Force: %.1f N", model.getSpringForce());
  lv_label_set_text(springForceLabel, buf);
  std::snprintf(buf, sizeof(buf), "Net Force: %.1f N", model.getNetForce());
  lv_label_set_text(netForceLabel, buf);
  std::snprintf(buf, sizeof(buf), "%.1f kg", model.getMassKg());
  lv_label_set_text(massText, buf);
  lv_obj_center(massText);
}

void render() {
  updateSpring();
  const int y = massY();
  lv_obj_set_pos(massBlock, ANCHOR_X - 30, y - 18);
  lv_obj_set_pos(restLine, 54, ANCHOR_Y + REST_LENGTH_PX);
  lv_obj_set_pos(equilibriumLine, 54, equilibriumY());
  lv_obj_set_pos(equilibriumText, 196, equilibriumY() + 8);
  setPlayActive(model.isPlaying());
  lv_label_set_text(playLabel, model.isPlaying() ? "Pause" : "Play");
  updateReadouts();
  lv_obj_move_foreground(massBlock);
}

void playEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  model.setPlaying(!model.isPlaying());
  lastMs = lv_tick_get();
  render();
}

void resetEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  model.reset();
  primeOscillation(true);
  lv_slider_set_value(massSlider, 10, LV_ANIM_OFF);
  lv_slider_set_value(kSlider, 80, LV_ANIM_OFF);
  lv_slider_set_value(dampingSlider, 15, LV_ANIM_OFF);
  lv_slider_set_value(gravitySlider, 98, LV_ANIM_OFF);
  lastMs = lv_tick_get();
  render();
}

void sliderEvent(lv_event_t* event) {
  lv_event_code_t code = lv_event_get_code(event);
  if (code != LV_EVENT_VALUE_CHANGED && code != LV_EVENT_RELEASED) return;

  lv_obj_t* slider = static_cast<lv_obj_t*>(lv_event_get_target(event));
  int value = lv_slider_get_value(slider);
  if (slider == massSlider) model.setMassKg(value / 10.0f);
  if (slider == kSlider) model.setSpringK(static_cast<float>(value));
  if (slider == dampingSlider) model.setDamping(value / 10.0f);
  if (slider == gravitySlider) model.setGravity(value / 10.0f);
  render();
}

void timerCallback(lv_timer_t*) {
  uint32_t now = lv_tick_get();
  float dt = (now - lastMs) / 1000.0f;
  lastMs = now;
  model.step(dt);
  render();
}

lv_obj_t* makeButton(lv_obj_t* parent, const char* text, int x, int y, int w, int h, lv_event_cb_t cb, lv_obj_t** outLabel = nullptr) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_set_pos(btn, x, y);
  lv_obj_set_size(btn, w, h);
  lv_obj_set_style_bg_color(btn, color(0xDDEBFF), 0);
  lv_obj_set_style_border_color(btn, color(0xA9B9D3), 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_radius(btn, 5, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, color(0x152033), 0);
  lv_obj_center(label);
  if (outLabel != nullptr) *outLabel = label;
  return btn;
}

lv_obj_t* makeSlider(lv_obj_t* parent, int x, int y, int min, int max, int value) {
  lv_obj_t* slider = lv_slider_create(parent);
  lv_obj_set_pos(slider, x, y);
  lv_obj_set_size(slider, SLIDER_W, 12);
  lv_slider_set_range(slider, min, max);
  lv_slider_set_value(slider, value, LV_ANIM_OFF);
  lv_obj_add_event_cb(slider, sliderEvent, LV_EVENT_VALUE_CHANGED, nullptr);
  lv_obj_add_event_cb(slider, sliderEvent, LV_EVENT_RELEASED, nullptr);
  return slider;
}

} // namespace

void MassesSpringsApp::create() {
  model.reset();

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xE8F0FC, 0xE8F0FC, 0);

  lv_obj_t* sim = lv_obj_create(screen);
  styleBox(sim, 0xF6F9FF, 0xB4C3DA, 1, 7);
  lv_obj_set_pos(sim, SIM_X, SIM_Y);
  lv_obj_set_size(sim, SIM_W, SIM_H);

  lv_obj_t* panel = lv_obj_create(screen);
  styleBox(panel, 0xDEE8F6, 0xACBCD2, 1, 7);
  lv_obj_set_pos(panel, PANEL_X, PANEL_Y);
  lv_obj_set_size(panel, PANEL_W, PANEL_H);

  makeLabel(screen, "Masses & Springs", 22, 16, &lv_font_montserrat_22);

  lv_obj_t* ceiling = lv_obj_create(screen);
  styleBox(ceiling, 0x253044, 0x253044, 0);
  lv_obj_set_pos(ceiling, ANCHOR_X - 54, ANCHOR_Y - 9);
  lv_obj_set_size(ceiling, 108, 4);

  lv_obj_t* hook = lv_obj_create(screen);
  styleBox(hook, 0x253044, 0x253044, 0, LV_RADIUS_CIRCLE);
  lv_obj_set_pos(hook, ANCHOR_X - 5, ANCHOR_Y - 2);
  lv_obj_set_size(hook, 10, 10);

  restLine = lv_obj_create(screen);
  styleBox(restLine, 0x7E8EA6, 0x7E8EA6, 0);
  lv_obj_set_size(restLine, 220, 1);
  makeLabel(screen, "rest", 60, ANCHOR_Y + REST_LENGTH_PX - 17);

  equilibriumLine = lv_obj_create(screen);
  styleBox(equilibriumLine, 0x2F9C63, 0x2F9C63, 0);
  lv_obj_set_size(equilibriumLine, 220, 2);
  equilibriumText = makeLabel(screen, "equilibrium", 196, ANCHOR_Y + REST_LENGTH_PX + 32);

  for (int i = 0; i < SPRING_SEGMENTS + 1; ++i) {
    springLines[i] = lv_line_create(screen);
    lv_obj_set_style_line_color(springLines[i], color(0x46505F), 0);
    lv_obj_set_style_line_width(springLines[i], 3, 0);
    lv_obj_set_style_line_rounded(springLines[i], true, 0);
  }

  massBlock = lv_obj_create(screen);
  styleBox(massBlock, 0xDC7B36, 0x824A23, 2, 7);
  lv_obj_set_size(massBlock, 60, 36);
  massText = lv_label_create(massBlock);
  lv_obj_set_style_text_color(massText, color(0xFFFFFF), 0);

  statusLabel = makeLabel(screen, "Status: Playing", 24, 232);
  displacementLabel = makeLabel(screen, "Displacement: 0.00 m", 24, 254);
  springForceLabel = makeLabel(screen, "Spring Force: 0.0 N", 24, 272);
  netForceLabel = makeLabel(screen, "Net Force: 0.0 N", 24, 290);

  playButton = makeButton(screen, "Pause", 334, 32, 58, 30, playEvent, &playLabel);
  makeButton(screen, "Reset", 402, 32, 58, 30, resetEvent);

  massLabel = makeLabel(screen, "Mass\n1.0 kg", 334, 86);
  massSlider = makeSlider(screen, 334, 120, 5, 50, 10);
  kLabel = makeLabel(screen, "k\n80 N/m", 334, 140);
  kSlider = makeSlider(screen, 334, 174, 20, 200, 80);
  dampingLabel = makeLabel(screen, "Damping\n1.5", 334, 194);
  dampingSlider = makeSlider(screen, 334, 228, 0, 80, 15);
  gravityLabel = makeLabel(screen, "Gravity\n9.8 m/s2", 334, 248);
  gravitySlider = makeSlider(screen, 334, 282, 0, 98, 98);

  lastMs = lv_tick_get();
  if (simTimer != nullptr) {
    lv_timer_del(simTimer);
  }
  simTimer = lv_timer_create(timerCallback, 33, nullptr);
  primeOscillation(true);
  render();
}
