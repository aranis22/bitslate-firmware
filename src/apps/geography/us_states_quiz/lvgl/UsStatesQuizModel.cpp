#include "UsStatesQuizModel.h"

#include "GeneratedGeographyData.h"

namespace {

constexpr const char* PROMPT_ORDER[] = {
    "Washington",
    "California",
    "Texas",
    "South Dakota",
    "Missouri",
};

int stateIndexByName(const char* name) {
  for (uint8_t index = 0; index < GEO_STATE_COUNT; ++index) {
    const char* candidate = GEO_STATE_MASKS[index].name;
    const char* left = name;
    const char* right = candidate;
    while (*left != '\0' && *right != '\0' && *left == *right) {
      ++left;
      ++right;
    }
    if (*left == '\0' && *right == '\0') {
      return index;
    }
  }
  return -1;
}

}  // namespace

UsStatesQuizModel::UsStatesQuizModel() {
  reset();
}

void UsStatesQuizModel::reset() {
  currentPrompt = 0;
  for (uint8_t index = 0; index < PROMPT_COUNT; ++index) {
    completed[index] = false;
  }
  status = STATUS_PROMPT;
}

const char* UsStatesQuizModel::getCurrentTargetName() const {
  if (currentPrompt >= PROMPT_COUNT) {
    return "";
  }
  return PROMPT_ORDER[currentPrompt];
}

int UsStatesQuizModel::getCurrentTargetIndex() const {
  return promptStateIndex(currentPrompt);
}

int UsStatesQuizModel::getStateAtCell(uint8_t x, uint8_t y) const {
  return findGeoStateAtCell(x, y);
}

UsStatesQuizModel::TapResult UsStatesQuizModel::handleCellTap(uint8_t x, uint8_t y) {
  if (isDone()) {
    status = STATUS_DONE;
    return TAP_DONE;
  }

  const int tappedState = getStateAtCell(x, y);
  if (tappedState < 0) {
    status = STATUS_OUTSIDE_MAP;
    return TAP_OUTSIDE;
  }

  const int targetState = getCurrentTargetIndex();
  if (tappedState != targetState) {
    status = STATUS_TRY_AGAIN;
    return TAP_WRONG;
  }

  completed[currentPrompt] = true;
  ++currentPrompt;
  advancePastCompleted();
  status = isDone() ? STATUS_DONE : STATUS_CORRECT;
  return isDone() ? TAP_DONE : TAP_CORRECT;
}

bool UsStatesQuizModel::isCompleted(uint8_t stateIndex) const {
  for (uint8_t promptIndex = 0; promptIndex < PROMPT_COUNT; ++promptIndex) {
    if (completed[promptIndex] && promptStateIndex(promptIndex) == stateIndex) {
      return true;
    }
  }
  return false;
}

UsStatesQuizModel::Status UsStatesQuizModel::getStatus() const {
  return status;
}

const char* UsStatesQuizModel::getStatusText() const {
  switch (status) {
    case STATUS_CORRECT:
      return "Correct";
    case STATUS_TRY_AGAIN:
      return "Try again";
    case STATUS_DONE:
      return "Done";
    case STATUS_OUTSIDE_MAP:
      return "Outside map";
    case STATUS_PROMPT:
    default:
      return "";
  }
}

bool UsStatesQuizModel::isDone() const {
  return currentPrompt >= PROMPT_COUNT;
}

int UsStatesQuizModel::promptStateIndex(uint8_t promptIndex) const {
  if (promptIndex >= PROMPT_COUNT) {
    return -1;
  }
  return stateIndexByName(PROMPT_ORDER[promptIndex]);
}

void UsStatesQuizModel::advancePastCompleted() {
  while (currentPrompt < PROMPT_COUNT && completed[currentPrompt]) {
    ++currentPrompt;
  }
}
