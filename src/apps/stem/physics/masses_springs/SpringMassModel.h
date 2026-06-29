#pragma once

class SpringMassModel {
public:
  SpringMassModel();

  void reset();
  void step(float dt);

  void setMassKg(float value);
  void setSpringK(float value);
  void setDamping(float value);
  void setGravity(float value);

  void startDrag(float screenOrWorldY);
  void dragTo(float screenOrWorldY);
  void releaseDrag();

  float getMassKg() const;
  float getSpringK() const;
  float getDamping() const;
  float getGravity() const;

  float getRestLength() const;
  float getDisplacement() const;
  float getVelocity() const;
  float getMassY() const;

  float getEquilibriumExtension() const;
  float getSpringForce() const;
  float getGravityForce() const;
  float getDampingForce() const;
  float getNetForce() const;

  bool isDragging() const;
  bool isPlaying() const;
  void setPlaying(bool value);

private:
  static constexpr float DEFAULT_MASS_KG = 1.0f;
  static constexpr float DEFAULT_SPRING_K = 80.0f;
  static constexpr float DEFAULT_DAMPING = 1.5f;
  static constexpr float DEFAULT_GRAVITY = 9.8f;
  static constexpr float DEFAULT_REST_LENGTH = 88.0f;
  static constexpr float PIXELS_PER_METER = 140.0f;
  static constexpr float ANCHOR_Y = 34.0f;

  float massKg = DEFAULT_MASS_KG;
  float springK = DEFAULT_SPRING_K;
  float damping = DEFAULT_DAMPING;
  float gravity = DEFAULT_GRAVITY;
  float restLength = DEFAULT_REST_LENGTH;
  float displacement = 0.0f;
  float velocity = 0.0f;
  bool playing = true;
  bool dragging = false;

  static float clamp(float value, float low, float high);
};
