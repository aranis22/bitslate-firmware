#include "ChessGameModel.h"

#include <cstdio>

namespace {

const ChessGameModel::Position kRookDirs[] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},
};

const ChessGameModel::Position kBishopDirs[] = {
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1},
};

const ChessGameModel::Position kQueenDirs[] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1},
};

const ChessGameModel::Position kKnightSteps[] = {
    {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
    {1, -2}, {1, 2}, {2, -1}, {2, 1},
};

const ChessGameModel::Position kKingSteps[] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},
    {1, -1},  {1, 0},  {1, 1},
};

ChessGameModel::Piece makePiece(ChessGameModel::PieceColor color, ChessGameModel::PieceType type) {
  ChessGameModel::Piece piece{color, type};
  return piece;
}

const char* colorName(ChessGameModel::PieceColor color) {
  switch (color) {
    case ChessGameModel::COLOR_WHITE: return "WHITE";
    case ChessGameModel::COLOR_BLACK: return "BLACK";
    default: return "--";
  }
}

}  // namespace

ChessGameModel::ChessGameModel() {
  reset();
}

void ChessGameModel::reset() {
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      board_[row][col] = makePiece(COLOR_NONE, TYPE_NONE);
    }
  }

  const PieceType backRank[8] = {
      TYPE_ROOK, TYPE_KNIGHT, TYPE_BISHOP, TYPE_QUEEN,
      TYPE_KING, TYPE_BISHOP, TYPE_KNIGHT, TYPE_ROOK
  };

  for (int col = 0; col < 8; ++col) {
    board_[0][col] = makePiece(COLOR_BLACK, backRank[col]);
    board_[1][col] = makePiece(COLOR_BLACK, TYPE_PAWN);
    board_[6][col] = makePiece(COLOR_WHITE, TYPE_PAWN);
    board_[7][col] = makePiece(COLOR_WHITE, backRank[col]);
  }

  currentTurn_ = COLOR_WHITE;
  setStatus("WHITE");
}

ChessGameModel::Piece ChessGameModel::getPiece(int row, int col) const {
  if (!inBounds(row, col)) {
    return makePiece(COLOR_NONE, TYPE_NONE);
  }
  return board_[row][col];
}

ChessGameModel::PieceColor ChessGameModel::getCurrentTurn() const {
  return currentTurn_;
}

const char* ChessGameModel::getStatusText() const {
  return statusText_;
}

bool ChessGameModel::isCurrentPlayersPiece(int row, int col) const {
  if (!inBounds(row, col)) return false;
  const Piece piece = board_[row][col];
  return piece.color == currentTurn_;
}

int ChessGameModel::legalMoves(int row, int col, Position* outMoves, int maxMoves) const {
  if (!inBounds(row, col)) return 0;
  const Piece piece = board_[row][col];
  if (piece.color == COLOR_NONE) return 0;
  return collectLegalMoves(row, col, piece, outMoves, maxMoves);
}

bool ChessGameModel::isLegalMove(int fromRow, int fromCol, int toRow, int toCol) const {
  Position moves[32];
  int moveCount = legalMoves(fromRow, fromCol, moves, 32);
  for (int i = 0; i < moveCount; ++i) {
    if (moves[i].row == toRow && moves[i].col == toCol) {
      return true;
    }
  }
  return false;
}

bool ChessGameModel::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
  if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol)) {
    setStatus("OUT OF BOUNDS");
    return false;
  }

  Piece piece = board_[fromRow][fromCol];
  if (piece.color == COLOR_NONE) {
    setStatus("NO PIECE");
    return false;
  }
  if (piece.color != currentTurn_) {
    setStatus(colorName(currentTurn_));
    return false;
  }
  if (!isLegalMove(fromRow, fromCol, toRow, toCol)) {
    setStatus("ILLEGAL");
    return false;
  }

  board_[toRow][toCol] = piece;
  board_[fromRow][fromCol] = makePiece(COLOR_NONE, TYPE_NONE);

  if (piece.type == TYPE_PAWN && (toRow == 0 || toRow == 7)) {
    board_[toRow][toCol] = makePiece(piece.color, TYPE_QUEEN);
  }

  currentTurn_ = (currentTurn_ == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
  setStatus(colorName(currentTurn_));
  return true;
}

int ChessGameModel::getPieceValue(const Piece& piece) const {
  switch (piece.type) {
    case TYPE_QUEEN: return 9;
    case TYPE_ROOK: return 5;
    case TYPE_BISHOP: return 3;
    case TYPE_KNIGHT: return 3;
    case TYPE_PAWN: return 1;
    case TYPE_KING: return 100;
    default: return 0;
  }
}

bool ChessGameModel::findAutoOpponentMove(ChessMove& outMove) const {
  if (currentTurn_ != COLOR_BLACK) {
    return false;
  }

  Position moves[32];
  bool foundMove = false;
  int bestScore = -1;

  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      const Piece piece = board_[row][col];
      if (piece.color != COLOR_BLACK) continue;

      const int moveCount = collectLegalMoves(row, col, piece, moves, 32);
      for (int i = 0; i < moveCount; ++i) {
        const Position target = moves[i];
        const Piece captured = board_[target.row][target.col];
        const int score = getPieceValue(captured);
        if (!foundMove || score > bestScore) {
          foundMove = true;
          bestScore = score;
          outMove = {row, col, target.row, target.col};
        }
      }
    }
  }

  return foundMove;
}

bool ChessGameModel::makeAutoOpponentMove() {
  ChessMove move{};
  if (!findAutoOpponentMove(move)) {
    setStatus("NO CPU MOVE");
    return false;
  }

  Piece piece = board_[move.fromRow][move.fromCol];
  board_[move.toRow][move.toCol] = piece;
  board_[move.fromRow][move.fromCol] = makePiece(COLOR_NONE, TYPE_NONE);

  if (piece.type == TYPE_PAWN && move.toRow == 7) {
    board_[move.toRow][move.toCol] = makePiece(piece.color, TYPE_QUEEN);
  }

  currentTurn_ = COLOR_WHITE;
  setStatus("CPU MOVED");
  return true;
}

bool ChessGameModel::inBounds(int row, int col) const {
  return row >= 0 && row < 8 && col >= 0 && col < 8;
}

void ChessGameModel::setStatus(const char* text) {
  std::snprintf(statusText_, sizeof(statusText_), "%s", text);
}

void ChessGameModel::appendMove(Position* outMoves, int& moveCount, int maxMoves, int row, int col) const {
  if (moveCount >= maxMoves) return;
  outMoves[moveCount].row = row;
  outMoves[moveCount].col = col;
  ++moveCount;
}

int ChessGameModel::collectLegalMoves(int row, int col, const Piece& piece, Position* outMoves, int maxMoves) const {
  switch (piece.type) {
    case TYPE_PAWN:
      return collectPawnMoves(row, col, piece, outMoves, maxMoves);
    case TYPE_ROOK:
      return collectRayMoves(row, col, piece, kRookDirs, 4, outMoves, maxMoves);
    case TYPE_BISHOP:
      return collectRayMoves(row, col, piece, kBishopDirs, 4, outMoves, maxMoves);
    case TYPE_QUEEN:
      return collectRayMoves(row, col, piece, kQueenDirs, 8, outMoves, maxMoves);
    case TYPE_KNIGHT:
      return collectStepMoves(row, col, piece, kKnightSteps, 8, outMoves, maxMoves);
    case TYPE_KING:
      return collectStepMoves(row, col, piece, kKingSteps, 8, outMoves, maxMoves);
    default:
      return 0;
  }
}

int ChessGameModel::collectPawnMoves(int row, int col, const Piece& piece, Position* outMoves, int maxMoves) const {
  int moveCount = 0;
  int direction = (piece.color == COLOR_WHITE) ? -1 : 1;
  int startRow = (piece.color == COLOR_WHITE) ? 6 : 1;

  int oneRow = row + direction;
  if (inBounds(oneRow, col) && board_[oneRow][col].color == COLOR_NONE) {
    appendMove(outMoves, moveCount, maxMoves, oneRow, col);

    int twoRow = row + 2 * direction;
    if (row == startRow && inBounds(twoRow, col) && board_[twoRow][col].color == COLOR_NONE) {
      appendMove(outMoves, moveCount, maxMoves, twoRow, col);
    }
  }

  for (int dc = -1; dc <= 1; dc += 2) {
    int captureRow = row + direction;
    int captureCol = col + dc;
    if (!inBounds(captureRow, captureCol)) continue;
    const Piece target = board_[captureRow][captureCol];
    if (target.color != COLOR_NONE && target.color != piece.color) {
      appendMove(outMoves, moveCount, maxMoves, captureRow, captureCol);
    }
  }

  return moveCount;
}

int ChessGameModel::collectRayMoves(int row, int col, const Piece& piece, const Position* directions, int dirCount,
                                    Position* outMoves, int maxMoves) const {
  int moveCount = 0;
  for (int i = 0; i < dirCount; ++i) {
    int curRow = row + directions[i].row;
    int curCol = col + directions[i].col;
    while (inBounds(curRow, curCol)) {
      const Piece target = board_[curRow][curCol];
      if (target.color == COLOR_NONE) {
        appendMove(outMoves, moveCount, maxMoves, curRow, curCol);
      } else {
        if (target.color != piece.color) {
          appendMove(outMoves, moveCount, maxMoves, curRow, curCol);
        }
        break;
      }
      curRow += directions[i].row;
      curCol += directions[i].col;
    }
  }
  return moveCount;
}

int ChessGameModel::collectStepMoves(int row, int col, const Piece& piece, const Position* deltas, int deltaCount,
                                     Position* outMoves, int maxMoves) const {
  int moveCount = 0;
  for (int i = 0; i < deltaCount; ++i) {
    int newRow = row + deltas[i].row;
    int newCol = col + deltas[i].col;
    if (!inBounds(newRow, newCol)) continue;
    const Piece target = board_[newRow][newCol];
    if (target.color == COLOR_NONE || target.color != piece.color) {
      appendMove(outMoves, moveCount, maxMoves, newRow, newCol);
    }
  }
  return moveCount;
}
