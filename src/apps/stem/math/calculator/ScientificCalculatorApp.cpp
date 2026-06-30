#include "ScientificCalculatorApp.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <lvgl.h>

LV_FONT_DECLARE(monogram_16);
LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);
LV_FONT_DECLARE(monogram_28);
LV_FONT_DECLARE(monogram_32);

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int OUTER_X = 6;
constexpr int OUTER_Y = 6;
constexpr int OUTER_W = 468;
constexpr int OUTER_H = 308;
constexpr int TITLE_H = 32;
constexpr int DISPLAY_X = 26;
constexpr int DISPLAY_Y = 48;
constexpr int DISPLAY_W = 428;
constexpr int DISPLAY_H = 62;
constexpr int GRID_X = 26;
constexpr int GRID_Y = 126;
constexpr int BTN_W = 64;
constexpr int BTN_H = 40;
constexpr int BTN_GAP_X = 10;
constexpr int BTN_GAP_Y = 8;
constexpr int TOGGLE_W = 42;
constexpr int TOGGLE_H = 20;
constexpr int TOGGLE_GAP = 4;

struct ButtonDef {
  const char* text;
  uint32_t color;
  uint32_t colorPressed;
  bool isClear;
  bool isEquals;
};

struct Parser {
  const char* text;
  size_t pos;
  double ansValue;
  bool degreeMode;
  bool error;
  bool divideByZero;
};

lv_obj_t* displayLabel = nullptr;
lv_obj_t* degButton = nullptr;
lv_obj_t* radButton = nullptr;
char displayBuffer[96] = "0";
double lastAnswer = 0.0;
bool hasAnswer = false;
bool lastInputWasEquals = false;
bool degreeMode = true;
bool stylesReady = false;
lv_style_t styleRoot;
lv_style_t styleWindow;
lv_style_t styleTitleBar;
lv_style_t styleDisplayFrame;
lv_style_t styleDisplayInner;
lv_style_t styleTitleText;
lv_style_t styleDisplayText;
lv_style_t styleButtonBase;
lv_style_t styleButtonText;
lv_style_t styleWindowButton;
lv_style_t styleWindowButtonPressed;
lv_style_t styleWindowButtonLabel;
lv_style_t styleNumberButton;
lv_style_t styleNumberButtonPressed;
lv_style_t styleFunctionButton;
lv_style_t styleFunctionButtonPressed;
lv_style_t styleDangerButton;
lv_style_t styleDangerButtonPressed;
lv_style_t styleToggleButton;
lv_style_t styleToggleButtonPressed;
lv_style_t styleToggleButtonActive;
lv_style_t styleToggleButtonLabel;

constexpr ButtonDef BUTTONS[5][6] = {
    {
        {"7", 0xF3C617, 0xD7A90B, false, false},
        {"8", 0xF3C617, 0xD7A90B, false, false},
        {"9", 0xF3C617, 0xD7A90B, false, false},
        {"+", 0x33D1A3, 0x21AA84, false, false},
        {"SQR", 0x4AE0B6, 0x2ABA92, false, false},
        {"SIN", 0x4AE0B6, 0x2ABA92, false, false},
    },
    {
        {"4", 0xF3C617, 0xD7A90B, false, false},
        {"5", 0xF3C617, 0xD7A90B, false, false},
        {"6", 0xF3C617, 0xD7A90B, false, false},
        {"-", 0x33D1A3, 0x21AA84, false, false},
        {"(", 0x4AE0B6, 0x2ABA92, false, false},
        {"COS", 0x4AE0B6, 0x2ABA92, false, false},
    },
    {
        {"1", 0xF3C617, 0xD7A90B, false, false},
        {"2", 0xF3C617, 0xD7A90B, false, false},
        {"3", 0xF3C617, 0xD7A90B, false, false},
        {"x", 0x33D1A3, 0x21AA84, false, false},
        {")", 0x4AE0B6, 0x2ABA92, false, false},
        {"TAN", 0x4AE0B6, 0x2ABA92, false, false},
    },
    {
        {"0", 0xF3C617, 0xD7A90B, false, false},
        {".", 0xF3C617, 0xD7A90B, false, false},
        {"=", 0xE55637, 0xBC3F24, false, true},
        {"/", 0x33D1A3, 0x21AA84, false, false},
        {"PI", 0x4AE0B6, 0x2ABA92, false, false},
        {"LOG", 0x4AE0B6, 0x2ABA92, false, false},
    },
    {
        {"%", 0x4AE0B6, 0x2ABA92, false, false},
        {"C", 0xE55637, 0xBC3F24, true, false},
        {"CE", 0xE55637, 0xBC3F24, true, false},
        {"+/-", 0x4AE0B6, 0x2ABA92, false, false},
        {"ANS", 0x4AE0B6, 0x2ABA92, false, false},
        {"LN", 0x4AE0B6, 0x2ABA92, false, false},
    },
};

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void initStyleBase(lv_style_t& style) {
  lv_style_init(&style);
  lv_style_set_border_color(&style, color(0x111111));
  lv_style_set_border_width(&style, 2);
  lv_style_set_outline_width(&style, 1);
  lv_style_set_outline_pad(&style, 0);
  lv_style_set_outline_color(&style, color(0x94A0AE));
  lv_style_set_radius(&style, 0);
  lv_style_set_shadow_width(&style, 0);
  lv_style_set_pad_all(&style, 0);
}

void ensureStyles() {
  if (stylesReady) return;

  lv_style_init(&styleRoot);
  lv_style_set_bg_color(&styleRoot, color(0x4A525C));
  lv_style_set_bg_opa(&styleRoot, LV_OPA_COVER);
  lv_style_set_border_width(&styleRoot, 0);
  lv_style_set_pad_all(&styleRoot, 0);

  initStyleBase(styleWindow);
  lv_style_set_bg_color(&styleWindow, color(0x626B76));
  lv_style_set_bg_opa(&styleWindow, LV_OPA_COVER);
  lv_style_set_border_color(&styleWindow, color(0x262B31));
  lv_style_set_outline_color(&styleWindow, color(0x89939D));

  initStyleBase(styleTitleBar);
  lv_style_set_bg_color(&styleTitleBar, color(0x46505A));
  lv_style_set_bg_opa(&styleTitleBar, LV_OPA_COVER);
  lv_style_set_border_color(&styleTitleBar, color(0x1D2228));
  lv_style_set_outline_color(&styleTitleBar, color(0x73808C));

  initStyleBase(styleDisplayFrame);
  lv_style_set_bg_color(&styleDisplayFrame, color(0x59616B));
  lv_style_set_bg_opa(&styleDisplayFrame, LV_OPA_COVER);
  lv_style_set_border_color(&styleDisplayFrame, color(0x20252A));
  lv_style_set_outline_color(&styleDisplayFrame, color(0x89939D));

  initStyleBase(styleDisplayInner);
  lv_style_set_bg_color(&styleDisplayInner, color(0x060908));
  lv_style_set_bg_opa(&styleDisplayInner, LV_OPA_COVER);
  lv_style_set_border_color(&styleDisplayInner, color(0x1B1F1C));
  lv_style_set_outline_color(&styleDisplayInner, color(0x293229));

  lv_style_init(&styleTitleText);
  lv_style_set_text_color(&styleTitleText, color(0xF2F3F5));
  lv_style_set_text_font(&styleTitleText, &monogram_28);

  lv_style_init(&styleDisplayText);
  lv_style_set_text_color(&styleDisplayText, color(0x1BEB22));
  lv_style_set_text_font(&styleDisplayText, &monogram_32);

  initStyleBase(styleButtonBase);
  lv_style_set_bg_opa(&styleButtonBase, LV_OPA_COVER);
  lv_style_set_text_color(&styleButtonBase, color(0x111111));
  lv_style_set_pad_bottom(&styleButtonBase, 4);
  lv_style_set_pad_top(&styleButtonBase, 0);
  lv_style_set_pad_left(&styleButtonBase, 0);
  lv_style_set_pad_right(&styleButtonBase, 0);

  lv_style_init(&styleButtonText);
  lv_style_set_text_font(&styleButtonText, &monogram_28);
  lv_style_set_text_color(&styleButtonText, color(0x101010));

  initStyleBase(styleWindowButton);
  lv_style_set_bg_color(&styleWindowButton, color(0x707A86));
  lv_style_set_bg_opa(&styleWindowButton, LV_OPA_COVER);
  lv_style_set_border_color(&styleWindowButton, color(0x171C21));
  lv_style_set_outline_color(&styleWindowButton, color(0xA2ABB4));

  initStyleBase(styleWindowButtonPressed);
  lv_style_set_bg_color(&styleWindowButtonPressed, color(0x535B65));
  lv_style_set_bg_opa(&styleWindowButtonPressed, LV_OPA_COVER);
  lv_style_set_border_color(&styleWindowButtonPressed, color(0x171C21));
  lv_style_set_outline_color(&styleWindowButtonPressed, color(0x848C96));

  lv_style_init(&styleWindowButtonLabel);
  lv_style_set_text_font(&styleWindowButtonLabel, &monogram_16);
  lv_style_set_text_color(&styleWindowButtonLabel, color(0xF2F3F5));

  initStyleBase(styleNumberButton);
  lv_style_set_bg_color(&styleNumberButton, color(0xF3C617));
  lv_style_set_bg_opa(&styleNumberButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleNumberButton, color(0xFFDE4C));

  initStyleBase(styleNumberButtonPressed);
  lv_style_set_bg_color(&styleNumberButtonPressed, color(0xD7A90B));
  lv_style_set_bg_opa(&styleNumberButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleNumberButtonPressed, color(0xF0CB50));

  initStyleBase(styleFunctionButton);
  lv_style_set_bg_color(&styleFunctionButton, color(0x41D7AA));
  lv_style_set_bg_opa(&styleFunctionButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleFunctionButton, color(0x6DE9C3));

  initStyleBase(styleFunctionButtonPressed);
  lv_style_set_bg_color(&styleFunctionButtonPressed, color(0x26B98C));
  lv_style_set_bg_opa(&styleFunctionButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleFunctionButtonPressed, color(0x56D6AF));

  initStyleBase(styleDangerButton);
  lv_style_set_bg_color(&styleDangerButton, color(0xE55637));
  lv_style_set_bg_opa(&styleDangerButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleDangerButton, color(0xFF785C));

  initStyleBase(styleDangerButtonPressed);
  lv_style_set_bg_color(&styleDangerButtonPressed, color(0xBC3F24));
  lv_style_set_bg_opa(&styleDangerButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleDangerButtonPressed, color(0xE06A52));

  initStyleBase(styleToggleButton);
  lv_style_set_bg_color(&styleToggleButton, color(0x505862));
  lv_style_set_bg_opa(&styleToggleButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleToggleButton, color(0xA3ACB5));
  lv_style_set_border_color(&styleToggleButton, color(0x171C21));

  initStyleBase(styleToggleButtonPressed);
  lv_style_set_bg_color(&styleToggleButtonPressed, color(0x444B54));
  lv_style_set_bg_opa(&styleToggleButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleToggleButtonPressed, color(0x8D97A0));
  lv_style_set_border_color(&styleToggleButtonPressed, color(0x171C21));

  initStyleBase(styleToggleButtonActive);
  lv_style_set_bg_color(&styleToggleButtonActive, color(0xE9D07A));
  lv_style_set_bg_opa(&styleToggleButtonActive, LV_OPA_COVER);
  lv_style_set_outline_color(&styleToggleButtonActive, color(0xFFF0A8));
  lv_style_set_border_color(&styleToggleButtonActive, color(0x171C21));

  lv_style_init(&styleToggleButtonLabel);
  lv_style_set_text_font(&styleToggleButtonLabel, &monogram_20);
  lv_style_set_text_color(&styleToggleButtonLabel, color(0xF2F3F5));

  stylesReady = true;
}

void setDisplayText(const char* text) {
  if (displayLabel != nullptr) {
    std::snprintf(displayBuffer, sizeof(displayBuffer), "%s", text);
    lv_label_set_text(displayLabel, text);
  }
}

bool isReplaceState(const char* current) {
  return current == nullptr ||
         std::strcmp(current, "0") == 0 ||
         std::strcmp(current, "ERR") == 0 ||
         std::strcmp(current, "DIV 0") == 0;
}

bool isDigitLabel(const char* text) {
  return std::strlen(text) == 1 && text[0] >= '0' && text[0] <= '9';
}

bool isOperatorLabel(const char* text) {
  return std::strcmp(text, "+") == 0 ||
         std::strcmp(text, "-") == 0 ||
         std::strcmp(text, "x") == 0 ||
         std::strcmp(text, "/") == 0;
}

double toRadians(double valueDegrees) {
  return valueDegrees * 3.14159265358979323846 / 180.0;
}

void skipSpaces(Parser& parser) {
  while (parser.text[parser.pos] == ' ') {
    ++parser.pos;
  }
}

double parseExpression(Parser& parser);

double parseNumber(Parser& parser) {
  skipSpaces(parser);
  const char* start = parser.text + parser.pos;
  char* end = nullptr;
  double value = std::strtod(start, &end);
  if (end == start) {
    parser.error = true;
    return 0.0;
  }
  parser.pos += static_cast<size_t>(end - start);
  return value;
}

bool matchToken(Parser& parser, const char* token) {
  skipSpaces(parser);
  size_t len = std::strlen(token);
  if (std::strncmp(parser.text + parser.pos, token, len) == 0) {
    parser.pos += len;
    return true;
  }
  return false;
}

bool peekAlphaToken(const Parser& parser, const char* token) {
  size_t len = std::strlen(token);
  return std::strncmp(parser.text + parser.pos, token, len) == 0;
}

double parseFactor(Parser& parser) {
  skipSpaces(parser);

  if (matchToken(parser, "-")) {
    return -parseFactor(parser);
  }

  if (matchToken(parser, "+")) {
    return parseFactor(parser);
  }

  if (matchToken(parser, "(")) {
    double value = parseExpression(parser);
    if (!matchToken(parser, ")")) {
      parser.error = true;
    }
    return value;
  }

  if (matchToken(parser, "PI")) {
    return 3.14159265;
  }

  if (matchToken(parser, "ANS")) {
    return parser.ansValue;
  }

  if (peekAlphaToken(parser, "SQR(") ||
      peekAlphaToken(parser, "SIN(") ||
      peekAlphaToken(parser, "COS(") ||
      peekAlphaToken(parser, "TAN(") ||
      peekAlphaToken(parser, "LOG(") ||
      peekAlphaToken(parser, "LN(")) {
    char fn[4] = {0, 0, 0, 0};
    std::memcpy(fn, parser.text + parser.pos, 3);
    parser.pos += 3;
    if (!matchToken(parser, "(")) {
      parser.error = true;
      return 0.0;
    }

    double value = parseExpression(parser);
    if (!matchToken(parser, ")")) {
      parser.error = true;
      return 0.0;
    }

    if (std::strcmp(fn, "SQR") == 0) {
      if (value < 0.0) {
        parser.error = true;
        return 0.0;
      }
      return std::sqrt(value);
    }
    if (std::strcmp(fn, "LOG") == 0) {
      if (value <= 0.0) {
        parser.error = true;
        return 0.0;
      }
      return std::log10(value);
    }
    if (std::strcmp(fn, "LN") == 0) {
      if (value <= 0.0) {
        parser.error = true;
        return 0.0;
      }
      return std::log(value);
    }

    double trigInput = parser.degreeMode ? toRadians(value) : value;
    if (std::strcmp(fn, "SIN") == 0) {
      return std::sin(trigInput);
    }
    if (std::strcmp(fn, "COS") == 0) {
      return std::cos(trigInput);
    }
    if (std::strcmp(fn, "TAN") == 0) {
      double cosine = std::cos(trigInput);
      if (std::fabs(cosine) < 1e-9) {
        parser.error = true;
        return 0.0;
      }
      return std::tan(trigInput);
    }

    parser.error = true;
    return 0.0;
  }

  return parseNumber(parser);
}

double parseTerm(Parser& parser) {
  double value = parseFactor(parser);
  while (!parser.error && !parser.divideByZero) {
    if (matchToken(parser, "x")) {
      value *= parseFactor(parser);
    } else if (matchToken(parser, "/")) {
      double divisor = parseFactor(parser);
      if (std::fabs(divisor) < 1e-12) {
        parser.divideByZero = true;
        return 0.0;
      }
      value /= divisor;
    } else if (matchToken(parser, "%")) {
      value /= 100.0;
    } else {
      break;
    }
  }
  return value;
}

double parseExpression(Parser& parser) {
  double value = parseTerm(parser);
  while (!parser.error && !parser.divideByZero) {
    if (matchToken(parser, "+")) {
      value += parseTerm(parser);
    } else if (matchToken(parser, "-")) {
      value -= parseTerm(parser);
    } else {
      break;
    }
  }
  return value;
}

bool evaluateExpression(const char* expression, double ansValue, double& outValue, bool& divideByZero) {
  Parser parser{expression, 0, ansValue, degreeMode, false, false};
  outValue = parseExpression(parser);
  skipSpaces(parser);
  divideByZero = parser.divideByZero;
  if (parser.divideByZero) return false;
  if (parser.error || parser.text[parser.pos] != '\0' || !std::isfinite(outValue)) return false;
  return true;
}

void formatResult(double value, char* out, size_t outSize) {
  char temp[64];
  std::snprintf(temp, sizeof(temp), "%.8f", value);

  char* dot = std::strchr(temp, '.');
  if (dot != nullptr) {
    char* end = temp + std::strlen(temp) - 1;
    while (end > dot && *end == '0') {
      *end-- = '\0';
    }
    if (end == dot) {
      *end = '\0';
    }
  }

  std::snprintf(out, outSize, "%s", temp);
}

bool isSimpleNumberText(const char* text) {
  if (text == nullptr || *text == '\0') return false;
  char* end = nullptr;
  std::strtod(text, &end);
  return end != text && *end == '\0';
}

void appendDisplayText(const char* suffix, bool replaceCurrent) {
  char buffer[96];
  if (replaceCurrent) {
    std::snprintf(buffer, sizeof(buffer), "%s", suffix);
  } else {
    std::snprintf(buffer, sizeof(buffer), "%s%s", displayBuffer, suffix);
  }
  setDisplayText(buffer);
}

void evaluateCurrentExpression() {
  double value = 0.0;
  bool divideByZero = false;
  if (!evaluateExpression(displayBuffer, hasAnswer ? lastAnswer : 0.0, value, divideByZero)) {
    setDisplayText(divideByZero ? "DIV 0" : "ERR");
    lastInputWasEquals = true;
    return;
  }

  char result[64];
  formatResult(value, result, sizeof(result));
  setDisplayText(result);
  lastAnswer = value;
  hasAnswer = true;
  lastInputWasEquals = true;
}

void handlePlusMinus() {
  if (isSimpleNumberText(displayBuffer)) {
    double value = std::strtod(displayBuffer, nullptr);
    char buffer[64];
    formatResult(-value, buffer, sizeof(buffer));
    setDisplayText(buffer);
    return;
  }

  if (std::strcmp(displayBuffer, "0") == 0) {
    setDisplayText("-");
    return;
  }

  char buffer[96];
  std::snprintf(buffer, sizeof(buffer), "-(%s)", displayBuffer);
  setDisplayText(buffer);
}

void updateAngleModeButtons() {
  if (degButton == nullptr || radButton == nullptr) return;

  if (degreeMode) {
    lv_obj_add_style(degButton, &styleToggleButtonActive, 0);
    lv_obj_remove_style(radButton, &styleToggleButtonActive, 0);
    lv_obj_set_style_text_color(degButton, color(0x101010), 0);
    lv_obj_set_style_text_color(radButton, color(0xF2F3F5), 0);
  } else {
    lv_obj_add_style(radButton, &styleToggleButtonActive, 0);
    lv_obj_remove_style(degButton, &styleToggleButtonActive, 0);
    lv_obj_set_style_text_color(radButton, color(0x101010), 0);
    lv_obj_set_style_text_color(degButton, color(0xF2F3F5), 0);
  }
}

void angleModeEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  const char* mode = static_cast<const char*>(lv_event_get_user_data(event));
  if (mode == nullptr) return;
  degreeMode = std::strcmp(mode, "DEG") == 0;
  updateAngleModeButtons();
}

lv_obj_t* createAngleButton(lv_obj_t* parent, const char* text, int x, int y) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_add_style(btn, &styleToggleButton, 0);
  lv_obj_add_style(btn, &styleToggleButtonPressed, LV_STATE_PRESSED);
  lv_obj_set_size(btn, TOGGLE_W, TOGGLE_H);
  lv_obj_set_pos(btn, x, y);
  lv_obj_add_event_cb(btn, angleModeEvent, LV_EVENT_CLICKED, const_cast<char*>(text));
  clearInteraction(btn);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_add_style(label, &styleToggleButtonLabel, 0);
  lv_obj_center(label);
  return btn;
}

void buttonEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  const ButtonDef* def = static_cast<const ButtonDef*>(lv_event_get_user_data(event));
  if (def == nullptr || displayLabel == nullptr) return;

  if (def->isClear) {
    setDisplayText("0");
    lastInputWasEquals = false;
    return;
  }

  if (def->isEquals) {
    evaluateCurrentExpression();
    return;
  }

  if (std::strcmp(def->text, "+/-") == 0) {
    handlePlusMinus();
    lastInputWasEquals = false;
    return;
  }

  bool replaceCurrent = isReplaceState(displayBuffer);
  if (lastInputWasEquals) {
    if (isDigitLabel(def->text) || std::strcmp(def->text, ".") == 0) {
      replaceCurrent = true;
    } else if (isOperatorLabel(def->text) || std::strcmp(def->text, ")") == 0) {
      replaceCurrent = false;
    } else {
      replaceCurrent = true;
    }
  }

  if (std::strcmp(def->text, "SQR") == 0 ||
      std::strcmp(def->text, "SIN") == 0 ||
      std::strcmp(def->text, "COS") == 0 ||
      std::strcmp(def->text, "TAN") == 0 ||
      std::strcmp(def->text, "LOG") == 0 ||
      std::strcmp(def->text, "LN") == 0) {
    char fnBuffer[8];
    std::snprintf(fnBuffer, sizeof(fnBuffer), "%s(", def->text);
    appendDisplayText(fnBuffer, replaceCurrent);
  } else if (std::strcmp(def->text, "PI") == 0 || std::strcmp(def->text, "ANS") == 0) {
    appendDisplayText(def->text, replaceCurrent);
  } else if (std::strcmp(def->text, "%") == 0) {
    appendDisplayText("%", replaceCurrent);
  } else {
    appendDisplayText(def->text, replaceCurrent);
  }

  lastInputWasEquals = false;
}

lv_obj_t* makeText(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font, uint32_t colorHex) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, color(colorHex), 0);
  clearInteraction(label);
  return label;
}

lv_obj_t* createWindowButton(lv_obj_t* parent, const char* text, int x, uint32_t bgColor) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_add_style(btn, &styleWindowButton, 0);
  lv_obj_add_style(btn, &styleWindowButtonPressed, LV_STATE_PRESSED);
  lv_obj_set_style_bg_color(btn, color(bgColor), 0);
  lv_obj_set_size(btn, 18, 18);
  lv_obj_set_pos(btn, x, 4);
  clearInteraction(btn);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_add_style(label, &styleWindowButtonLabel, 0);
  lv_obj_center(label);
  return btn;
}

void createButton(lv_obj_t* parent, const ButtonDef& def, int row, int col) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_add_style(btn, &styleButtonBase, 0);

  const bool isDanger = def.isClear || def.isEquals;
  const bool isNumber = isDigitLabel(def.text) || std::strcmp(def.text, ".") == 0;

  if (isDanger) {
    lv_obj_add_style(btn, &styleDangerButton, 0);
    lv_obj_add_style(btn, &styleDangerButtonPressed, LV_STATE_PRESSED);
  } else if (isNumber) {
    lv_obj_add_style(btn, &styleNumberButton, 0);
    lv_obj_add_style(btn, &styleNumberButtonPressed, LV_STATE_PRESSED);
  } else {
    lv_obj_add_style(btn, &styleFunctionButton, 0);
    lv_obj_add_style(btn, &styleFunctionButtonPressed, LV_STATE_PRESSED);
  }

  lv_obj_set_size(btn, BTN_W, BTN_H);
  lv_obj_set_pos(btn, GRID_X + col * (BTN_W + BTN_GAP_X), GRID_Y + row * (BTN_H + BTN_GAP_Y));
  lv_obj_add_event_cb(btn, buttonEvent, LV_EVENT_CLICKED, const_cast<ButtonDef*>(&def));
  clearInteraction(btn);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, def.text);
  lv_obj_add_style(label, &styleButtonText, 0);
  if (isNumber || std::strcmp(def.text, "=") == 0) {
    lv_obj_set_style_text_font(label, &monogram_28, 0);
  }
  lv_obj_center(label);
}

}  // namespace

void ScientificCalculatorApp::create() {
  ensureStyles();

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  lv_obj_remove_style_all(screen);
  lv_obj_add_style(screen, &styleRoot, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
  clearInteraction(screen);

  lv_obj_t* window = lv_obj_create(screen);
  lv_obj_remove_style_all(window);
  lv_obj_add_style(window, &styleWindow, 0);
  lv_obj_set_pos(window, OUTER_X, OUTER_Y);
  lv_obj_set_size(window, OUTER_W, OUTER_H);
  clearInteraction(window);

  lv_obj_t* titleBar = lv_obj_create(window);
  lv_obj_remove_style_all(titleBar);
  lv_obj_add_style(titleBar, &styleTitleBar, 0);
  lv_obj_set_pos(titleBar, 4, 4);
  lv_obj_set_size(titleBar, OUTER_W - 8, TITLE_H);
  clearInteraction(titleBar);

  lv_obj_t* iconBox = lv_obj_create(titleBar);
  lv_obj_remove_style_all(iconBox);
  lv_obj_add_style(iconBox, &styleWindowButton, 0);
  lv_obj_set_size(iconBox, 18, 18);
  lv_obj_set_pos(iconBox, 6, 4);
  clearInteraction(iconBox);
  makeText(iconBox, "[]", 2, 3, &monogram_16, 0xE5FFF1);

  makeText(titleBar, "Scientific Calculator", 30, 1, &monogram_28, 0xF0F2F5);
  createWindowButton(titleBar, "_", OUTER_W - 74, 0x66707B);
  createWindowButton(titleBar, "[]", OUTER_W - 50, 0x66707B);
  createWindowButton(titleBar, "X", OUTER_W - 26, 0xD85639);

  lv_obj_t* displayFrame = lv_obj_create(window);
  lv_obj_remove_style_all(displayFrame);
  lv_obj_add_style(displayFrame, &styleDisplayFrame, 0);
  lv_obj_set_pos(displayFrame, DISPLAY_X, DISPLAY_Y);
  lv_obj_set_size(displayFrame, DISPLAY_W, DISPLAY_H);
  clearInteraction(displayFrame);

  lv_obj_t* displayInner = lv_obj_create(displayFrame);
  lv_obj_remove_style_all(displayInner);
  lv_obj_add_style(displayInner, &styleDisplayInner, 0);
  lv_obj_set_pos(displayInner, 6, 6);
  lv_obj_set_size(displayInner, DISPLAY_W - 12, DISPLAY_H - 12);
  clearInteraction(displayInner);

  displayLabel = lv_label_create(displayInner);
  lv_label_set_text(displayLabel, "0");
  lv_obj_add_style(displayLabel, &styleDisplayText, 0);
  lv_obj_align(displayLabel, LV_ALIGN_RIGHT_MID, -12, 0);
  clearInteraction(displayLabel);

  const int toggleY = DISPLAY_Y - TOGGLE_H + 2;
  const int radX = DISPLAY_X + DISPLAY_W - TOGGLE_W - 8;
  const int degX = radX - TOGGLE_W - TOGGLE_GAP;
  degButton = createAngleButton(window, "DEG", degX, toggleY);
  radButton = createAngleButton(window, "RAD", radX, toggleY);
  updateAngleModeButtons();

  for (int row = 0; row < 5; ++row) {
    for (int col = 0; col < 6; ++col) {
      createButton(window, BUTTONS[row][col], row, col);
    }
  }

}
