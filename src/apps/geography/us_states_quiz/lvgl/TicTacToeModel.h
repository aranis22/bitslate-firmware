#pragma once

class TicTacToeModel {
public:
  enum Cell {
    EMPTY = 0,
    X,
    O,
  };

  enum GameState {
    X_TURN,
    O_TURN,
    X_WINS,
    O_WINS,
    DRAW,
  };

  TicTacToeModel();

  void reset();
  bool playCell(int row, int col);

  Cell getCell(int row, int col) const;
  Cell getCurrentPlayer() const;
  GameState getGameState() const;
  bool isFinished() const;
  const char* getStatusText() const;

private:
  Cell board[3][3];
  Cell currentPlayer;
  GameState gameState;

  bool isInsideBoard(int row, int col) const;
  bool hasWin(Cell player) const;
  bool hasDraw() const;
  void updateGameStateAfterMove();
};
