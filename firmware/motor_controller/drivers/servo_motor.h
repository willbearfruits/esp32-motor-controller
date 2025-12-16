#pragma once

#include "motor_base.h"
#include <ESP32Servo.h>

// ============================================================================
// Servo Motor Driver - Standard PWM Servo Control
// ============================================================================

class ServoMotor : public MotorBase {
public:
  ServoMotor(uint8_t slot, uint8_t pin, uint16_t minPulse = SERVO_MIN_PULSE,
             uint16_t maxPulse = SERVO_MAX_PULSE);
  ~ServoMotor() override;

  // === Core Control ===
  void init() override;
  void update() override;
  void stop() override;
  void emergencyStop() override;

  // === Servo Specific Control ===
  void setAngle(uint8_t angle);                           // 0-180 degrees, instant
  void setAngleSmooth(uint8_t angle, uint16_t durationMs); // Smooth transition
  void setPulseWidth(uint16_t microseconds);              // Direct pulse control
  void detach();                                          // Release servo
  void attach();                                          // Re-attach servo

  // === Status ===
  bool isMoving() const override;
  int32_t getPosition() const override { return currentAngle; }
  float getSpeed() const override;
  uint8_t getCurrentAngle() const { return currentAngle; }
  uint8_t getTargetAngle() const { return targetAngle; }
  bool isAttached() const { return servo.attached(); }

  // === Type Info ===
  MotorType getType() const override { return MotorType::SERVO; }
  const char* getTypeName() const override { return "Servo"; }

  // === JSON ===
  void toJson(JsonObject& obj) const override;

  // === Configuration ===
  void setMinPulse(uint16_t us) { minPulse = us; }
  void setMaxPulse(uint16_t us) { maxPulse = us; }
  void setSpeed(uint8_t degreesPerSecond) { sweepSpeed = degreesPerSecond; }

private:
  Servo servo;
  uint8_t pin;
  uint16_t minPulse, maxPulse;

  uint8_t currentAngle = 90;
  uint8_t targetAngle = 90;

  // Smooth movement
  bool smoothMode = false;
  uint32_t sweepStartTime = 0;
  uint16_t sweepDuration = 0;
  uint8_t sweepStartAngle = 0;
  uint8_t sweepSpeed = 90;  // degrees per second for default smooth movement

  uint8_t calculateSmoothAngle();
};
