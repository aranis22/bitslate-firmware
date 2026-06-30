#include "ChessRenderSmokeApp.h"

#include <Arduino.h>
#include <lvgl.h>

#include "ChessGameModel.h"
#include "assets/images/chess/generated/chess_piece_assets.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int BOARD_SIZE = 304;
constexpr int BOARD_X = (SCREEN_W - BOARD_SIZE) / 2;
constexpr int BOARD_Y = (SCREEN_H - BOARD_SIZE) / 2;
constexpr int SQUARE_SIZE = BOARD_SIZE / 8;
constexpr int PIECE_SIZE = 24;
constexpr int PIECE_OFFSET = (SQUARE_SIZE - PIECE_SIZE) / 2;

struct SquareData {
  int row;
  int col;
};

lv_obj_t* squareObjs[8][8] = {};
SquareData squareData[8][8] = {};
lv_obj_t* boardObj = nullptr;
lv_obj_t* pieceObjs[32] = {};
lv_obj_t* turnLabel = nullptr;
int selectedRow = -1;
int selectedCol = -1;
ChessGameModel model;
ChessGameModel::Position legalMoves[32] = {};
int legalMoveCount = 0;

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

lv_color_t color(unsigned int hex) {
  return lv_color_hex(hex);
}

void styleSquareBase(lv_obj_t* square, int row, int col) {
  bool light = ((row + col) % 2) == 0;
  lv_obj_set_style_bg_color(square, light ? color(0xF0D9B5) : color(0xB67A45), 0);
  lv_obj_set_style_bg_opa(square, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(square, 1, 0);
  lv_obj_set_style_border_color(square, light ? color(0xD5BE9D) : color(0x8C5F39), 0);
  lv_obj_set_style_radius(square, 0, 0);
  lv_obj_set_style_shadow_width(square, 0, 0);
  lv_obj_set_style_outline_width(square, 0, 0);
  lv_obj_set_style_bg_color(square, light ? color(0xE2C887) : color(0x98643A), LV_STATE_PRESSED);
}

const lv_image_dsc_t* pieceImageFor(const ChessGameModel::Piece& piece) {
  using Color = ChessGameModel::PieceColor;
  using Type = ChessGameModel::PieceType;

  if (piece.color == Color::COLOR_WHITE) {
    switch (piece.type) {
      case Type::TYPE_PAWN: return &img_pawn_white;
      case Type::TYPE_ROOK: return &img_rook_white;
      case Type::TYPE_KNIGHT: return &img_knight_white;
      case Type::TYPE_BISHOP: return &img_bishop_white;
      case Type::TYPE_QUEEN: return &img_queen_white;
      case Type::TYPE_KING: return &img_king_white;
      default: return nullptr;
    }
  }

  if (piece.color == Color::COLOR_BLACK) {
    switch (piece.type) {
      case Type::TYPE_PAWN: return &img_pawn_black;
      case Type::TYPE_ROOK: return &img_rook_black;
      case Type::TYPE_KNIGHT: return &img_knight_black;
      case Type::TYPE_BISHOP: return &img_bishop_black;
      case Type::TYPE_QUEEN: return &img_queen_black;
      case Type::TYPE_KING: return &img_king_black;
      default: return nullptr;
    }
  }

  return nullptr;
}

bool isLegalTarget(int row, int col) {
  for (int i = 0; i < legalMoveCount; ++i) {
    if (legalMoves[i].row == row && legalMoves[i].col == col) {
      return true;
    }
  }
  return false;
}

void updateTurnLabel() {
  if (turnLabel == nullptr) return;
  lv_label_set_text(turnLabel, model.getStatusText());
}

void updateSquareHighlight() {
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      lv_obj_t* square = squareObjs[row][col];
      if (square == nullptr) continue;
      styleSquareBase(square, row, col);
      if (isLegalTarget(row, col)) {
        lv_obj_set_style_border_width(square, 2, 0);
        lv_obj_set_style_border_color(square, color(0x37B24D), 0);
        lv_obj_set_style_outline_width(square, 1, 0);
        lv_obj_set_style_outline_color(square, color(0xA7E0B4), 0);
      }
    }
  }

  if (selectedRow >= 0 && selectedCol >= 0) {
    lv_obj_t* selected = squareObjs[selectedRow][selectedCol];
    if (selected != nullptr) {
      lv_obj_set_style_border_width(selected, 3, 0);
      lv_obj_set_style_border_color(selected, color(0x2D6BFF), 0);
      lv_obj_set_style_outline_width(selected, 2, 0);
      lv_obj_set_style_outline_color(selected, color(0xA8C4FF), 0);
      lv_obj_set_style_outline_pad(selected, -1, 0);
    }
  }
}

void clearSelection() {
  selectedRow = -1;
  selectedCol = -1;
  legalMoveCount = 0;
  updateSquareHighlight();
}

void refreshPieces() {
  for (lv_obj_t*& pieceObj : pieceObjs) {
    if (pieceObj != nullptr) {
      lv_obj_delete(pieceObj);
      pieceObj = nullptr;
    }
  }

  int pieceIndex = 0;
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      ChessGameModel::Piece piece = model.getPiece(row, col);
      const lv_image_dsc_t* image = pieceImageFor(piece);
      if (image == nullptr) continue;

      lv_obj_t* img = lv_image_create(squareObjs[row][col]);
      lv_image_set_src(img, image);
      lv_obj_set_pos(img, PIECE_OFFSET, PIECE_OFFSET);
      lv_obj_clear_flag(img, LV_OBJ_FLAG_CLICKABLE);
      lv_obj_add_flag(img, LV_OBJ_FLAG_EVENT_BUBBLE);
      clearInteraction(img);
      if (pieceIndex < 32) {
        pieceObjs[pieceIndex++] = img;
      }
    }
  }
}

void squareTapEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  SquareData* data = static_cast<SquareData*>(lv_event_get_user_data(event));
  if (data == nullptr) return;

  const int row = data->row;
  const int col = data->col;
  Serial.printf("Chess square tapped: row=%d col=%d\n", row, col);

  if (selectedRow < 0 || selectedCol < 0) {
    if (model.isCurrentPlayersPiece(row, col)) {
      selectedRow = row;
      selectedCol = col;
      legalMoveCount = model.legalMoves(row, col, legalMoves, 32);
      updateSquareHighlight();
    }
    updateTurnLabel();
    return;
  }

  if (row == selectedRow && col == selectedCol) {
    clearSelection();
    updateTurnLabel();
    return;
  }

  if (model.movePiece(selectedRow, selectedCol, row, col)) {
    clearSelection();
    refreshPieces();
    updateTurnLabel();
    return;
  }

  if (model.isCurrentPlayersPiece(row, col)) {
    selectedRow = row;
    selectedCol = col;
    legalMoveCount = model.legalMoves(row, col, legalMoves, 32);
    updateSquareHighlight();
  } else {
    updateSquareHighlight();
  }
  updateTurnLabel();
}

void createBoard(lv_obj_t* parent) {
  boardObj = lv_obj_create(parent);
  lv_obj_remove_style_all(boardObj);
  lv_obj_set_pos(boardObj, BOARD_X, BOARD_Y);
  lv_obj_set_size(boardObj, BOARD_SIZE, BOARD_SIZE);
  lv_obj_set_style_bg_color(boardObj, color(0x3E2B1F), 0);
  lv_obj_set_style_bg_opa(boardObj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(boardObj, 2, 0);
  lv_obj_set_style_border_color(boardObj, color(0x22170F), 0);
  lv_obj_set_style_radius(boardObj, 8, 0);
  lv_obj_set_style_pad_all(boardObj, 0, 0);
  clearInteraction(boardObj);

  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      lv_obj_t* square = lv_button_create(boardObj);
      lv_obj_remove_style_all(square);
      lv_obj_set_pos(square, col * SQUARE_SIZE, row * SQUARE_SIZE);
      lv_obj_set_size(square, SQUARE_SIZE, SQUARE_SIZE);
      styleSquareBase(square, row, col);
      squareObjs[row][col] = square;
      squareData[row][col] = {row, col};
      lv_obj_add_event_cb(square, squareTapEvent, LV_EVENT_CLICKED, &squareData[row][col]);
      clearInteraction(square);
    }
  }

  refreshPieces();
  updateTurnLabel();
  updateSquareHighlight();
}

}  // namespace

void ChessRenderSmokeApp::create() {
  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  lv_obj_remove_style_all(screen);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
  lv_obj_set_style_bg_color(screen, color(0xCDD4DB), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  clearInteraction(screen);

  turnLabel = lv_label_create(screen);
  lv_label_set_text(turnLabel, "WHITE");
  lv_obj_set_style_text_font(turnLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(turnLabel, color(0x25303B), 0);
  lv_obj_set_pos(turnLabel, 12, 6);
  clearInteraction(turnLabel);

  createBoard(screen);
}
