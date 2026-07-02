#pragma once

#include <stdint.h>

namespace bitslate::geo {

enum class QuizTapResult {
  OutsideMap,
  Correct,
  Wrong,
  Done,
};

struct QuizTapResponse {
  QuizTapResult result;
  int clickedStateId;
};

class UsStatesQuizCore {
 public:
  void reset();

  const char* currentPrompt() const;
  int currentTargetStateId() const;
  int stateAtCell(uint16_t x, uint16_t y) const;
  QuizTapResponse handleCellTap(uint16_t x, uint16_t y);
  bool isCompleted(uint8_t stateId) const;
  bool isDone() const;

 private:
  static constexpr uint8_t kQuizTargetCount = 5;

  uint8_t currentIndex_ = 0;
  bool completed_[48] = {};
};

}  // namespace bitslate::geo
