#include "GraphingCalculatorApp.h"

#include <Arduino.h>

#include <cmath>
#include <cctype>
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
constexpr int FRAME_X = 4;
constexpr int FRAME_Y = 4;
constexpr int FRAME_W = 472;
constexpr int FRAME_H = 312;

constexpr int GRAPH_X = 10;
constexpr int GRAPH_Y = 8;
constexpr int GRAPH_W = 320;
constexpr int GRAPH_H = 152;
constexpr int GRAPH_MARGIN_L = 18;
constexpr int GRAPH_MARGIN_R = 18;
constexpr int GRAPH_MARGIN_T = 10;
constexpr int GRAPH_MARGIN_B = 14;
constexpr int GRAPH_SAMPLES = 96;

constexpr int TABLE_X = 338;
constexpr int TABLE_Y = 8;
constexpr int TABLE_W = 124;
constexpr int TABLE_H = 152;
constexpr int TABLE_ROWS = 11;

constexpr int LEFT_PANEL_X = 12;
constexpr int READOUT_Y = 168;
constexpr int READOUT_W = 70;
constexpr int READOUT_H = 22;
constexpr int READOUT_GAP = 6;

constexpr int SLIDER_INFO_Y = 194;
constexpr int SLIDER_X = 26;
constexpr int SLIDER_Y = 210;
constexpr int SLIDER_W = 112;
constexpr int SLIDER_H = 12;
constexpr int SLIDER_MINMAX_Y = 226;
constexpr int SLIDER_MAX = 50000;

constexpr int FUNC_LABEL_Y = 236;
constexpr int OUTPUT_X = 12;
constexpr int OUTPUT_Y = 250;
constexpr int OUTPUT_W = 146;
constexpr int OUTPUT_H = 24;

constexpr int ENCODER_PIN_A = 38;
constexpr int ENCODER_PIN_B = 39;
constexpr int ENCODER_STEP = 2500;

constexpr int KEYPAD_X = 170;
constexpr int KEYPAD_Y = 168;
constexpr int BTN_W = 44;
constexpr int BTN_H = 24;
constexpr int BTN_GAP_X = 6;
constexpr int BTN_GAP_Y = 4;

constexpr float X_MIN = -5.0f;
constexpr float X_MAX = 5.0f;
constexpr float Y_MIN = -10.0f;
constexpr float Y_MAX = 10.0f;
constexpr float TRACE_DEFAULT_X = 1.7017f;

enum class ActiveValue {
  X,
  Y,
};

struct ButtonDef {
  const char* text;
  bool numberStyle;
  bool equalsStyle;
};

struct Parser {
  const char* text;
  size_t pos;
  double xValue;
  bool error;
  bool divideByZero;
};

constexpr ButtonDef BUTTONS[5][6] = {
    {
        {"7", true, false},
        {"8", true, false},
        {"9", true, false},
        {"+", false, false},
        {"SQR", false, false},
        {"SIN", false, false},
    },
    {
        {"4", true, false},
        {"5", true, false},
        {"6", true, false},
        {"-", false, false},
        {"(", false, false},
        {"COS", false, false},
    },
    {
        {"1", true, false},
        {"2", true, false},
        {"3", true, false},
        {"x", false, false},
        {")", false, false},
        {"TAN", false, false},
    },
    {
        {"0", true, false},
        {".", true, false},
        {"=", false, true},
        {"/", false, false},
        {"PI", false, false},
        {"LOG", false, false},
    },
    {
        {"+/-", false, false},
        {"^", false, false},
        {"CLR", false, false},
        {"X", false, false},
        {"DEL", false, false},
        {"LN", false, false},
    },
};

lv_obj_t* outputLabel = nullptr;
lv_obj_t* activeFunctionLabel = nullptr;
lv_obj_t* xReadoutButton = nullptr;
lv_obj_t* yReadoutButton = nullptr;
lv_obj_t* xReadoutLabel = nullptr;
lv_obj_t* yReadoutLabel = nullptr;
lv_obj_t* sliderObj = nullptr;
lv_obj_t* sliderTargetLabel = nullptr;
lv_obj_t* sliderValueLabel = nullptr;
lv_obj_t* graphPanel = nullptr;
lv_obj_t* graphCurve = nullptr;
lv_obj_t* graphCursor = nullptr;
lv_obj_t* tableYLabels[TABLE_ROWS] = {};

lv_point_precise_t graphPoints[GRAPH_SAMPLES];

char activeExpression[64] = "X^2";
char editorExpression[64] = "X^2";
char outputBuffer[96] = "X^2";
float traceX = TRACE_DEFAULT_X;
float traceY = 0.0f;
ActiveValue activeValue = ActiveValue::Y;
bool errorDisplayed = false;
uint8_t encoderPrevState = 0;
int encoderTransitionSum = 0;

bool stylesReady = false;
lv_style_t styleScreen;
lv_style_t styleFrame;
lv_style_t styleGraphPanel;
lv_style_t styleTablePanel;
lv_style_t styleReadoutButton;
lv_style_t styleReadoutButtonActive;
lv_style_t styleReadoutLabel;
lv_style_t styleReadoutLabelActive;
lv_style_t styleSlider;
lv_style_t styleSliderKnob;
lv_style_t styleOutputFrame;
lv_style_t styleOutputInner;
lv_style_t styleOutputText;
lv_style_t styleFunctionText;
lv_style_t styleButtonBase;
lv_style_t styleButtonText;
lv_style_t styleNumberButton;
lv_style_t styleNumberButtonPressed;
lv_style_t styleFunctionButton;
lv_style_t styleFunctionButtonPressed;
lv_style_t styleEqualsButton;
lv_style_t styleEqualsButtonPressed;
lv_style_t styleTableHeader;
lv_style_t styleTableHeaderText;
lv_style_t styleTableCellText;
lv_style_t styleGraphLine;
lv_style_t styleGraphLineSecondary;
lv_style_t styleAxisLine;

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void initStyleBase(lv_style_t& style) {
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_shadow_width(&style, 0);
  lv_style_set_pad_all(&style, 0);
  lv_style_set_border_width(&style, 2);
  lv_style_set_border_color(&style, color(0x111111));
  lv_style_set_outline_width(&style, 1);
  lv_style_set_outline_pad(&style, 0);
  lv_style_set_outline_color(&style, color(0x9AA3AD));
}

void ensureStyles() {
  if (stylesReady) return;

  lv_style_init(&styleScreen);
  lv_style_set_bg_color(&styleScreen, color(0x49515B));
  lv_style_set_bg_opa(&styleScreen, LV_OPA_COVER);
  lv_style_set_border_width(&styleScreen, 0);

  initStyleBase(styleFrame);
  lv_style_set_bg_color(&styleFrame, color(0xE5E7E8));
  lv_style_set_bg_opa(&styleFrame, LV_OPA_COVER);
  lv_style_set_outline_color(&styleFrame, color(0xF3F4F4));
  lv_style_set_border_color(&styleFrame, color(0x15181C));

  initStyleBase(styleGraphPanel);
  lv_style_set_bg_color(&styleGraphPanel, color(0xF7F7F5));
  lv_style_set_bg_opa(&styleGraphPanel, LV_OPA_COVER);
  lv_style_set_border_color(&styleGraphPanel, color(0x1C1C1C));
  lv_style_set_outline_color(&styleGraphPanel, color(0xC6C8CA));

  initStyleBase(styleTablePanel);
  lv_style_set_bg_color(&styleTablePanel, color(0xFBFBFB));
  lv_style_set_bg_opa(&styleTablePanel, LV_OPA_COVER);
  lv_style_set_border_color(&styleTablePanel, color(0x161616));
  lv_style_set_outline_color(&styleTablePanel, color(0xD2D5D8));

  initStyleBase(styleReadoutButton);
  lv_style_set_bg_color(&styleReadoutButton, color(0xF4F4F2));
  lv_style_set_bg_opa(&styleReadoutButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleReadoutButton, color(0xCFD2D4));
  lv_style_set_border_color(&styleReadoutButton, color(0x171717));

  initStyleBase(styleReadoutButtonActive);
  lv_style_set_bg_color(&styleReadoutButtonActive, color(0x0B1F83));
  lv_style_set_bg_opa(&styleReadoutButtonActive, LV_OPA_COVER);
  lv_style_set_outline_color(&styleReadoutButtonActive, color(0x7F94FF));
  lv_style_set_border_color(&styleReadoutButtonActive, color(0x141414));

  lv_style_init(&styleReadoutLabel);
  lv_style_set_text_font(&styleReadoutLabel, &monogram_20);
  lv_style_set_text_color(&styleReadoutLabel, color(0x141414));

  lv_style_init(&styleReadoutLabelActive);
  lv_style_set_text_font(&styleReadoutLabelActive, &monogram_20);
  lv_style_set_text_color(&styleReadoutLabelActive, color(0xF4F6FF));

  lv_style_init(&styleSlider);
  lv_style_set_bg_color(&styleSlider, color(0xA9ADB0));
  lv_style_set_bg_opa(&styleSlider, LV_OPA_COVER);
  lv_style_set_border_width(&styleSlider, 2);
  lv_style_set_border_color(&styleSlider, color(0x151515));
  lv_style_set_outline_width(&styleSlider, 1);
  lv_style_set_outline_color(&styleSlider, color(0xCBCDD0));
  lv_style_set_radius(&styleSlider, 6);

  lv_style_init(&styleSliderKnob);
  lv_style_set_bg_color(&styleSliderKnob, color(0xE9ECEF));
  lv_style_set_bg_opa(&styleSliderKnob, LV_OPA_COVER);
  lv_style_set_border_width(&styleSliderKnob, 2);
  lv_style_set_border_color(&styleSliderKnob, color(0x171717));
  lv_style_set_outline_width(&styleSliderKnob, 1);
  lv_style_set_outline_color(&styleSliderKnob, color(0xFFFFFF));
  lv_style_set_radius(&styleSliderKnob, 4);
  lv_style_set_pad_all(&styleSliderKnob, 3);

  initStyleBase(styleOutputFrame);
  lv_style_set_bg_color(&styleOutputFrame, color(0x5B636D));
  lv_style_set_bg_opa(&styleOutputFrame, LV_OPA_COVER);
  lv_style_set_border_color(&styleOutputFrame, color(0x20252A));
  lv_style_set_outline_color(&styleOutputFrame, color(0x8B949D));

  initStyleBase(styleOutputInner);
  lv_style_set_bg_color(&styleOutputInner, color(0x060908));
  lv_style_set_bg_opa(&styleOutputInner, LV_OPA_COVER);
  lv_style_set_border_color(&styleOutputInner, color(0x1B1F1C));
  lv_style_set_outline_color(&styleOutputInner, color(0x2A332A));

  lv_style_init(&styleOutputText);
  lv_style_set_text_font(&styleOutputText, &monogram_20);
  lv_style_set_text_color(&styleOutputText, color(0x25EE24));

  lv_style_init(&styleFunctionText);
  lv_style_set_text_font(&styleFunctionText, &monogram_20);
  lv_style_set_text_color(&styleFunctionText, color(0x101010));

  initStyleBase(styleButtonBase);
  lv_style_set_bg_opa(&styleButtonBase, LV_OPA_COVER);
  lv_style_set_pad_bottom(&styleButtonBase, 2);
  lv_style_set_pad_top(&styleButtonBase, 0);
  lv_style_set_pad_left(&styleButtonBase, 0);
  lv_style_set_pad_right(&styleButtonBase, 0);

  lv_style_init(&styleButtonText);
  lv_style_set_text_font(&styleButtonText, &monogram_24);
  lv_style_set_text_color(&styleButtonText, color(0x101010));

  initStyleBase(styleNumberButton);
  lv_style_set_bg_color(&styleNumberButton, color(0xF2C73E));
  lv_style_set_bg_opa(&styleNumberButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleNumberButton, color(0xFFDF70));
  lv_style_set_border_color(&styleNumberButton, color(0x8D6B0E));

  initStyleBase(styleNumberButtonPressed);
  lv_style_set_bg_color(&styleNumberButtonPressed, color(0xD6A921));
  lv_style_set_bg_opa(&styleNumberButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleNumberButtonPressed, color(0xF0C960));
  lv_style_set_border_color(&styleNumberButtonPressed, color(0x7A5D0B));

  initStyleBase(styleFunctionButton);
  lv_style_set_bg_color(&styleFunctionButton, color(0x6BC18D));
  lv_style_set_bg_opa(&styleFunctionButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleFunctionButton, color(0x9AE0B3));
  lv_style_set_border_color(&styleFunctionButton, color(0x2D6D49));

  initStyleBase(styleFunctionButtonPressed);
  lv_style_set_bg_color(&styleFunctionButtonPressed, color(0x4EA16E));
  lv_style_set_bg_opa(&styleFunctionButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleFunctionButtonPressed, color(0x80D49D));
  lv_style_set_border_color(&styleFunctionButtonPressed, color(0x245A3C));

  initStyleBase(styleEqualsButton);
  lv_style_set_bg_color(&styleEqualsButton, color(0xD36441));
  lv_style_set_bg_opa(&styleEqualsButton, LV_OPA_COVER);
  lv_style_set_outline_color(&styleEqualsButton, color(0xEE9679));
  lv_style_set_border_color(&styleEqualsButton, color(0x7E2F1A));

  initStyleBase(styleEqualsButtonPressed);
  lv_style_set_bg_color(&styleEqualsButtonPressed, color(0xB54E2E));
  lv_style_set_bg_opa(&styleEqualsButtonPressed, LV_OPA_COVER);
  lv_style_set_outline_color(&styleEqualsButtonPressed, color(0xD77F65));
  lv_style_set_border_color(&styleEqualsButtonPressed, color(0x6F2413));

  initStyleBase(styleTableHeader);
  lv_style_set_bg_color(&styleTableHeader, color(0x080808));
  lv_style_set_bg_opa(&styleTableHeader, LV_OPA_COVER);
  lv_style_set_border_color(&styleTableHeader, color(0x0F0F0F));
  lv_style_set_outline_color(&styleTableHeader, color(0xAAB0B5));

  lv_style_init(&styleTableHeaderText);
  lv_style_set_text_font(&styleTableHeaderText, &monogram_24);
  lv_style_set_text_color(&styleTableHeaderText, color(0xF7F7F7));

  lv_style_init(&styleTableCellText);
  lv_style_set_text_font(&styleTableCellText, &monogram_20);
  lv_style_set_text_color(&styleTableCellText, color(0x101010));

  lv_style_init(&styleGraphLine);
  lv_style_set_line_color(&styleGraphLine, color(0x121212));
  lv_style_set_line_width(&styleGraphLine, 3);
  lv_style_set_line_rounded(&styleGraphLine, false);

  lv_style_init(&styleGraphLineSecondary);
  lv_style_set_line_color(&styleGraphLineSecondary, color(0x222222));
  lv_style_set_line_width(&styleGraphLineSecondary, 1);
  lv_style_set_line_rounded(&styleGraphLineSecondary, false);

  lv_style_init(&styleAxisLine);
  lv_style_set_line_color(&styleAxisLine, color(0x111111));
  lv_style_set_line_width(&styleAxisLine, 2);
  lv_style_set_line_rounded(&styleAxisLine, false);

  stylesReady = true;
}

void setOutputText(const char* text) {
  if (outputLabel == nullptr) return;
  std::snprintf(outputBuffer, sizeof(outputBuffer), "%s", text);
  lv_label_set_text(outputLabel, outputBuffer);
}

void showEditorExpression() {
  setOutputText(editorExpression);
  errorDisplayed = false;
}

void updateFunctionLabel() {
  if (activeFunctionLabel == nullptr) return;
  char buffer[80];
  std::snprintf(buffer, sizeof(buffer), "Y=%s", activeExpression);
  lv_label_set_text(activeFunctionLabel, buffer);
}

void formatReadout(char* out, size_t outSize, char prefix, float value) {
  std::snprintf(out, outSize, "%c=%.4f", prefix, static_cast<double>(value));
}

void updateReadoutSelection() {
  if (xReadoutButton == nullptr || yReadoutButton == nullptr ||
      xReadoutLabel == nullptr || yReadoutLabel == nullptr ||
      sliderTargetLabel == nullptr || sliderValueLabel == nullptr) {
    return;
  }

  if (activeValue == ActiveValue::Y) {
    lv_obj_add_style(yReadoutButton, &styleReadoutButtonActive, 0);
    lv_obj_remove_style(xReadoutButton, &styleReadoutButtonActive, 0);
    lv_obj_add_style(yReadoutLabel, &styleReadoutLabelActive, 0);
    lv_obj_remove_style(yReadoutLabel, &styleReadoutLabel, 0);
    lv_obj_add_style(xReadoutLabel, &styleReadoutLabel, 0);
    lv_obj_remove_style(xReadoutLabel, &styleReadoutLabelActive, 0);
  } else {
    lv_obj_add_style(xReadoutButton, &styleReadoutButtonActive, 0);
    lv_obj_remove_style(yReadoutButton, &styleReadoutButtonActive, 0);
    lv_obj_add_style(xReadoutLabel, &styleReadoutLabelActive, 0);
    lv_obj_remove_style(xReadoutLabel, &styleReadoutLabel, 0);
    lv_obj_add_style(yReadoutLabel, &styleReadoutLabel, 0);
    lv_obj_remove_style(yReadoutLabel, &styleReadoutLabelActive, 0);
  }

  lv_label_set_text(sliderTargetLabel, "TRACE X");
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%.4f", static_cast<double>(traceX));
  lv_label_set_text(sliderValueLabel, buffer);
}

bool isValueEndChar(char c) {
  return (c >= '0' && c <= '9') || c == '.' || c == 'X' || c == ')' || c == 'I';
}

bool isValueStartChar(char c) {
  return (c >= '0' && c <= '9') || c == '.' || c == 'X' || c == '(' ||
         c == 'P' || c == 'S' || c == 'C' || c == 'T' || c == 'L';
}

void appendCharSafe(char* out, size_t outSize, size_t& outPos, char c) {
  if (outPos + 1 >= outSize) return;
  out[outPos++] = c;
  out[outPos] = '\0';
}

void normalizeExpression(const char* input, char* output, size_t outputSize) {
  char cleaned[64];
  size_t cleanPos = 0;
  cleaned[0] = '\0';

  for (size_t i = 0; input[i] != '\0' && cleanPos + 1 < sizeof(cleaned); ++i) {
    unsigned char ch = static_cast<unsigned char>(input[i]);
    if (std::isspace(ch)) {
      continue;
    }

    if (ch == 'x') {
      cleaned[cleanPos++] = '*';
    } else if (std::isalpha(ch)) {
      cleaned[cleanPos++] = static_cast<char>(std::toupper(ch));
    } else {
      cleaned[cleanPos++] = static_cast<char>(ch);
    }
  }
  cleaned[cleanPos] = '\0';

  const char* source = cleaned;
  if (cleaned[0] == 'Y' && cleaned[1] == '=') {
    source += 2;
  }

  size_t outPos = 0;
  output[0] = '\0';
  for (size_t i = 0; source[i] != '\0'; ++i) {
    char current = source[i];
    if (outPos > 0) {
      char previous = output[outPos - 1];
      if (isValueEndChar(previous) && isValueStartChar(current)) {
        appendCharSafe(output, outputSize, outPos, '*');
      }
    }
    appendCharSafe(output, outputSize, outPos, current);
  }
}

void skipSpaces(Parser& parser) {
  while (parser.text[parser.pos] == ' ') {
    ++parser.pos;
  }
}

double parseExpression(Parser& parser);

bool matchToken(Parser& parser, const char* token) {
  skipSpaces(parser);
  size_t len = std::strlen(token);
  if (std::strncmp(parser.text + parser.pos, token, len) == 0) {
    parser.pos += len;
    return true;
  }
  return false;
}

bool peekToken(const Parser& parser, const char* token) {
  size_t len = std::strlen(token);
  return std::strncmp(parser.text + parser.pos, token, len) == 0;
}

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

double parsePrimary(Parser& parser) {
  skipSpaces(parser);

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

  if (matchToken(parser, "X")) {
    return parser.xValue;
  }

  if (peekToken(parser, "SQR(") ||
      peekToken(parser, "SIN(") ||
      peekToken(parser, "COS(") ||
      peekToken(parser, "TAN(") ||
      peekToken(parser, "LOG(") ||
      peekToken(parser, "LN(")) {
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

    // Graphing calculator v0 uses radians for trig for now.
    if (std::strcmp(fn, "SIN") == 0) return std::sin(value);
    if (std::strcmp(fn, "COS") == 0) return std::cos(value);
    if (std::strcmp(fn, "TAN") == 0) {
      double c = std::cos(value);
      if (std::fabs(c) < 1e-9) {
        parser.error = true;
        return 0.0;
      }
      return std::tan(value);
    }

    parser.error = true;
    return 0.0;
  }

  return parseNumber(parser);
}

double parseUnary(Parser& parser) {
  skipSpaces(parser);
  if (matchToken(parser, "-")) return -parseUnary(parser);
  if (matchToken(parser, "+")) return parseUnary(parser);
  return parsePrimary(parser);
}

double parsePower(Parser& parser) {
  double base = parseUnary(parser);
  skipSpaces(parser);
  if (matchToken(parser, "^")) {
    double exponent = parsePower(parser);
    return std::pow(base, exponent);
  }
  return base;
}

double parseTerm(Parser& parser) {
  double value = parsePower(parser);
  while (!parser.error && !parser.divideByZero) {
    if (matchToken(parser, "x") || matchToken(parser, "*")) {
      value *= parsePower(parser);
    } else if (matchToken(parser, "/")) {
      double divisor = parsePower(parser);
      if (std::fabs(divisor) < 1e-12) {
        parser.divideByZero = true;
        return 0.0;
      }
      value /= divisor;
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

bool evaluateExpression(const char* expression, double xValue, double& outValue, bool& divideByZero) {
  Parser parser{expression, 0, xValue, false, false};
  outValue = parseExpression(parser);
  skipSpaces(parser);
  divideByZero = parser.divideByZero;
  if (parser.divideByZero) return false;
  if (parser.error || parser.text[parser.pos] != '\0' || !std::isfinite(outValue)) return false;
  return true;
}

int graphPixelX(float xValue) {
  float norm = (xValue - X_MIN) / (X_MAX - X_MIN);
  return GRAPH_MARGIN_L + static_cast<int>(norm * static_cast<float>(GRAPH_W - GRAPH_MARGIN_L - GRAPH_MARGIN_R));
}

int graphPixelY(float yValue) {
  float norm = (yValue - Y_MIN) / (Y_MAX - Y_MIN);
  return GRAPH_H - GRAPH_MARGIN_B - static_cast<int>(norm * static_cast<float>(GRAPH_H - GRAPH_MARGIN_T - GRAPH_MARGIN_B));
}

bool evaluateActiveAt(float xValue, float& outY) {
  double value = 0.0;
  bool div0 = false;
  if (!evaluateExpression(activeExpression, xValue, value, div0)) return false;
  outY = static_cast<float>(value);
  return std::isfinite(outY);
}

void updateReadouts() {
  if (xReadoutLabel == nullptr || yReadoutLabel == nullptr) return;
  char xBuffer[32];
  char yBuffer[32];
  formatReadout(xBuffer, sizeof(xBuffer), 'X', traceX);
  formatReadout(yBuffer, sizeof(yBuffer), 'Y', traceY);
  lv_label_set_text(xReadoutLabel, xBuffer);
  lv_label_set_text(yReadoutLabel, yBuffer);
  updateReadoutSelection();
}

void updateTable() {
  for (int i = 0; i < TABLE_ROWS; ++i) {
    if (tableYLabels[i] == nullptr) continue;
    float y = 0.0f;
    char buffer[24];
    if (evaluateActiveAt(static_cast<float>(i), y)) {
      std::snprintf(buffer, sizeof(buffer), "%.2f", static_cast<double>(y));
    } else {
      std::snprintf(buffer, sizeof(buffer), "--");
    }
    lv_label_set_text(tableYLabels[i], buffer);
  }
}

void updateGraph() {
  if (graphCurve == nullptr || graphCursor == nullptr) return;

  for (int i = 0; i < GRAPH_SAMPLES; ++i) {
    float x = X_MIN + (X_MAX - X_MIN) * static_cast<float>(i) / static_cast<float>(GRAPH_SAMPLES - 1);
    float y = 0.0f;
    graphPoints[i].x = graphPixelX(x);
    if (!evaluateActiveAt(x, y)) {
      graphPoints[i].y = graphPixelY(0.0f);
    } else {
      if (y < Y_MIN) y = Y_MIN;
      if (y > Y_MAX) y = Y_MAX;
      graphPoints[i].y = graphPixelY(y);
    }
  }
  lv_line_set_points(graphCurve, graphPoints, GRAPH_SAMPLES);

  int cursorX = graphPixelX(traceX) - 3;
  int cursorY = graphPixelY(traceY) - 3;
  lv_obj_set_pos(graphCursor, cursorX, cursorY);
}

void syncTraceFromSlider() {
  if (sliderObj == nullptr) return;
  traceX = static_cast<float>(lv_slider_get_value(sliderObj)) / 10000.0f;
  float y = 0.0f;
  if (evaluateActiveAt(traceX, y)) {
    traceY = y;
  } else {
    traceY = 0.0f;
  }
  updateReadouts();
  updateGraph();
}

void setSliderFromTraceValue(int sliderValue) {
  if (sliderObj == nullptr) return;

  if (sliderValue < 0) sliderValue = 0;
  if (sliderValue > SLIDER_MAX) sliderValue = SLIDER_MAX;

  int currentValue = lv_slider_get_value(sliderObj);
  if (sliderValue == currentValue) return;

  lv_slider_set_value(sliderObj, sliderValue, LV_ANIM_OFF);
  syncTraceFromSlider();
}

int decodeEncoderStep(uint8_t previousState, uint8_t currentState) {
  switch ((previousState << 2) | currentState) {
    case 0b0001:
    case 0b0111:
    case 0b1110:
    case 0b1000:
      return 1;
    case 0b0010:
    case 0b0100:
    case 0b1101:
    case 0b1011:
      return -1;
    default:
      return 0;
  }
}

void applyExpression() {
  char normalized[64];
  normalizeExpression(editorExpression, normalized, sizeof(normalized));
  if (normalized[0] == '\0') {
    setOutputText("ERR");
    errorDisplayed = true;
    return;
  }

  double testY = 0.0;
  bool div0 = false;
  if (!evaluateExpression(normalized, traceX, testY, div0)) {
    setOutputText(div0 ? "DIV 0" : "ERR");
    errorDisplayed = true;
    return;
  }

  std::snprintf(activeExpression, sizeof(activeExpression), "%s", normalized);
  updateFunctionLabel();
  traceY = static_cast<float>(testY);
  updateReadouts();
  updateTable();
  updateGraph();
  showEditorExpression();
}

void selectReadoutEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  const char* which = static_cast<const char*>(lv_event_get_user_data(event));
  activeValue = (which != nullptr && std::strcmp(which, "X") == 0) ? ActiveValue::X : ActiveValue::Y;
  updateReadoutSelection();
}

void sliderEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_VALUE_CHANGED) return;
  syncTraceFromSlider();
}

void trimEditorIfNeeded() {
  if (std::strlen(editorExpression) >= sizeof(editorExpression) - 2) {
    editorExpression[sizeof(editorExpression) - 2] = '\0';
  }
}

void appendEditorToken(const char* token) {
  char buffer[64];
  std::snprintf(buffer, sizeof(buffer), "%s%s", editorExpression, token);
  std::snprintf(editorExpression, sizeof(editorExpression), "%s", buffer);
  trimEditorIfNeeded();
  showEditorExpression();
}

void deleteEditorChar() {
  size_t len = std::strlen(editorExpression);
  if (len == 0) return;
  editorExpression[len - 1] = '\0';
  showEditorExpression();
}

void clearEditor() {
  editorExpression[0] = '\0';
  showEditorExpression();
}

void keypadEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  const ButtonDef* def = static_cast<const ButtonDef*>(lv_event_get_user_data(event));
  if (def == nullptr) return;

  if (errorDisplayed) {
    showEditorExpression();
  }

  if (std::strcmp(def->text, "=") == 0) {
    applyExpression();
    return;
  }

  if (std::strcmp(def->text, "CLR") == 0) {
    clearEditor();
    return;
  }

  if (std::strcmp(def->text, "DEL") == 0) {
    deleteEditorChar();
    return;
  }

  if (std::strcmp(def->text, "+/-") == 0) {
    appendEditorToken("-");
    return;
  }

  if (std::strcmp(def->text, "SQR") == 0 ||
      std::strcmp(def->text, "SIN") == 0 ||
      std::strcmp(def->text, "COS") == 0 ||
      std::strcmp(def->text, "TAN") == 0 ||
      std::strcmp(def->text, "LOG") == 0 ||
      std::strcmp(def->text, "LN") == 0) {
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%s(", def->text);
    appendEditorToken(buffer);
    return;
  }

  appendEditorToken(def->text);
}

lv_obj_t* createText(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font, uint32_t colorHex) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, color(colorHex), 0);
  clearInteraction(label);
  return label;
}

void createLine(lv_obj_t* parent, const lv_point_precise_t* points, uint32_t count, const lv_style_t* style) {
  lv_obj_t* line = lv_line_create(parent);
  lv_line_set_points(line, points, count);
  lv_obj_add_style(line, const_cast<lv_style_t*>(style), 0);
  clearInteraction(line);
}

void createGraphPanel(lv_obj_t* parent) {
  graphPanel = lv_obj_create(parent);
  lv_obj_remove_style_all(graphPanel);
  lv_obj_add_style(graphPanel, &styleGraphPanel, 0);
  lv_obj_set_pos(graphPanel, GRAPH_X, GRAPH_Y);
  lv_obj_set_size(graphPanel, GRAPH_W, GRAPH_H);
  clearInteraction(graphPanel);

  static const lv_point_precise_t axisH[] = {{GRAPH_MARGIN_L, graphPixelY(0.0f)}, {GRAPH_W - GRAPH_MARGIN_R, graphPixelY(0.0f)}};
  static const lv_point_precise_t axisV[] = {{graphPixelX(0.0f), GRAPH_MARGIN_T}, {graphPixelX(0.0f), GRAPH_H - GRAPH_MARGIN_B}};
  createLine(graphPanel, axisH, 2, &styleAxisLine);
  createLine(graphPanel, axisV, 2, &styleAxisLine);

  static const lv_point_precise_t tick0[] = {{graphPixelX(-2.0f), graphPixelY(0.0f) - 4}, {graphPixelX(-2.0f), graphPixelY(0.0f) + 4}};
  static const lv_point_precise_t tick1[] = {{graphPixelX(2.0f), graphPixelY(0.0f) - 4}, {graphPixelX(2.0f), graphPixelY(0.0f) + 4}};
  static const lv_point_precise_t tick2[] = {{graphPixelX(-4.0f), graphPixelY(0.0f) - 4}, {graphPixelX(-4.0f), graphPixelY(0.0f) + 4}};
  static const lv_point_precise_t tick3[] = {{graphPixelX(4.0f), graphPixelY(0.0f) - 4}, {graphPixelX(4.0f), graphPixelY(0.0f) + 4}};
  static const lv_point_precise_t vtick0[] = {{graphPixelX(0.0f) - 4, graphPixelY(5.0f)}, {graphPixelX(0.0f) + 4, graphPixelY(5.0f)}};
  static const lv_point_precise_t vtick1[] = {{graphPixelX(0.0f) - 4, graphPixelY(2.5f)}, {graphPixelX(0.0f) + 4, graphPixelY(2.5f)}};
  static const lv_point_precise_t vtick2[] = {{graphPixelX(0.0f) - 4, graphPixelY(-2.5f)}, {graphPixelX(0.0f) + 4, graphPixelY(-2.5f)}};
  static const lv_point_precise_t vtick3[] = {{graphPixelX(0.0f) - 4, graphPixelY(-5.0f)}, {graphPixelX(0.0f) + 4, graphPixelY(-5.0f)}};
  createLine(graphPanel, tick0, 2, &styleAxisLine);
  createLine(graphPanel, tick1, 2, &styleAxisLine);
  createLine(graphPanel, tick2, 2, &styleAxisLine);
  createLine(graphPanel, tick3, 2, &styleAxisLine);
  createLine(graphPanel, vtick0, 2, &styleAxisLine);
  createLine(graphPanel, vtick1, 2, &styleAxisLine);
  createLine(graphPanel, vtick2, 2, &styleAxisLine);
  createLine(graphPanel, vtick3, 2, &styleAxisLine);

  graphCurve = lv_line_create(graphPanel);
  lv_obj_add_style(graphCurve, &styleGraphLine, 0);
  clearInteraction(graphCurve);

  graphCursor = lv_obj_create(graphPanel);
  lv_obj_remove_style_all(graphCursor);
  lv_obj_set_size(graphCursor, 6, 6);
  lv_obj_set_style_radius(graphCursor, 3, 0);
  lv_obj_set_style_bg_color(graphCursor, color(0x101010), 0);
  lv_obj_set_style_bg_opa(graphCursor, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(graphCursor, 1, 0);
  lv_obj_set_style_border_color(graphCursor, color(0xFAFAFA), 0);
  clearInteraction(graphCursor);
}

void createTablePanel(lv_obj_t* parent) {
  lv_obj_t* table = lv_obj_create(parent);
  lv_obj_remove_style_all(table);
  lv_obj_add_style(table, &styleTablePanel, 0);
  lv_obj_set_pos(table, TABLE_X, TABLE_Y);
  lv_obj_set_size(table, TABLE_W, TABLE_H);
  clearInteraction(table);

  lv_obj_t* headerX = lv_obj_create(table);
  lv_obj_remove_style_all(headerX);
  lv_obj_add_style(headerX, &styleTableHeader, 0);
  lv_obj_set_pos(headerX, 0, 0);
  lv_obj_set_size(headerX, TABLE_W / 2, 22);
  clearInteraction(headerX);
  lv_obj_t* labelX = lv_label_create(headerX);
  lv_label_set_text(labelX, "X");
  lv_obj_add_style(labelX, &styleTableHeaderText, 0);
  lv_obj_center(labelX);

  lv_obj_t* headerY = lv_obj_create(table);
  lv_obj_remove_style_all(headerY);
  lv_obj_add_style(headerY, &styleTableHeader, 0);
  lv_obj_set_pos(headerY, TABLE_W / 2, 0);
  lv_obj_set_size(headerY, TABLE_W / 2, 22);
  clearInteraction(headerY);
  lv_obj_t* labelY = lv_label_create(headerY);
  lv_label_set_text(labelY, "Y2");
  lv_obj_add_style(labelY, &styleTableHeaderText, 0);
  lv_obj_center(labelY);

  for (int i = 0; i < TABLE_ROWS; ++i) {
    int rowY = 26 + i * 11;
    char xBuffer[8];
    std::snprintf(xBuffer, sizeof(xBuffer), "%d", i);
    createText(table, xBuffer, 18, rowY, &monogram_20, 0x101010);
    tableYLabels[i] = createText(table, "--", 70, rowY, &monogram_20, 0x3451F0);
  }
}

void createReadouts(lv_obj_t* parent) {
  yReadoutButton = lv_button_create(parent);
  lv_obj_add_style(yReadoutButton, &styleReadoutButton, 0);
  lv_obj_add_event_cb(yReadoutButton, selectReadoutEvent, LV_EVENT_CLICKED, const_cast<char*>("Y"));
  lv_obj_set_pos(yReadoutButton, LEFT_PANEL_X, READOUT_Y);
  lv_obj_set_size(yReadoutButton, READOUT_W, READOUT_H);
  clearInteraction(yReadoutButton);

  yReadoutLabel = lv_label_create(yReadoutButton);
  lv_obj_add_style(yReadoutLabel, &styleReadoutLabel, 0);
  lv_obj_center(yReadoutLabel);

  xReadoutButton = lv_button_create(parent);
  lv_obj_add_style(xReadoutButton, &styleReadoutButton, 0);
  lv_obj_add_event_cb(xReadoutButton, selectReadoutEvent, LV_EVENT_CLICKED, const_cast<char*>("X"));
  lv_obj_set_pos(xReadoutButton, LEFT_PANEL_X + READOUT_W + READOUT_GAP, READOUT_Y);
  lv_obj_set_size(xReadoutButton, READOUT_W, READOUT_H);
  clearInteraction(xReadoutButton);

  xReadoutLabel = lv_label_create(xReadoutButton);
  lv_obj_add_style(xReadoutLabel, &styleReadoutLabel, 0);
  lv_obj_center(xReadoutLabel);
}

void createSliderArea(lv_obj_t* parent) {
  sliderTargetLabel = createText(parent, "TRACE X", LEFT_PANEL_X, SLIDER_INFO_Y, &monogram_20, 0x111111);
  sliderValueLabel = createText(parent, "1.7017", LEFT_PANEL_X + 82, SLIDER_INFO_Y, &monogram_20, 0x111111);

  sliderObj = lv_slider_create(parent);
  lv_obj_set_pos(sliderObj, SLIDER_X, SLIDER_Y);
  lv_obj_set_size(sliderObj, SLIDER_W, SLIDER_H);
  lv_slider_set_range(sliderObj, 0, SLIDER_MAX);
  lv_slider_set_value(sliderObj, static_cast<int>(TRACE_DEFAULT_X * 10000.0f), LV_ANIM_OFF);
  lv_obj_add_style(sliderObj, &styleSlider, LV_PART_MAIN);
  lv_obj_add_style(sliderObj, &styleFunctionButton, LV_PART_INDICATOR);
  lv_obj_add_style(sliderObj, &styleSliderKnob, LV_PART_KNOB);
  lv_obj_add_event_cb(sliderObj, sliderEvent, LV_EVENT_VALUE_CHANGED, nullptr);
  clearInteraction(sliderObj);

  createText(parent, "0.0000", LEFT_PANEL_X, SLIDER_MINMAX_Y, &monogram_16, 0x111111);
  createText(parent, "5.0000", LEFT_PANEL_X + 86, SLIDER_MINMAX_Y, &monogram_16, 0x111111);
}

void createOutputDisplay(lv_obj_t* parent) {
  activeFunctionLabel = createText(parent, "Y=X^2", OUTPUT_X, FUNC_LABEL_Y, &monogram_20, 0x101010);

  lv_obj_t* frame = lv_obj_create(parent);
  lv_obj_remove_style_all(frame);
  lv_obj_add_style(frame, &styleOutputFrame, 0);
  lv_obj_set_pos(frame, OUTPUT_X, OUTPUT_Y);
  lv_obj_set_size(frame, OUTPUT_W, OUTPUT_H);
  clearInteraction(frame);

  lv_obj_t* inner = lv_obj_create(frame);
  lv_obj_remove_style_all(inner);
  lv_obj_add_style(inner, &styleOutputInner, 0);
  lv_obj_set_pos(inner, 4, 4);
  lv_obj_set_size(inner, OUTPUT_W - 8, OUTPUT_H - 8);
  clearInteraction(inner);

  outputLabel = lv_label_create(inner);
  lv_label_set_text(outputLabel, editorExpression);
  lv_obj_add_style(outputLabel, &styleOutputText, 0);
  lv_obj_set_width(outputLabel, OUTPUT_W - 20);
  lv_obj_set_style_text_align(outputLabel, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_align(outputLabel, LV_ALIGN_RIGHT_MID, -6, 0);
  clearInteraction(outputLabel);
}

void createButton(lv_obj_t* parent, const ButtonDef& def, int row, int col) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_add_style(btn, &styleButtonBase, 0);
  if (def.equalsStyle) {
    lv_obj_add_style(btn, &styleEqualsButton, 0);
    lv_obj_add_style(btn, &styleEqualsButtonPressed, LV_STATE_PRESSED);
  } else if (def.numberStyle) {
    lv_obj_add_style(btn, &styleNumberButton, 0);
    lv_obj_add_style(btn, &styleNumberButtonPressed, LV_STATE_PRESSED);
  } else {
    lv_obj_add_style(btn, &styleFunctionButton, 0);
    lv_obj_add_style(btn, &styleFunctionButtonPressed, LV_STATE_PRESSED);
  }

  lv_obj_set_pos(btn, KEYPAD_X + col * (BTN_W + BTN_GAP_X), KEYPAD_Y + row * (BTN_H + BTN_GAP_Y));
  lv_obj_set_size(btn, BTN_W, BTN_H);
  lv_obj_add_event_cb(btn, keypadEvent, LV_EVENT_CLICKED, const_cast<ButtonDef*>(&def));
  clearInteraction(btn);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, def.text);
  lv_obj_add_style(label, &styleButtonText, 0);
  if (std::strlen(def.text) >= 3) {
    lv_obj_set_style_text_font(label, &monogram_20, 0);
  } else {
    lv_obj_set_style_text_font(label, &monogram_24, 0);
  }
  lv_obj_center(label);
}

void createKeypad(lv_obj_t* parent) {
  for (int row = 0; row < 5; ++row) {
    for (int col = 0; col < 6; ++col) {
      createButton(parent, BUTTONS[row][col], row, col);
    }
  }
}

}  // namespace

void GraphingCalculatorApp::create() {
  ensureStyles();

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  encoderPrevState = static_cast<uint8_t>((digitalRead(ENCODER_PIN_A) << 1) | digitalRead(ENCODER_PIN_B));
  encoderTransitionSum = 0;

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  lv_obj_remove_style_all(screen);
  lv_obj_add_style(screen, &styleScreen, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
  clearInteraction(screen);

  lv_obj_t* frame = lv_obj_create(screen);
  lv_obj_remove_style_all(frame);
  lv_obj_add_style(frame, &styleFrame, 0);
  lv_obj_set_pos(frame, FRAME_X, FRAME_Y);
  lv_obj_set_size(frame, FRAME_W, FRAME_H);
  clearInteraction(frame);

  createGraphPanel(frame);
  createTablePanel(frame);
  createReadouts(frame);
  createSliderArea(frame);
  createOutputDisplay(frame);
  createKeypad(frame);

  updateFunctionLabel();
  updateTable();
  syncTraceFromSlider();
  updateReadouts();
  showEditorExpression();
}

void GraphingCalculatorApp::update() {
  uint8_t currentState = static_cast<uint8_t>((digitalRead(ENCODER_PIN_A) << 1) | digitalRead(ENCODER_PIN_B));
  if (currentState == encoderPrevState) return;

  encoderTransitionSum += decodeEncoderStep(encoderPrevState, currentState);
  encoderPrevState = currentState;

  if (encoderTransitionSum >= 4) {
    encoderTransitionSum = 0;
    setSliderFromTraceValue(lv_slider_get_value(sliderObj) + ENCODER_STEP);
  } else if (encoderTransitionSum <= -4) {
    encoderTransitionSum = 0;
    setSliderFromTraceValue(lv_slider_get_value(sliderObj) - ENCODER_STEP);
  }
}
