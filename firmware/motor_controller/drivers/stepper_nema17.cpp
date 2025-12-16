#include "stepper_nema17.h"

StepperNema17::StepperNema17(uint8_t slot, StepperDriver driver, uint8_t step, uint8_t dir,
                             uint8_t en, uint8_t m1, uint8_t m2, uint8_t m3)
  : MotorBase(slot),
    stepper(AccelStepper::DRIVER, step, dir),
    driverType(driver),
    stepPin(step), dirPin(dir), enablePin(en),
    ms1Pin(m1), ms2Pin(m2), ms3Pin(m3) {
}

StepperNema17::~StepperNema17() {
  emergencyStop();
  disable();
}

void StepperNema17::init() {
  // Configure enable pin
  if (enablePin != 255) {
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, HIGH);  // Active LOW - disabled
    driverEnabled = false;
  } else {
    driverEnabled = true;  // No enable pin = always enabled
  }

  // Configure microstep pins
  if (ms1Pin != 255) pinMode(ms1Pin, OUTPUT);
  if (ms2Pin != 255) pinMode(ms2Pin, OUTPUT);
  if (ms3Pin != 255) pinMode(ms3Pin, OUTPUT);

  // Set default microstepping
  applyMicrosteps();

  // Configure AccelStepper
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(acceleration);
  stepper.setCurrentPosition(0);

  enabled = true;

  Serial.printf("[MOTOR] Stepper NEMA17 slot %d initialized (%s)\n",
                slotId, driverType == StepperDriver::A4988 ? "A4988" : "DRV8825");
}

void StepperNema17::update() {
  if (!enabled) return;

  // Check position limits before stepping
  if (limitsEnabled && stepper.distanceToGo() != 0) {
    int32_t nextPos = stepper.currentPosition();
    if (stepper.distanceToGo() > 0) nextPos++;
    else if (stepper.distanceToGo() < 0) nextPos--;

    if (!isWithinLimits(nextPos)) {
      stepper.stop();
      setError("Position limit reached");
      return;
    }
  }

  if (constantSpeedMode) {
    stepper.runSpeed();
  } else {
    stepper.run();
  }
}

void StepperNema17::stop() {
  constantSpeedMode = false;
  stepper.stop();  // Decelerates to stop
}

void StepperNema17::emergencyStop() {
  constantSpeedMode = false;
  stepper.setCurrentPosition(stepper.currentPosition());  // Immediate stop
  stepper.stop();
}

void StepperNema17::moveTo(int32_t position) {
  // Check limits
  if (limitsEnabled) {
    position = clampToLimits(position);
  }

  constantSpeedMode = false;
  stepper.moveTo(position);
}

void StepperNema17::moveRelative(int32_t steps) {
  int32_t target = stepper.currentPosition() + steps;

  // Check limits
  if (limitsEnabled) {
    target = clampToLimits(target);
  }

  constantSpeedMode = false;
  stepper.moveTo(target);
}

void StepperNema17::setSpeed(float stepsPerSecond) {
  maxSpeed = constrain(stepsPerSecond, 0.0f, MAX_STEPPER_SPEED);
  stepper.setMaxSpeed(maxSpeed);
  stepper.setSpeed(maxSpeed);  // For runSpeed mode
}

void StepperNema17::setAcceleration(float stepsPerSecondSquared) {
  acceleration = stepsPerSecondSquared;
  stepper.setAcceleration(acceleration);
}

void StepperNema17::runSpeed() {
  // Constant speed mode (no acceleration)
  constantSpeedMode = true;
}

void StepperNema17::setCurrentPosition(int32_t position) {
  stepper.setCurrentPosition(position);
}

void StepperNema17::enable() {
  if (enablePin != 255) {
    digitalWrite(enablePin, LOW);  // Active LOW
    driverEnabled = true;
    Serial.printf("[MOTOR] Stepper slot %d enabled\n", slotId);
  }
}

void StepperNema17::disable() {
  if (enablePin != 255) {
    digitalWrite(enablePin, HIGH);  // Active LOW - disabled
    driverEnabled = false;
    Serial.printf("[MOTOR] Stepper slot %d disabled\n", slotId);
  }
}

void StepperNema17::setMicrosteps(MicrostepMode mode) {
  // Validate mode for driver type
  if (driverType == StepperDriver::A4988 && mode == MicrostepMode::THIRTYSECOND) {
    mode = MicrostepMode::SIXTEENTH;  // A4988 max is 1/16
  }

  microstepMode = mode;
  applyMicrosteps();
}

uint16_t StepperNema17::getStepsPerRevolution() const {
  return stepsPerRev * static_cast<uint8_t>(microstepMode);
}

bool StepperNema17::isMoving() const {
  return stepper.isRunning();
}

int32_t StepperNema17::getPosition() const {
  return stepper.currentPosition();
}

float StepperNema17::getSpeed() const {
  return stepper.speed();
}

int32_t StepperNema17::distanceToGo() const {
  return stepper.distanceToGo();
}

int32_t StepperNema17::getTargetPosition() const {
  return stepper.targetPosition();
}

MotorType StepperNema17::getType() const {
  return driverType == StepperDriver::A4988 ? MotorType::STEPPER_A4988 : MotorType::STEPPER_DRV8825;
}

const char* StepperNema17::getTypeName() const {
  return driverType == StepperDriver::A4988 ? "Stepper (A4988)" : "Stepper (DRV8825)";
}

void StepperNema17::toJson(JsonObject& obj) const {
  MotorBase::toJson(obj);
  obj["driverType"] = driverType == StepperDriver::A4988 ? "A4988" : "DRV8825";
  obj["targetPosition"] = stepper.targetPosition();
  obj["distanceToGo"] = stepper.distanceToGo();
  obj["maxSpeed"] = maxSpeed;
  obj["acceleration"] = acceleration;
  obj["microsteps"] = static_cast<uint8_t>(microstepMode);
  obj["stepsPerRev"] = getStepsPerRevolution();
  obj["driverEnabled"] = driverEnabled;
  obj["constantSpeedMode"] = constantSpeedMode;
  obj["stepPin"] = stepPin;
  obj["dirPin"] = dirPin;
  obj["enablePin"] = enablePin;
}

void StepperNema17::applyMicrosteps() {
  // Skip if no microstep pins configured
  if (ms1Pin == 255 && ms2Pin == 255 && ms3Pin == 255) return;

  bool m1 = false, m2 = false, m3 = false;

  // A4988 and DRV8825 share same truth table for MS1-MS3
  switch (microstepMode) {
    case MicrostepMode::FULL:       // 000
      m1 = false; m2 = false; m3 = false;
      break;
    case MicrostepMode::HALF:       // 100
      m1 = true;  m2 = false; m3 = false;
      break;
    case MicrostepMode::QUARTER:    // 010
      m1 = false; m2 = true;  m3 = false;
      break;
    case MicrostepMode::EIGHTH:     // 110
      m1 = true;  m2 = true;  m3 = false;
      break;
    case MicrostepMode::SIXTEENTH:  // 111
      m1 = true;  m2 = true;  m3 = true;
      break;
    case MicrostepMode::THIRTYSECOND: // DRV8825 only: different pattern
      // DRV8825: MS3=HIGH for 1/32 (with MS1=MS2=LOW typically)
      m1 = false; m2 = false; m3 = true;
      break;
  }

  if (ms1Pin != 255) digitalWrite(ms1Pin, m1);
  if (ms2Pin != 255) digitalWrite(ms2Pin, m2);
  if (ms3Pin != 255) digitalWrite(ms3Pin, m3);

  Serial.printf("[MOTOR] Stepper slot %d microstep set to 1/%d\n",
                slotId, static_cast<uint8_t>(microstepMode));
}
