#include "FlagsQuizApp.h"

#include <lvgl.h>

#include "../generated/FlagsQuizAssets.h"

LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);
LV_FONT_DECLARE(monogram_28);

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int FLAG_CARD_W = 320;
constexpr int FLAG_CARD_H = 196;
constexpr int FLAG_CARD_X = (SCREEN_W - FLAG_CARD_W) / 2;
constexpr int FLAG_CARD_Y = 8;
constexpr int FLAG_INSET_X = FLAG_CARD_X + 20;
constexpr int FLAG_INSET_Y = FLAG_CARD_Y + 8;
constexpr int FLAG_INSET_W = FLAG_CARD_W - 40;
constexpr int FLAG_INSET_H = 180;
constexpr int STATUS_Y = 204;
constexpr int BUTTON_W = 156;
constexpr int BUTTON_H = 42;
constexpr int BUTTON_GAP_X = 12;
constexpr int BUTTON_GAP_Y = 8;
constexpr int BUTTON_X0 = (SCREEN_W - (BUTTON_W * 2 + BUTTON_GAP_X)) / 2;
constexpr int BUTTON_X1 = BUTTON_X0 + BUTTON_W + BUTTON_GAP_X;
constexpr int BUTTON_Y0 = 224;
constexpr int BUTTON_Y1 = BUTTON_Y0 + BUTTON_H + BUTTON_GAP_Y;
constexpr uint32_t CORRECT_DELAY_MS = 4000;

lv_obj_t* statusLabel = nullptr;
lv_obj_t* flagImage = nullptr;
lv_obj_t* optionButtons[4] = {};
lv_obj_t* optionLabels[4] = {};
uint8_t optionAssetIndexes[4] = {};
uint8_t currentQuestion = 0;
bool waitingToAdvance = false;
lv_timer_t* advanceTimer = nullptr;

lv_color_t color(unsigned int hex) {
  return lv_color_hex(hex);
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void stylePanel(lv_obj_t* obj, unsigned int bg, unsigned int border, int borderWidth, int radius) {
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

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, const lv_font_t* font, unsigned int textColor) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, color(textColor), 0);
  lv_obj_set_style_text_letter_space(label, 0, 0);
  clearInteraction(label);
  return label;
}

void setStatus(const char* text, unsigned int textColor) {
  if (statusLabel == nullptr) {
    return;
  }
  lv_label_set_text(statusLabel, text);
  lv_obj_set_style_text_color(statusLabel, color(textColor), 0);
}

void positionFlagImage(const bitslate::geo::flags::FlagQuizAsset& asset) {
  if (flagImage == nullptr) {
    return;
  }
  lv_image_set_src(flagImage, asset.image);
  lv_obj_set_size(flagImage, asset.width, asset.height);
  lv_obj_set_pos(
      flagImage,
      FLAG_INSET_X + (FLAG_INSET_W - asset.width) / 2,
      FLAG_INSET_Y + (FLAG_INSET_H - asset.height) / 2
  );
}

void setButtonEnabled(bool enabled) {
  for (lv_obj_t* button : optionButtons) {
    if (button == nullptr) {
      continue;
    }
    if (enabled) {
      lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
      lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_STATE_DISABLED);
    } else {
      lv_obj_clear_flag(button, LV_OBJ_FLAG_CLICKABLE);
    }
  }
}

void loadQuestion() {
  const uint8_t count = bitslate::geo::flags::FLAGS_QUIZ_ASSET_COUNT;
  if (currentQuestion >= count) {
    setStatus("Done!", 0x6CFF83);
    waitingToAdvance = true;
    setButtonEnabled(false);
    return;
  }

  waitingToAdvance = false;
  setButtonEnabled(true);

  const uint8_t correctSlot = currentQuestion % 4;
  uint8_t distractorOffset = 1;
  for (uint8_t slot = 0; slot < 4; ++slot) {
    if (slot == correctSlot) {
      optionAssetIndexes[slot] = currentQuestion;
    } else {
      optionAssetIndexes[slot] = (currentQuestion + distractorOffset) % count;
      ++distractorOffset;
    }

    const auto& option = bitslate::geo::flags::FLAGS_QUIZ_ASSETS[optionAssetIndexes[slot]];
    lv_label_set_text(optionLabels[slot], option.name);
    lv_obj_center(optionLabels[slot]);
  }

  positionFlagImage(bitslate::geo::flags::FLAGS_QUIZ_ASSETS[currentQuestion]);
  setStatus("Choose the flag", 0xF3F6F8);
}

void advanceTimerEvent(lv_timer_t* timer) {
  lv_timer_delete(timer);
  advanceTimer = nullptr;
  ++currentQuestion;
  loadQuestion();
}

void optionEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED || waitingToAdvance) {
    return;
  }

  const uint8_t* slot = static_cast<const uint8_t*>(lv_event_get_user_data(event));
  if (slot == nullptr || *slot >= 4) {
    return;
  }

  if (optionAssetIndexes[*slot] == currentQuestion) {
    setStatus("Correct!", 0x6CFF83);
    waitingToAdvance = true;
    setButtonEnabled(false);
    advanceTimer = lv_timer_create(advanceTimerEvent, CORRECT_DELAY_MS, nullptr);
  } else {
    setStatus("Try Again", 0xF3F6F8);
  }
}

lv_obj_t* makeOption(lv_obj_t* parent, int slot, int x, int y, unsigned int bg) {
  static const uint8_t SLOT_IDS[] = {0, 1, 2, 3};

  lv_obj_t* button = lv_obj_create(parent);
  stylePanel(button, bg, 0x0B0E10, 3, 4);
  lv_obj_set_pos(button, x, y);
  lv_obj_set_size(button, BUTTON_W, BUTTON_H);
  lv_obj_set_style_pad_all(button, 0, 0);
  lv_obj_set_style_bg_color(button, color(bg), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(button, color(0x252A2E), LV_STATE_PRESSED);
  lv_obj_set_style_translate_y(button, 2, LV_STATE_PRESSED);
  lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(button, optionEvent, LV_EVENT_CLICKED, const_cast<uint8_t*>(&SLOT_IDS[slot]));

  lv_obj_t* label = makeLabel(button, "", &monogram_28, 0xFFFFFF);
  lv_obj_center(label);
  optionButtons[slot] = button;
  optionLabels[slot] = label;
  return button;
}

void createFlagCard(lv_obj_t* screen) {
  lv_obj_t* shadow = lv_obj_create(screen);
  stylePanel(shadow, 0x15191C, 0x15191C, 0, 4);
  lv_obj_set_pos(shadow, FLAG_CARD_X + 4, FLAG_CARD_Y + 4);
  lv_obj_set_size(shadow, FLAG_CARD_W, FLAG_CARD_H);

  lv_obj_t* frame = lv_obj_create(screen);
  stylePanel(frame, 0x5A6268, 0x0C0F12, 4, 4);
  lv_obj_set_pos(frame, FLAG_CARD_X, FLAG_CARD_Y);
  lv_obj_set_size(frame, FLAG_CARD_W, FLAG_CARD_H);
  lv_obj_set_style_pad_all(frame, 0, 0);

  lv_obj_t* inset = lv_obj_create(screen);
  stylePanel(inset, 0x22282C, 0xAAB1B5, 2, 2);
  lv_obj_set_pos(inset, FLAG_INSET_X - 5, FLAG_INSET_Y - 5);
  lv_obj_set_size(inset, FLAG_INSET_W + 10, FLAG_INSET_H + 10);

  flagImage = lv_image_create(screen);
  clearInteraction(flagImage);
}

}  // namespace

void FlagsQuizApp::create() {
  currentQuestion = 0;
  waitingToAdvance = false;

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  stylePanel(screen, 0x343A3F, 0x343A3F, 0, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);

  createFlagCard(screen);

  statusLabel = makeLabel(screen, "Choose the flag", &monogram_24, 0xF3F6F8);
  lv_obj_set_pos(statusLabel, 0, STATUS_Y);
  lv_obj_set_width(statusLabel, SCREEN_W);
  lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, 0);

  makeOption(screen, 0, BUTTON_X0, BUTTON_Y0, 0xB94E35);
  makeOption(screen, 1, BUTTON_X1, BUTTON_Y0, 0x4979B8);
  makeOption(screen, 2, BUTTON_X0, BUTTON_Y1, 0x72A87A);
  makeOption(screen, 3, BUTTON_X1, BUTTON_Y1, 0xE0B83F);

  loadQuestion();
}

void FlagsQuizApp::destroy() {
  if(advanceTimer != nullptr) {
    lv_timer_del(advanceTimer);
    advanceTimer = nullptr;
  }
}
