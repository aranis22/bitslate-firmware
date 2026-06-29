#include "SpringMassModel.h"

SpringMassModel::SpringMassModel() {
  reset();
}

void SpringMassModel::reset() {
  massKg = DEFAULT_MASS_KG;
  springK = DEFAULT_SPRING_K;
  damping = DEFAULT_DAMPING;
  gravity = DEFAULT_GRAVITY;
  restLength = DEFAULT_REST_LENGTH;
  displacement = getEquilibriumExtension();
  velocity = 0.0f;
  playing = true;
  dragging = false;
}

void SpringMassModel::step(float dt) {
  if (!playing || dragging) {
    return;
  }

  dt = clamp(dt, 0.0f, 0.05f);
  const float acceleration = getNetForce() / massKg;
  velocity += acceleration * dt;
  velocity = clamp(velocity, -10.0f, 10.0f);
  displacement += velocity * dt;
  displacement = clamp(displacement, -0.35f, 1.25f);

  if (displacement <= -0.35f && velocity < 0.0f) {
    velocity = 0.0f;
  }
  if (displacement >= 1.25f && velocity > 0.0f) {
    velocity = 0.0f;
  }
}

void SpringMassModel::setMassKg(float value) {
  const float oldEquilibrium = getEquilibriumExtension();
  massKg = clamp(value, 0.1f, 5.0f);
  displacement += getEquilibriumExtension() - oldEquilibrium;
  displacement = clamp(displacement, -0.35f, 1.25f);
}

void SpringMassModel::setSpringK(float value) {
  const float oldEquilibrium = getEquilibriumExtension();
  springK = clamp(value, 1.0f, 200.0f);
  displacement += getEquilibriumExtension() - oldEquilibrium;
  displacement = clamp(displacement, -0.35f, 1.25f);
}

void SpringMassModel::setDamping(float value) {
  damping = clamp(value, 0.0f, 8.0f);
}

void SpringMassModel::setGravity(float value) {
  const float oldEquilibrium = getEquilibriumExtension();
  gravity = clamp(value, 0.0f, 9.8f);
  displacement += getEquilibriumExtension() - oldEquilibrium;
  displacement = clamp(displacement, -0.35f, 1.25f);
}

void SpringMassModel::startDrag(float screenOrWorldY) {
  dragging = true;
  playing = false;
  dragTo(screenOrWorldY);
}

void SpringMassModel::dragTo(float screenOrWorldY) {
  if (!dragging) {
    return;
  }

  const float raw = (screenOrWorldY - ANCHOR_Y - restLength) / PIXELS_PER_METER;
  displacement = clamp(raw, -0.35f, 1.25f);
  velocity = 0.0f;
}

void SpringMassModel::releaseDrag() {
  dragging = false;
  playing = true;
}

float SpringMassModel::getMassKg() const { return massKg; }
float SpringMassModel::getSpringK() const { return springK; }
float SpringMassModel::getDamping() const { return damping; }
float SpringMassModel::getGravity() const { return gravity; }
float SpringMassModel::getRestLength() const { return restLength; }
float SpringMassModel::getDisplacement() const { return displacement; }
float SpringMassModel::getVelocity() const { return velocity; }

float SpringMassModel::getMassY() const {
  return ANCHOR_Y + restLength + displacement * PIXELS_PER_METER;
}

float SpringMassModel::getEquilibriumExtension() const {
  return springK > 0.0f ? (massKg * gravity) / springK : 0.0f;
}

float SpringMassModel::getSpringForce() const { return -springK * displacement; }
float SpringMassModel::getGravityForce() const { return massKg * gravity; }
float SpringMassModel::getDampingForce() const { return -damping * velocity; }

float SpringMassModel::getNetForce() const {
  return getSpringForce() + getGravityForce() + getDampingForce();
}

bool SpringMassModel::isDragging() const { return dragging; }
bool SpringMassModel::isPlaying() const { return playing; }

void SpringMassModel::setPlaying(bool value) {
  if (!dragging) {
    playing = value;
  }
}

float SpringMassModel::clamp(float value, float low, float high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}
