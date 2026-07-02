#include "TicTacToeModel.h"

TicTacToeModel::TicTacToeModel() {
  reset();
}

void TicTacToeModel::reset() {
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      board[row][col] = EMPTY;
    }
  }
  currentPlayer = X;
  gameState = X_TURN;
}

bool TicTacToeModel::playCell(int row, int col) {
  if (!isInsideBoard(row, col) || isFinished() || board[row][col] != EMPTY) {
    return false;
  }

  board[row][col] = currentPlayer;
  updateGameStateAfterMove();
  return true;
}

TicTacToeModel::Cell TicTacToeModel::getCell(int row, int col) const {
  if (!isInsideBoard(row, col)) {
    return EMPTY;
  }
  return board[row][col];
}

TicTacToeModel::Cell TicTacToeModel::getCurrentPlayer() const {
  return currentPlayer;
}

TicTacToeModel::GameState TicTacToeModel::getGameState() const {
  return gameState;
}

bool TicTacToeModel::isFinished() const {
  return gameState == X_WINS || gameState == O_WINS || gameState == DRAW;
}

const char* TicTacToeModel::getStatusText() const {
  switch (gameState) {
    case X_TURN:
      return "X turn";
    case O_TURN:
      return "O turn";
    case X_WINS:
      return "X wins";
    case O_WINS:
      return "O wins";
    case DRAW:
      return "Draw";
    default:
      return "X turn";
  }
}

bool TicTacToeModel::isInsideBoard(int row, int col) const {
  return row >= 0 && row < 3 && col >= 0 && col < 3;
}

bool TicTacToeModel::hasWin(Cell player) const {
  for (int i = 0; i < 3; ++i) {
    if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
      return true;
    }
    if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
      return true;
    }
  }

  if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
    return true;
  }
  return board[0][2] == player && board[1][1] == player && board[2][0] == player;
}

bool TicTacToeModel::hasDraw() const {
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      if (board[row][col] == EMPTY) {
        return false;
      }
    }
  }
  return true;
}

void TicTacToeModel::updateGameStateAfterMove() {
  if (hasWin(currentPlayer)) {
    gameState = currentPlayer == X ? X_WINS : O_WINS;
    return;
  }

  if (hasDraw()) {
    gameState = DRAW;
    return;
  }

  currentPlayer = currentPlayer == X ? O : X;
  gameState = currentPlayer == X ? X_TURN : O_TURN;
}
