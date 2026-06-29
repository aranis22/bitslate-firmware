#include "CollisionModel.h"

CollisionModel::CollisionModel() {
  reset();
}

void CollisionModel::reset() {
  balls[0].id = 1;
  balls[0].x = 145.0f;
  balls[0].y = trackY;
  balls[0].radius = 18.0f;
  balls[0].mass = 1.0f;
  balls[0].velocityX = 70.0f;

  balls[1].id = 2;
  balls[1].x = 335.0f;
  balls[1].y = trackY;
  balls[1].radius = 24.0f;
  balls[1].mass = 2.0f;
  balls[1].velocityX = -45.0f;

  lastStepCollision = false;
  lastStepWallBounce = false;
}

void CollisionModel::step(float dt) {
  lastStepCollision = false;
  lastStepWallBounce = false;

  dt = clamp(dt, 0.0f, 1.0f / 20.0f);
  for (Ball& ball : balls) {
    ball.x += ball.velocityX * dt;
  }

  handleWallBounce();
  handleBallCollision();
}

void CollisionModel::setMass(int ballId, float mass) {
  Ball& ball = mutableBall(ballId);
  ball.mass = clamp(mass, 1.0f, 5.0f);
  ball.radius = 14.0f + ball.mass * 4.0f;
  separateBalls();
  keepBallsInside();
}

void CollisionModel::setVelocityX(int ballId, float velocityX) {
  mutableBall(ballId).velocityX = clamp(velocityX, -120.0f, 120.0f);
}

const Ball& CollisionModel::getBall(int ballId) const {
  return balls[ballId == 2 ? 1 : 0];
}

float CollisionModel::totalMomentum() const {
  return balls[0].mass * balls[0].velocityX + balls[1].mass * balls[1].velocityX;
}

float CollisionModel::totalKineticEnergy() const {
  return 0.5f * balls[0].mass * balls[0].velocityX * balls[0].velocityX +
         0.5f * balls[1].mass * balls[1].velocityX * balls[1].velocityX;
}

bool CollisionModel::wasLastStepCollision() const {
  return lastStepCollision;
}

bool CollisionModel::wasLastStepWallBounce() const {
  return lastStepWallBounce;
}

Ball& CollisionModel::mutableBall(int ballId) {
  return balls[ballId == 2 ? 1 : 0];
}

void CollisionModel::handleWallBounce() {
  for (Ball& ball : balls) {
    if (ball.x - ball.radius < leftWall) {
      ball.x = leftWall + ball.radius;
      if (ball.velocityX < 0.0f) {
        ball.velocityX = -ball.velocityX;
        lastStepWallBounce = true;
      }
    } else if (ball.x + ball.radius > rightWall) {
      ball.x = rightWall - ball.radius;
      if (ball.velocityX > 0.0f) {
        ball.velocityX = -ball.velocityX;
        lastStepWallBounce = true;
      }
    }
  }
}

void CollisionModel::handleBallCollision() {
  Ball* left = &balls[0];
  Ball* right = &balls[1];
  if (left->x > right->x) {
    left = &balls[1];
    right = &balls[0];
  }

  const float distance = right->x - left->x;
  const float minDistance = left->radius + right->radius;
  const float relativeVelocity = left->velocityX - right->velocityX;

  if (distance <= minDistance && relativeVelocity > 0.0f) {
    const float v1 = left->velocityX;
    const float v2 = right->velocityX;
    const float m1 = left->mass;
    const float m2 = right->mass;
    const float totalMass = m1 + m2;

    left->velocityX = ((m1 - m2) / totalMass) * v1 + ((2.0f * m2) / totalMass) * v2;
    right->velocityX = ((2.0f * m1) / totalMass) * v1 + ((m2 - m1) / totalMass) * v2;
    separatePair(*left, *right);
    lastStepCollision = true;
  }
}

void CollisionModel::separateBalls() {
  Ball* left = &balls[0];
  Ball* right = &balls[1];
  if (left->x > right->x) {
    left = &balls[1];
    right = &balls[0];
  }
  separatePair(*left, *right);
}

void CollisionModel::keepBallsInside() {
  for (Ball& ball : balls) {
    ball.x = clamp(ball.x, leftWall + ball.radius, rightWall - ball.radius);
  }
}

void CollisionModel::separatePair(Ball& left, Ball& right) {
  const float minDistance = left.radius + right.radius;
  const float overlap = minDistance - (right.x - left.x);
  if (overlap <= 0.0f) {
    return;
  }

  left.x -= overlap / 2.0f + 0.1f;
  right.x += overlap / 2.0f + 0.1f;
  keepBallsInside();
}

float CollisionModel::clamp(float value, float minValue, float maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}
