#pragma once

struct CollisionBall {
  int id = 0;
  float x = 0.0f;
  float y = 0.0f;
  float radius = 0.0f;
  float mass = 1.0f;
  float velocityX = 0.0f;
};

class CollisionModel {
public:
  CollisionModel();

  void reset();
  void step(float dt);

  void setMass(int ballId, float mass);
  void setVelocityX(int ballId, float velocityX);

  const CollisionBall& getBall(int ballId) const;

  float totalMomentum() const;
  float totalKineticEnergy() const;

  bool wasLastStepCollision() const;
  bool wasLastStepWallBounce() const;

private:
  CollisionBall balls[2] = {};
  float leftWall = 28.0f;
  float rightWall = 452.0f;
  float trackY = 118.0f;
  bool lastStepCollision = false;
  bool lastStepWallBounce = false;

  CollisionBall& mutableBall(int ballId);
  void handleWallBounce();
  void handleBallCollision();
  void separateBalls();
  void keepBallsInside();
  void separatePair(CollisionBall& left, CollisionBall& right);
  static float clamp(float value, float minValue, float maxValue);
};
