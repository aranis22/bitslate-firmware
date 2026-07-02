#include "GeographyTicTacToeSmokeApp.h"

#include <lvgl.h>

#include "TicTacToeModel.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int BOARD_SIZE = 204;
constexpr int CELL_SIZE = BOARD_SIZE / 3;
constexpr int BOARD_X = (SCREEN_W - BOARD_SIZE) / 2;
constexpr int BOARD_Y = 74;

struct CellData {
  int row;
  int col;
};

TicTacToeModel model;
lv_obj_t* statusLabel = nullptr;
lv_obj_t* cellButtons[3][3] = {};
lv_obj_t* cellLabels[3][3] = {};
CellData cellData[3][3] = {};

lv_color_t color(unsigned int hex) {
  return lv_color_hex(hex);
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

const char* cellText(TicTacToeModel::Cell cell) {
  switch (cell) {
    case TicTacToeModel::X:
      return "X";
    case TicTacToeModel::O:
      return "O";
    case TicTacToeModel::EMPTY:
    default:
      return "";
  }
}

unsigned int cellTextColor(TicTacToeModel::Cell cell) {
  if (cell == TicTacToeModel::X) {
    return 0x1F5FBF;
  }
  if (cell == TicTacToeModel::O) {
    return 0xB4473C;
  }
  return 0x243241;
}

void refresh() {
  if (statusLabel != nullptr) {
    lv_label_set_text(statusLabel, model.getStatusText());
  }

  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      TicTacToeModel::Cell cell = model.getCell(row, col);
      lv_label_set_text(cellLabels[row][col], cellText(cell));
      lv_obj_set_style_text_color(cellLabels[row][col], color(cellTextColor(cell)), 0);

      if (model.isFinished() || cell != TicTacToeModel::EMPTY) {
        lv_obj_set_style_bg_color(cellButtons[row][col], color(0xE7ECF1), 0);
      } else {
        lv_obj_set_style_bg_color(cellButtons[row][col], color(0xF8FAFC), 0);
      }
    }
  }
}

void cellEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }

  CellData* data = static_cast<CellData*>(lv_event_get_user_data(event));
  if (data == nullptr) {
    return;
  }

  if (model.playCell(data->row, data->col)) {
    refresh();
  }
}

void resetEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  model.reset();
  refresh();
}

void createBoard(lv_obj_t* screen) {
  lv_obj_t* board = lv_obj_create(screen);
  styleBox(board, 0xCBD5E1, 0x334155, 2, 6);
  lv_obj_set_pos(board, BOARD_X, BOARD_Y);
  lv_obj_set_size(board, BOARD_SIZE, BOARD_SIZE);
  lv_obj_set_style_pad_all(board, 0, 0);

  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      lv_obj_t* cell = lv_button_create(board);
      styleBox(cell, 0xF8FAFC, 0x64748B, 1, 0);
      lv_obj_set_pos(cell, col * CELL_SIZE, row * CELL_SIZE);
      lv_obj_set_size(cell, CELL_SIZE, CELL_SIZE);
      lv_obj_set_style_bg_color(cell, color(0xDDE7F2), LV_STATE_PRESSED);
      lv_obj_add_flag(cell, LV_OBJ_FLAG_CLICKABLE);

      lv_obj_t* label = lv_label_create(cell);
      lv_label_set_text(label, "");
      lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
      lv_obj_set_style_text_color(label, color(0x243241), 0);
      lv_obj_center(label);
      clearInteraction(label);

      cellButtons[row][col] = cell;
      cellLabels[row][col] = label;
      cellData[row][col] = {row, col};
      lv_obj_add_event_cb(cell, cellEvent, LV_EVENT_CLICKED, &cellData[row][col]);
    }
  }
}

lv_obj_t* createResetButton(lv_obj_t* screen) {
  lv_obj_t* button = lv_button_create(screen);
  styleBox(button, 0xFFFFFF, 0x64748B, 1, 6);
  lv_obj_set_pos(button, 364, 262);
  lv_obj_set_size(button, 86, 38);
  lv_obj_set_style_bg_color(button, color(0xE6EEF8), LV_STATE_PRESSED);
  lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(button, resetEvent, LV_EVENT_CLICKED, nullptr);

  lv_obj_t* label = lv_label_create(button);
  lv_label_set_text(label, "Reset");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(label, color(0x1E293B), 0);
  lv_obj_center(label);
  clearInteraction(label);
  return button;
}

}  // namespace

void GeographyTicTacToeSmokeApp::create() {
  model.reset();

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xEEF2F7, 0xEEF2F7, 0, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);

  lv_obj_t* title = makeLabel(screen, "Geography LVGL Smoke", 20, 12, &lv_font_montserrat_22, 0x172033);
  lv_obj_set_width(title, SCREEN_W - 40);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t* subtitle = makeLabel(screen, "Tic Tac Toe Placeholder", 20, 42, &lv_font_montserrat_14, 0x475569);
  lv_obj_set_width(subtitle, SCREEN_W - 40);
  lv_obj_set_style_text_align(subtitle, LV_TEXT_ALIGN_CENTER, 0);

  statusLabel = makeLabel(screen, "X turn", 28, 270, &lv_font_montserrat_22, 0x172033);
  lv_obj_set_size(statusLabel, 190, 32);

  createBoard(screen);
  createResetButton(screen);
  refresh();
}
