#pragma once

#include "motor_base.h"
#include <AccelStepper.h>

// ============================================================================
// NEMA17 Stepper Driver - A4988/DRV8825 Step/Dir Interface
// ============================================================================

enum class StepperDriver : uint8_t {
  A4988,    // Up to 1/16 microstepping
  DRV8825   // Up to 1/32 microstepping
};

class StepperNema17 : public MotorBase {
public:
  StepperNema17(uint8_t slot, StepperDriver driver, uint8_t stepPin, uint8_t dirPin,
                uint8_t enablePin = 255, uint8_t ms1 = 255, uint8_t ms2 = 255, uint8_t ms3 = 255);
  ~StepperNema17() override;

  // === Core Control ===
  void init() override;
  void update() override;
  void stop() override;
  void emergencyStop() override;

  // === Stepper Specific Control ===
  void moveTo(int32_t position);        // Absolute position
  void moveRelative(int32_t steps);     // Relative move
  void setSpeed(float stepsPerSecond);  // Maximum speed
  void setAcceleration(float stepsPerSecondSquared);
  void runSpeed();                       // Constant speed mode
  void setCurrentPosition(int32_t position);
  void enable();
  void disable();

  // === Microstepping ===
  void setMicrosteps(MicrostepMode mode);
  MicrostepMode getMicrosteps() const { return microstepMode; }
  uint16_t getStepsPerRevolution() const;

  // === Status ===
  bool isMoving() const override;
  bool isEnabled() const override { return driverEnabled; }
  int32_t getPosition() const override;
  float getSpeed() const override;
  float getTargetSpeed() const override { return maxSpeed; }
  int32_t distanceToGo() const;
  int32_t getTargetPosition() const;

  // === Type Info ===
  MotorType getType() const override;
  const char* getTypeName() const override;

  // === JSON ===
  void toJson(JsonObject& obj) const override;

  // === Configuration ===
  StepperDriver getDriverType() const { return driverType; }
  void setStepsPerRevolution(uint16_t steps) { stepsPerRev = steps; }

private:
  AccelStepper stepper;
  StepperDriver driverType;

  uint8_t stepPin, dirPin, enablePin;
  uint8_t ms1Pin, ms2Pin, ms3Pin;

  MicrostepMode microstepMode = MicrostepMode::FULL;
  uint16_t stepsPerRev = NEMA17_STEPS_PER_REV;
  float maxSpeed = DEFAULT_STEPPER_SPEED;
  float acceleration = DEFAULT_STEPPER_ACCEL;
  bool driverEnabled = false;
  bool constantSpeedMode = false;

  void applyMicrosteps();
};
