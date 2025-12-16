#pragma once

#include "motor_base.h"
#include <AccelStepper.h>

// ============================================================================
// 28BYJ-48 Stepper Driver - ULN2003 4-Wire Interface
// ============================================================================
// The 28BYJ-48 is a small unipolar stepper with 64:1 gear reduction.
// With half-stepping, it has 2048 steps per revolution of the output shaft.

class Stepper28BYJ48 : public MotorBase {
public:
  Stepper28BYJ48(uint8_t slot, uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);
  ~Stepper28BYJ48() override;

  // === Core Control ===
  void init() override;
  void update() override;
  void stop() override;
  void emergencyStop() override;

  // === Stepper Specific Control ===
  void moveTo(int32_t position);        // Absolute position (in steps)
  void moveRelative(int32_t steps);     // Relative move
  void moveRevolutions(float revs);     // Move by revolutions
  void setSpeed(float rpm);             // Speed in RPM (more intuitive for this motor)
  void setSpeedSteps(float stepsPerSecond);
  void setAcceleration(float stepsPerSecondSquared);
  void setCurrentPosition(int32_t position);

  // === Status ===
  bool isMoving() const override;
  int32_t getPosition() const override;
  float getSpeed() const override;
  float getSpeedRPM() const;
  int32_t distanceToGo() const;
  int32_t getTargetPosition() const;
  float getRevolutions() const;

  // === Type Info ===
  MotorType getType() const override { return MotorType::STEPPER_ULN2003; }
  const char* getTypeName() const override { return "Stepper (28BYJ-48)"; }

  // === JSON ===
  void toJson(JsonObject& obj) const override;

  // === Constants ===
  static constexpr int32_t STEPS_PER_REV = ULN2003_STEPS_PER_REV;  // 2048 with 64:1 gearbox

private:
  mutable AccelStepper stepper;
  uint8_t pins[4];

  float maxSpeedRPM = DEFAULT_28BYJ_SPEED;
  float acceleration = 500;  // steps/sec^2

  float rpmToStepsPerSecond(float rpm) const;
  float stepsPerSecondToRPM(float sps) const;
};
