#pragma once

#include "motor_base.h"

// ============================================================================
// DC Motor Driver - L298N and L9110S H-Bridge Support
// ============================================================================

enum class DCDriverType : uint8_t {
  L298N,   // IN1, IN2 for direction, EN for PWM speed
  L9110S   // IA (PWM), IB (PWM) - both pins are PWM capable
};

class DCMotor : public MotorBase {
public:
  DCMotor(uint8_t slot, DCDriverType driver, uint8_t pinA, uint8_t pinB, uint8_t pinEn = 255);
  ~DCMotor() override;

  // === Core Control ===
  void init() override;
  void update() override;
  void stop() override;
  void emergencyStop() override;

  // === DC Motor Specific Control ===
  void setSpeed(int16_t speed);     // -255 to +255 (negative = reverse)
  void setDirection(bool forward);
  void brake();                      // Active braking
  void coast();                      // Free spinning (no power)

  // === Status ===
  bool isMoving() const override;
  float getSpeed() const override;
  float getTargetSpeed() const override { return static_cast<float>(targetSpeed); }
  int16_t getRawSpeed() const { return currentSpeed; }
  bool isReversed() const { return currentSpeed < 0; }
  bool isBraking() const { return brakeMode; }

  // === Type Info ===
  MotorType getType() const override;
  const char* getTypeName() const override;

  // === JSON ===
  void toJson(JsonObject& obj) const override;

  // === Configuration ===
  void setRampRate(uint8_t rate) { rampRate = rate; }  // Speed change per update
  void setMinSpeed(uint8_t min) { minSpeed = min; }    // Dead band threshold
  DCDriverType getDriverType() const { return driverType; }

private:
  DCDriverType driverType;
  uint8_t pinA, pinB, pinEn;

  int16_t currentSpeed = 0;   // Current actual speed
  int16_t targetSpeed = 0;    // Target speed
  bool brakeMode = false;

  uint8_t rampRate = 10;      // Speed change per update cycle
  uint8_t minSpeed = 0;       // Minimum speed threshold

  uint8_t pwmChannel = 0;     // LEDC channel

  void applySpeed();
  void applyL298N();
  void applyL9110S();
};
