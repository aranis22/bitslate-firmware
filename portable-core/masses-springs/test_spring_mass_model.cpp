#include "SpringMassModel.h"

#include <cassert>
#include <cmath>
#include <iostream>

bool approxEqual(float a, float b, float eps = 0.05f) {
  return std::fabs(a - b) <= eps;
}

void testInitializes() {
  SpringMassModel model;
  assert(approxEqual(model.getMassKg(), 1.0f));
  assert(approxEqual(model.getSpringK(), 80.0f));
  assert(model.isPlaying());
  assert(!model.isDragging());
}

void testMassIncreasesEquilibriumExtension() {
  SpringMassModel model;
  const float base = model.getEquilibriumExtension();
  model.setMassKg(3.0f);
  assert(model.getEquilibriumExtension() > base);
}

void testSpringConstantDecreasesEquilibriumExtension() {
  SpringMassModel model;
  model.setSpringK(40.0f);
  const float loose = model.getEquilibriumExtension();
  model.setSpringK(160.0f);
  const float stiff = model.getEquilibriumExtension();
  assert(stiff < loose);
}

void testResetRestoresDefaults() {
  SpringMassModel model;
  model.setMassKg(4.0f);
  model.setSpringK(120.0f);
  model.startDrag(250.0f);
  model.releaseDrag();
  model.step(0.016f);
  model.reset();
  assert(approxEqual(model.getMassKg(), 1.0f));
  assert(approxEqual(model.getSpringK(), 80.0f));
  assert(approxEqual(model.getDisplacement(), model.getEquilibriumExtension()));
  assert(approxEqual(model.getVelocity(), 0.0f));
}

void testDampingReducesVelocity() {
  SpringMassModel model;
  model.setDamping(8.0f);
  model.startDrag(122.0f + model.getEquilibriumExtension() * 140.0f);
  model.releaseDrag();
  model.step(0.016f);
  const float before = std::fabs(model.getVelocity());
  for (int i = 0; i < 240; ++i) {
    model.step(1.0f / 60.0f);
  }
  assert(std::fabs(model.getVelocity()) < before + 0.5f);
}

void testDraggingChangesDisplacement() {
  SpringMassModel model;
  model.startDrag(34.0f + model.getRestLength() + 120.0f);
  assert(model.isDragging());
  assert(!model.isPlaying());
  assert(model.getDisplacement() > 0.5f);
}

void testReleaseDragExitsDraggingState() {
  SpringMassModel model;
  model.startDrag(260.0f);
  model.releaseDrag();
  assert(!model.isDragging());
  assert(model.isPlaying());
}

void testLowGravityGivesSmallerEquilibriumExtension() {
  SpringMassModel model;
  const float earth = model.getEquilibriumExtension();
  model.setGravity(0.0f);
  assert(model.getEquilibriumExtension() < earth);
  assert(approxEqual(model.getEquilibriumExtension(), 0.0f));
}

void testStepChangesDisplacementWhenPlaying() {
  SpringMassModel model;
  model.startDrag(260.0f);
  model.releaseDrag();
  const float before = model.getDisplacement();
  model.step(1.0f / 60.0f);
  assert(!approxEqual(model.getDisplacement(), before, 0.0001f));
}

void testLargeDtIsClampedAndStable() {
  SpringMassModel model;
  model.startDrag(320.0f);
  model.releaseDrag();
  for (int i = 0; i < 80; ++i) {
    model.step(5.0f);
  }
  assert(model.getDisplacement() >= -0.35f);
  assert(model.getDisplacement() <= 1.25f);
  assert(std::fabs(model.getVelocity()) <= 10.0f);
}

int main() {
  testInitializes();
  testMassIncreasesEquilibriumExtension();
  testSpringConstantDecreasesEquilibriumExtension();
  testResetRestoresDefaults();
  testDampingReducesVelocity();
  testDraggingChangesDisplacement();
  testReleaseDragExitsDraggingState();
  testLowGravityGivesSmallerEquilibriumExtension();
  testStepChangesDisplacementWhenPlaying();
  testLargeDtIsClampedAndStable();

  std::cout << "SpringMassModel C++ tests passed\n";
  return 0;
}
