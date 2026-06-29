#include "CollisionModel.h"

#include <cassert>
#include <cmath>
#include <iostream>

static bool approxEqual(float a, float b, float eps = 0.05f) {
  return std::fabs(a - b) <= eps;
}

static void stepCollision(CollisionModel& model) {
  model.step(1.0f / 60.0f);
}

static void testEqualMassesSwapVelocities() {
  CollisionModel model;
  model.setMass(1, 1.0f);
  model.setMass(2, 1.0f);
  model.setVelocityX(1, 50.0f);
  model.setVelocityX(2, -30.0f);

  // Drive the balls into overlap through public stepping.
  while (!model.wasLastStepCollision()) {
    stepCollision(model);
  }

  assert(approxEqual(model.getBall(1).velocityX, -30.0f));
  assert(approxEqual(model.getBall(2).velocityX, 50.0f));
}

static void testHeavyBallAffectsLightBallCorrectly() {
  CollisionModel model;
  model.setMass(1, 5.0f);
  model.setMass(2, 1.0f);
  model.setVelocityX(1, 20.0f);
  model.setVelocityX(2, -20.0f);

  while (!model.wasLastStepCollision()) {
    stepCollision(model);
  }

  const float expectedA = ((5.0f - 1.0f) / 6.0f) * 20.0f + ((2.0f * 1.0f) / 6.0f) * -20.0f;
  const float expectedB = ((2.0f * 5.0f) / 6.0f) * 20.0f + ((1.0f - 5.0f) / 6.0f) * -20.0f;
  assert(approxEqual(model.getBall(1).velocityX, expectedA));
  assert(approxEqual(model.getBall(2).velocityX, expectedB));
}

static void testMomentumConserved() {
  CollisionModel model;
  model.setMass(1, 3.0f);
  model.setMass(2, 2.0f);
  model.setVelocityX(1, 60.0f);
  model.setVelocityX(2, -25.0f);

  const float before = model.totalMomentum();
  while (!model.wasLastStepCollision()) {
    stepCollision(model);
  }
  assert(approxEqual(model.totalMomentum(), before));
}

static void testKineticEnergyConserved() {
  CollisionModel model;
  model.setMass(1, 3.0f);
  model.setMass(2, 2.0f);
  model.setVelocityX(1, 60.0f);
  model.setVelocityX(2, -25.0f);

  const float before = model.totalKineticEnergy();
  while (!model.wasLastStepCollision()) {
    stepCollision(model);
  }
  assert(approxEqual(model.totalKineticEnergy(), before));
}

static void testWallBounceFlipsVelocity() {
  CollisionModel model;
  model.setVelocityX(1, -120.0f);

  while (!model.wasLastStepWallBounce()) {
    stepCollision(model);
  }
  assert(model.getBall(1).velocityX > 0.0f);
}

static void testResetRestoresInitialState() {
  CollisionModel model;
  model.setMass(1, 4.0f);
  model.setVelocityX(1, -100.0f);
  model.step(0.05f);
  model.reset();

  assert(approxEqual(model.getBall(1).x, 145.0f));
  assert(approxEqual(model.getBall(1).mass, 1.0f));
  assert(approxEqual(model.getBall(1).velocityX, 70.0f));
  assert(approxEqual(model.getBall(2).x, 335.0f));
  assert(approxEqual(model.getBall(2).mass, 2.0f));
  assert(approxEqual(model.getBall(2).velocityX, -45.0f));
}

static void testOverlappingBallsDoNotStick() {
  CollisionModel model;
  model.setMass(1, 5.0f);
  model.setMass(2, 5.0f);
  model.setVelocityX(1, 80.0f);
  model.setVelocityX(2, -80.0f);

  while (!model.wasLastStepCollision()) {
    stepCollision(model);
  }

  const float v1 = model.getBall(1).velocityX;
  const float v2 = model.getBall(2).velocityX;
  for (int i = 0; i < 20; ++i) {
    stepCollision(model);
  }
  assert(approxEqual(model.getBall(1).velocityX, v1));
  assert(approxEqual(model.getBall(2).velocityX, v2));
}

int main() {
  testEqualMassesSwapVelocities();
  testHeavyBallAffectsLightBallCorrectly();
  testMomentumConserved();
  testKineticEnergyConserved();
  testWallBounceFlipsVelocity();
  testResetRestoresInitialState();
  testOverlappingBallsDoNotStick();
  std::cout << "CollisionModel C++ tests passed\n";
  return 0;
}
