#include "CollisionLabApp.h"

#include <cstdio>

#include <lvgl.h>

#include "CollisionModel.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int TRACK_TOP = 32;
constexpr int TRACK_H = 158;
constexpr int CONTROL_TOP = 200;
constexpr int SLIDER_W = 126;

CollisionModel model;
bool playing = false;
lv_obj_t* ballA = nullptr;
lv_obj_t* ballB = nullptr;
lv_obj_t* ballALabel = nullptr;
lv_obj_t* ballBLabel = nullptr;
lv_obj_t* arrowA = nullptr;
lv_obj_t* arrowB = nullptr;
lv_obj_t* arrowAHeadTop = nullptr;
lv_obj_t* arrowAHeadBottom = nullptr;
lv_obj_t* arrowBHeadTop = nullptr;
lv_obj_t* arrowBHeadBottom = nullptr;
lv_obj_t* arrowALabel = nullptr;
lv_obj_t* arrowBLabel = nullptr;
lv_obj_t* playButton = nullptr;
lv_obj_t* playButtonLabel = nullptr;
lv_obj_t* statusLabel = nullptr;
lv_obj_t* massALabel = nullptr;
lv_obj_t* massBLabel = nullptr;
lv_obj_t* velocityALabel = nullptr;
lv_obj_t* velocityBLabel = nullptr;
lv_obj_t* momentumLabel = nullptr;
lv_obj_t* energyLabel = nullptr;
lv_obj_t* massASlider = nullptr;
lv_obj_t* massBSlider = nullptr;
lv_obj_t* velocityASlider = nullptr;
lv_obj_t* velocityBSlider = nullptr;
lv_timer_t* simTimer = nullptr;
uint32_t lastMs = 0;
lv_point_precise_t arrowAPoints[2] = {};
lv_point_precise_t arrowBPoints[2] = {};
lv_point_precise_t arrowAHeadTopPoints[2] = {};
lv_point_precise_t arrowAHeadBottomPoints[2] = {};
lv_point_precise_t arrowBHeadTopPoints[2] = {};
lv_point_precise_t arrowBHeadBottomPoints[2] = {};

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
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

void setStatus(const char* text) {
  if (statusLabel != nullptr) {
    lv_label_set_text(statusLabel, text);
  }
}

void setPlayButtonActive(bool active) {
  if (playButton == nullptr || playButtonLabel == nullptr) return;

  lv_obj_set_style_bg_color(playButton, color(active ? 0x2E6CE6 : 0xDDEBFF), 0);
  lv_obj_set_style_border_color(playButton, color(active ? 0x174AA9 : 0xA9B9D3), 0);
  lv_obj_set_style_border_width(playButton, active ? 2 : 1, 0);
  lv_obj_set_style_text_color(playButtonLabel, color(active ? 0xFFFFFF : 0x152033), 0);
}

void setCircle(lv_obj_t* obj, lv_obj_t* label, const CollisionBall& ball, uint32_t fill, const char* text) {
  int size = static_cast<int>(ball.radius * 2.0f);
  lv_obj_set_size(obj, size, size);
  lv_obj_set_pos(obj, static_cast<int>(ball.x - ball.radius), static_cast<int>(TRACK_TOP + ball.y - ball.radius));
  lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(obj, color(fill), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(0xFFFFFF), 0);
  lv_obj_set_style_border_width(obj, 2, 0);

  if (label != nullptr) {
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, color(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, ball.radius > 25.0f ? &lv_font_montserrat_28 : &lv_font_montserrat_22, 0);
    lv_obj_center(label);
  }
}

void updateArrow(
    lv_obj_t* body,
    lv_obj_t* headTop,
    lv_obj_t* headBottom,
    lv_obj_t* valueLabel,
    lv_point_precise_t bodyPoints[2],
    lv_point_precise_t headTopPoints[2],
    lv_point_precise_t headBottomPoints[2],
    const CollisionBall& ball) {
  int y = static_cast<int>(TRACK_TOP + ball.y - ball.radius - 18.0f);
  int x = static_cast<int>(ball.x);
  float len = ball.velocityX * 0.28f;
  if (len > 42.0f) len = 42.0f;
  if (len < -42.0f) len = -42.0f;
  if (len > -5.0f && len < 5.0f) len = 0.0f;

  int endX = x + static_cast<int>(len);
  bodyPoints[0] = {static_cast<lv_coord_t>(x), static_cast<lv_coord_t>(y)};
  bodyPoints[1] = {static_cast<lv_coord_t>(endX), static_cast<lv_coord_t>(y)};
  lv_line_set_points_mutable(body, bodyPoints, 2);

  const int dir = len < 0.0f ? -1 : 1;
  headTopPoints[0] = {static_cast<lv_coord_t>(endX), static_cast<lv_coord_t>(y)};
  headTopPoints[1] = {static_cast<lv_coord_t>(endX - dir * 9), static_cast<lv_coord_t>(y - 6)};
  headBottomPoints[0] = {static_cast<lv_coord_t>(endX), static_cast<lv_coord_t>(y)};
  headBottomPoints[1] = {static_cast<lv_coord_t>(endX - dir * 9), static_cast<lv_coord_t>(y + 6)};
  lv_line_set_points_mutable(headTop, headTopPoints, 2);
  lv_line_set_points_mutable(headBottom, headBottomPoints, 2);

  if (len == 0.0f) {
    lv_obj_add_flag(body, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(headTop, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(headBottom, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_clear_flag(body, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(headTop, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(headBottom, LV_OBJ_FLAG_HIDDEN);
  }

  char buf[24];
  std::snprintf(buf, sizeof(buf), "%.0f m/s", ball.velocityX);
  lv_label_set_text(valueLabel, buf);
  lv_obj_set_pos(valueLabel, endX - (len < 0.0f ? 5 : 44), y - 27);
}

void updateReadouts() {
  char buf[48];
  const CollisionBall& a = model.getBall(1);
  const CollisionBall& b = model.getBall(2);

  std::snprintf(buf, sizeof(buf), "Mass A\n%.1f kg", a.mass);
  lv_label_set_text(massALabel, buf);
  std::snprintf(buf, sizeof(buf), "Vel A\n%.0f m/s", a.velocityX);
  lv_label_set_text(velocityALabel, buf);
  std::snprintf(buf, sizeof(buf), "Mass B\n%.1f kg", b.mass);
  lv_label_set_text(massBLabel, buf);
  std::snprintf(buf, sizeof(buf), "Vel B\n%.0f m/s", b.velocityX);
  lv_label_set_text(velocityBLabel, buf);
  std::snprintf(buf, sizeof(buf), "Momentum: %.0f kg m/s", model.totalMomentum());
  lv_label_set_text(momentumLabel, buf);
  std::snprintf(buf, sizeof(buf), "Kinetic Energy: %.0f J", model.totalKineticEnergy());
  lv_label_set_text(energyLabel, buf);
}

void render() {
  const CollisionBall& a = model.getBall(1);
  const CollisionBall& b = model.getBall(2);
  setCircle(ballA, ballALabel, a, 0x4B47F4, "A");
  setCircle(ballB, ballBLabel, b, 0xA35B68, "B");
  updateArrow(arrowA, arrowAHeadTop, arrowAHeadBottom, arrowALabel, arrowAPoints, arrowAHeadTopPoints, arrowAHeadBottomPoints, a);
  updateArrow(arrowB, arrowBHeadTop, arrowBHeadBottom, arrowBLabel, arrowBPoints, arrowBHeadTopPoints, arrowBHeadBottomPoints, b);
  updateReadouts();
  lv_obj_move_foreground(ballA);
  lv_obj_move_foreground(ballB);
}

void updateStatusFromModel() {
  if (model.wasLastStepCollision()) {
    setStatus("Collision!");
  } else if (model.wasLastStepWallBounce()) {
    setStatus("Wall bounce");
  } else {
    setStatus(playing ? "Playing" : "Paused");
  }
}

void playEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  playing = !playing;
  lv_label_set_text(playButtonLabel, playing ? "Pause" : "Play");
  setPlayButtonActive(playing);
  lastMs = lv_tick_get();
  setStatus(playing ? "Playing" : "Paused");
}

void resetEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  playing = false;
  model.reset();
  lv_label_set_text(playButtonLabel, "Play");
  setPlayButtonActive(false);
  lv_slider_set_value(massASlider, 10, LV_ANIM_OFF);
  lv_slider_set_value(massBSlider, 20, LV_ANIM_OFF);
  lv_slider_set_value(velocityASlider, 70, LV_ANIM_OFF);
  lv_slider_set_value(velocityBSlider, -45, LV_ANIM_OFF);
  setStatus("Paused");
  render();
}

void sliderEvent(lv_event_t* event) {
  lv_event_code_t code = lv_event_get_code(event);
  if (code != LV_EVENT_VALUE_CHANGED && code != LV_EVENT_RELEASED) return;

  lv_obj_t* slider = static_cast<lv_obj_t*>(lv_event_get_target(event));
  int value = lv_slider_get_value(slider);
  if (slider == massASlider) model.setMass(1, value / 10.0f);
  if (slider == massBSlider) model.setMass(2, value / 10.0f);
  if (slider == velocityASlider) model.setVelocityX(1, static_cast<float>(value));
  if (slider == velocityBSlider) model.setVelocityX(2, static_cast<float>(value));
  playing = false;
  lv_label_set_text(playButtonLabel, "Play");
  setPlayButtonActive(false);
  setStatus("Paused");
  render();
}

void timerCallback(lv_timer_t*) {
  uint32_t now = lv_tick_get();
  float dt = (now - lastMs) / 1000.0f;
  lastMs = now;

  if (playing) {
    model.step(dt);
    updateStatusFromModel();
    render();
  }
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
  if (outLabel != nullptr) {
    *outLabel = label;
  }
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

void CollisionLabApp::create() {
  model.reset();
  playing = false;

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xE8F0FC, 0xE8F0FC, 0);

  makeLabel(screen, "Collision Lab", 12, 7, &lv_font_montserrat_22);

  lv_obj_t* track = lv_obj_create(screen);
  styleBox(track, 0xD6E2F7, 0xA4B5D0, 1, 4);
  lv_obj_set_pos(track, 8, TRACK_TOP);
  lv_obj_set_size(track, SCREEN_W - 16, TRACK_H);

  lv_obj_t* leftWall = lv_obj_create(track);
  styleBox(leftWall, 0x28303C, 0x28303C, 0);
  lv_obj_set_pos(leftWall, 20, 20);
  lv_obj_set_size(leftWall, 4, TRACK_H - 40);

  lv_obj_t* rightWall = lv_obj_create(track);
  styleBox(rightWall, 0x28303C, 0x28303C, 0);
  lv_obj_set_pos(rightWall, SCREEN_W - 36, 20);
  lv_obj_set_size(rightWall, 4, TRACK_H - 40);

  lv_obj_t* line = lv_obj_create(track);
  styleBox(line, 0x7D90AA, 0x7D90AA, 0);
  lv_obj_set_pos(line, 24, static_cast<int>(model.getBall(1).y));
  lv_obj_set_size(line, SCREEN_W - 56, 2);

  ballA = lv_obj_create(screen);
  styleBox(ballA, 0x4B87F5, 0xFFFFFF, 2, LV_RADIUS_CIRCLE);
  ballALabel = lv_label_create(ballA);
  clearInteraction(ballALabel);
  ballB = lv_obj_create(screen);
  styleBox(ballB, 0xEC6D4B, 0xFFFFFF, 2, LV_RADIUS_CIRCLE);
  ballBLabel = lv_label_create(ballB);
  clearInteraction(ballBLabel);

  arrowA = lv_line_create(screen);
  lv_obj_set_style_line_color(arrowA, color(0xE83B2D), 0);
  lv_obj_set_style_line_width(arrowA, 3, 0);
  lv_obj_set_style_line_rounded(arrowA, true, 0);
  arrowAHeadTop = lv_line_create(screen);
  lv_obj_set_style_line_color(arrowAHeadTop, color(0xE83B2D), 0);
  lv_obj_set_style_line_width(arrowAHeadTop, 3, 0);
  lv_obj_set_style_line_rounded(arrowAHeadTop, true, 0);
  arrowAHeadBottom = lv_line_create(screen);
  lv_obj_set_style_line_color(arrowAHeadBottom, color(0xE83B2D), 0);
  lv_obj_set_style_line_width(arrowAHeadBottom, 3, 0);
  lv_obj_set_style_line_rounded(arrowAHeadBottom, true, 0);
  arrowALabel = makeLabel(screen, "0 m/s", 0, 0, &lv_font_montserrat_18);
  lv_obj_set_style_text_color(arrowALabel, color(0xE83B2D), 0);
  arrowB = lv_line_create(screen);
  lv_obj_set_style_line_color(arrowB, color(0xE83B2D), 0);
  lv_obj_set_style_line_width(arrowB, 3, 0);
  lv_obj_set_style_line_rounded(arrowB, true, 0);
  arrowBHeadTop = lv_line_create(screen);
  lv_obj_set_style_line_color(arrowBHeadTop, color(0xE83B2D), 0);
  lv_obj_set_style_line_width(arrowBHeadTop, 3, 0);
  lv_obj_set_style_line_rounded(arrowBHeadTop, true, 0);
  arrowBHeadBottom = lv_line_create(screen);
  lv_obj_set_style_line_color(arrowBHeadBottom, color(0xE83B2D), 0);
  lv_obj_set_style_line_width(arrowBHeadBottom, 3, 0);
  lv_obj_set_style_line_rounded(arrowBHeadBottom, true, 0);
  arrowBLabel = makeLabel(screen, "0 m/s", 0, 0, &lv_font_montserrat_18);
  lv_obj_set_style_text_color(arrowBLabel, color(0xE83B2D), 0);

  playButton = makeButton(screen, "Play", 10, 205, 64, 28, playEvent, &playButtonLabel);
  makeButton(screen, "Reset", 82, 205, 64, 28, resetEvent);

  statusLabel = makeLabel(screen, "Paused", 156, 211, &lv_font_montserrat_14);
  momentumLabel = makeLabel(screen, "Momentum: 0 kg m/s", 270, 204, &lv_font_montserrat_14);
  energyLabel = makeLabel(screen, "Kinetic Energy: 0 J", 270, 222, &lv_font_montserrat_14);

  massALabel = makeLabel(screen, "Mass A\n1.0 kg", 10, 238);
  lv_obj_set_width(massALabel, 74);
  massASlider = makeSlider(screen, 94, 252, 10, 50, 10);
  velocityALabel = makeLabel(screen, "Vel A\n70 m/s", 10, 280);
  lv_obj_set_width(velocityALabel, 74);
  velocityASlider = makeSlider(screen, 94, 294, -120, 120, 70);

  massBLabel = makeLabel(screen, "Mass B\n2.0 kg", 244, 238);
  lv_obj_set_width(massBLabel, 74);
  massBSlider = makeSlider(screen, 330, 252, 10, 50, 20);
  velocityBLabel = makeLabel(screen, "Vel B\n-45 m/s", 244, 280);
  lv_obj_set_width(velocityBLabel, 74);
  velocityBSlider = makeSlider(screen, 330, 294, -120, 120, -45);

  lastMs = lv_tick_get();
  if (simTimer != nullptr) {
    lv_timer_del(simTimer);
  }
  simTimer = lv_timer_create(timerCallback, 33, nullptr);
  setPlayButtonActive(false);
  render();
}

void CollisionLabApp::destroy() {
  if(simTimer != nullptr) {
    lv_timer_del(simTimer);
    simTimer = nullptr;
  }
}
