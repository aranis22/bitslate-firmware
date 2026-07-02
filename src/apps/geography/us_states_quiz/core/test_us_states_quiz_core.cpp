// Native build, when g++ is available:
// g++ -std=c++17 ^
//   src/apps/geography/us_states_quiz/generated/UsStatesVectorData.cpp ^
//   src/apps/geography/us_states_quiz/core/UsStatesQuizCore.cpp ^
//   src/apps/geography/us_states_quiz/core/test_us_states_quiz_core.cpp ^
//   -o src/apps/geography/us_states_quiz/core/test_us_states_quiz_core.exe

#include "UsStatesQuizCore.h"

#include "../generated/UsStatesVectorData.h"

#include <cstdio>

using bitslate::geo::QuizTapResult;
using bitslate::geo::UsStatesQuizCore;

namespace {

const char* const QUIZ_ORDER[] = {
    "Washington",
    "California",
    "Texas",
    "South Dakota",
    "Missouri",
};

bool expect(bool value, const char* message) {
  if (!value) {
    std::printf("FAIL: %s\n", message);
    return false;
  }
  return true;
}

bool findCellForState(int stateId, uint16_t& outX, uint16_t& outY) {
  for (uint16_t y = 0; y < bitslate::geo::US_GRID_H; ++y) {
    for (uint16_t x = 0; x < bitslate::geo::US_GRID_W; ++x) {
      if (bitslate::geo::stateAtCell(x, y) == stateId) {
        outX = x;
        outY = y;
        return true;
      }
    }
  }
  return false;
}

}  // namespace

int main() {
  bool ok = true;
  ok &= expect(bitslate::geo::US_STATE_COUNT == 48, "exports 48 contiguous states");

  for (const char* stateName : QUIZ_ORDER) {
    const int stateId = bitslate::geo::findStateByName(stateName);
    ok &= expect(stateId >= 0, "quiz state resolves by name");
    uint16_t x = 0;
    uint16_t y = 0;
    ok &= expect(findCellForState(stateId, x, y), "quiz state has at least one grid cell");
  }

  UsStatesQuizCore quiz;
  quiz.reset();
  ok &= expect(!quiz.isDone(), "quiz starts unfinished");
  ok &= expect(quiz.handleCellTap(0, 0).result == QuizTapResult::OutsideMap, "outside map tap is outside");

  const int californiaId = bitslate::geo::findStateByName("California");
  uint16_t wrongX = 0;
  uint16_t wrongY = 0;
  ok &= expect(findCellForState(californiaId, wrongX, wrongY), "wrong-state cell found");
  ok &= expect(quiz.handleCellTap(wrongX, wrongY).result == QuizTapResult::Wrong, "wrong tap does not advance");
  ok &= expect(quiz.currentTargetStateId() == bitslate::geo::findStateByName("Washington"), "wrong tap keeps prompt");

  for (const char* stateName : QUIZ_ORDER) {
    const int targetId = bitslate::geo::findStateByName(stateName);
    uint16_t x = 0;
    uint16_t y = 0;
    ok &= expect(findCellForState(targetId, x, y), "target cell found");
    const auto response = quiz.handleCellTap(x, y);
    ok &= expect(
        response.result == QuizTapResult::Correct || response.result == QuizTapResult::Done,
        "target tap is correct or done"
    );
    ok &= expect(quiz.isCompleted(static_cast<uint8_t>(targetId)), "target marked completed");
  }

  ok &= expect(quiz.isDone(), "quiz done after five targets");
  ok &= expect(quiz.handleCellTap(0, 0).result == QuizTapResult::Done, "done remains done");

  std::printf(ok ? "PASS\n" : "FAIL\n");
  return ok ? 0 : 1;
}
