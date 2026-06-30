#pragma once

class ChessGameModel {
public:
  enum PieceColor {
    COLOR_NONE = 0,
    COLOR_WHITE,
    COLOR_BLACK
  };

  enum PieceType {
    TYPE_NONE = 0,
    TYPE_PAWN,
    TYPE_ROOK,
    TYPE_KNIGHT,
    TYPE_BISHOP,
    TYPE_QUEEN,
    TYPE_KING
  };

  struct Piece {
    PieceColor color;
    PieceType type;
  };

  struct Position {
    int row;
    int col;
  };

  ChessGameModel();

  void reset();
  Piece getPiece(int row, int col) const;
  PieceColor getCurrentTurn() const;
  const char* getStatusText() const;

  bool isCurrentPlayersPiece(int row, int col) const;
  int legalMoves(int row, int col, Position* outMoves, int maxMoves) const;
  bool isLegalMove(int fromRow, int fromCol, int toRow, int toCol) const;
  bool movePiece(int fromRow, int fromCol, int toRow, int toCol);

private:
  Piece board_[8][8];
  PieceColor currentTurn_;
  char statusText_[80];

  bool inBounds(int row, int col) const;
  void setStatus(const char* text);
  void appendMove(Position* outMoves, int& moveCount, int maxMoves, int row, int col) const;
  int collectLegalMoves(int row, int col, const Piece& piece, Position* outMoves, int maxMoves) const;
  int collectPawnMoves(int row, int col, const Piece& piece, Position* outMoves, int maxMoves) const;
  int collectRayMoves(int row, int col, const Piece& piece, const Position* directions, int dirCount,
                      Position* outMoves, int maxMoves) const;
  int collectStepMoves(int row, int col, const Piece& piece, const Position* deltas, int deltaCount,
                       Position* outMoves, int maxMoves) const;
};
