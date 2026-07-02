#pragma once

#include <stdint.h>

class UsStatesQuizModel {
public:
  enum TapResult {
    TAP_NONE,
    TAP_OUTSIDE,
    TAP_CORRECT,
    TAP_WRONG,
    TAP_DONE,
  };

  enum Status {
    STATUS_PROMPT,
    STATUS_CORRECT,
    STATUS_TRY_AGAIN,
    STATUS_DONE,
    STATUS_OUTSIDE_MAP,
  };

  UsStatesQuizModel();

  void reset();
  const char* getCurrentTargetName() const;
  int getCurrentTargetIndex() const;
  int getStateAtCell(uint8_t x, uint8_t y) const;
  TapResult handleCellTap(uint8_t x, uint8_t y);
  bool isCompleted(uint8_t stateIndex) const;
  Status getStatus() const;
  const char* getStatusText() const;
  bool isDone() const;

private:
  static constexpr uint8_t PROMPT_COUNT = 5;

  uint8_t currentPrompt;
  bool completed[PROMPT_COUNT];
  Status status;

  int promptStateIndex(uint8_t promptIndex) const;
  void advancePastCompleted();
};
