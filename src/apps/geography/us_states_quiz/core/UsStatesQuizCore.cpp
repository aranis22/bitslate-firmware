#include "UsStatesQuizCore.h"

#include "../generated/UsStatesVectorData.h"

namespace bitslate::geo {
namespace {

const char* const QUIZ_ORDER[] = {
    "Washington",
    "California",
    "Texas",
    "South Dakota",
    "Missouri",
};

}  // namespace

void UsStatesQuizCore::reset() {
  currentIndex_ = 0;
  for (bool& completed : completed_) {
    completed = false;
  }
}

const char* UsStatesQuizCore::currentPrompt() const {
  if (isDone()) {
    return "Done";
  }
  return QUIZ_ORDER[currentIndex_];
}

int UsStatesQuizCore::currentTargetStateId() const {
  if (isDone()) {
    return -1;
  }
  return findStateByName(QUIZ_ORDER[currentIndex_]);
}

int UsStatesQuizCore::stateAtCell(uint16_t x, uint16_t y) const {
  return bitslate::geo::stateAtCell(x, y);
}

QuizTapResponse UsStatesQuizCore::handleCellTap(uint16_t x, uint16_t y) {
  if (isDone()) {
    return {QuizTapResult::Done, -1};
  }

  const int clickedStateId = stateAtCell(x, y);
  if (clickedStateId < 0) {
    return {QuizTapResult::OutsideMap, -1};
  }

  const int targetStateId = currentTargetStateId();
  if (clickedStateId == targetStateId) {
    completed_[clickedStateId] = true;
    ++currentIndex_;
    return {isDone() ? QuizTapResult::Done : QuizTapResult::Correct, clickedStateId};
  }

  return {QuizTapResult::Wrong, clickedStateId};
}

bool UsStatesQuizCore::isCompleted(uint8_t stateId) const {
  if (stateId >= US_STATE_COUNT) {
    return false;
  }
  return completed_[stateId];
}

bool UsStatesQuizCore::isDone() const {
  return currentIndex_ >= kQuizTargetCount;
}

}  // namespace bitslate::geo
