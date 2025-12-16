#include "stepper_28byj48.h"

Stepper28BYJ48::Stepper28BYJ48(uint8_t slot, uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4)
  : MotorBase(slot),
    // HALF4WIRE mode for half-stepping (smoother motion)
    // Pin order: IN1, IN3, IN2, IN4 for correct sequencing
    stepper(AccelStepper::HALF4WIRE, in1, in3, in2, in4) {
  pins[0] = in1;
  pins[1] = in2;
  pins[2] = in3;
  pins[3] = in4;
}

Stepper28BYJ48::~Stepper28BYJ48() {
  emergencyStop();
  // Set all pins LOW to release motor
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW);
  }
}

void Stepper28BYJ48::init() {
  // AccelStepper handles pin modes

  // Set reasonable defaults for 28BYJ-48
  setSpeed(maxSpeedRPM);
  stepper.setAcceleration(acceleration);
  stepper.setCurrentPosition(0);

  enabled = true;

  Serial.printf("[MOTOR] 28BYJ-48 slot %d initialized (pins %d,%d,%d,%d)\n",
                slotId, pins[0], pins[1], pins[2], pins[3]);
}

void Stepper28BYJ48::update() {
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

  stepper.run();
}

void Stepper28BYJ48::stop() {
  stepper.stop();  // Decelerates to stop
}

void Stepper28BYJ48::emergencyStop() {
  stepper.setCurrentPosition(stepper.currentPosition());
  stepper.stop();

  // De-energize all coils to prevent heating
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW);
  }
}

void Stepper28BYJ48::moveTo(int32_t position) {
  if (limitsEnabled) {
    position = clampToLimits(position);
  }
  stepper.moveTo(position);
}

void Stepper28BYJ48::moveRelative(int32_t steps) {
  int32_t target = stepper.currentPosition() + steps;
  if (limitsEnabled) {
    target = clampToLimits(target);
  }
  stepper.moveTo(target);
}

void Stepper28BYJ48::moveRevolutions(float revs) {
  int32_t steps = (int32_t)(revs * STEPS_PER_REV);
  moveRelative(steps);
}

void Stepper28BYJ48::setSpeed(float rpm) {
  // Clamp to reasonable range for 28BYJ-48
  maxSpeedRPM = constrain(rpm, 0.1f, MAX_28BYJ_SPEED);
  float stepsPerSec = rpmToStepsPerSecond(maxSpeedRPM);
  stepper.setMaxSpeed(stepsPerSec);
}

void Stepper28BYJ48::setSpeedSteps(float stepsPerSecond) {
  float maxStepsPerSec = rpmToStepsPerSecond(MAX_28BYJ_SPEED);
  stepsPerSecond = constrain(stepsPerSecond, 0.0f, maxStepsPerSec);
  stepper.setMaxSpeed(stepsPerSecond);
  maxSpeedRPM = stepsPerSecondToRPM(stepsPerSecond);
}

void Stepper28BYJ48::setAcceleration(float stepsPerSecondSquared) {
  acceleration = stepsPerSecondSquared;
  stepper.setAcceleration(acceleration);
}

void Stepper28BYJ48::setCurrentPosition(int32_t position) {
  stepper.setCurrentPosition(position);
}

bool Stepper28BYJ48::isMoving() const {
  return stepper.isRunning();
}

int32_t Stepper28BYJ48::getPosition() const {
  return stepper.currentPosition();
}

float Stepper28BYJ48::getSpeed() const {
  return stepper.speed();
}

float Stepper28BYJ48::getSpeedRPM() const {
  return stepsPerSecondToRPM(stepper.speed());
}

int32_t Stepper28BYJ48::distanceToGo() const {
  return stepper.distanceToGo();
}

int32_t Stepper28BYJ48::getTargetPosition() const {
  return stepper.targetPosition();
}

float Stepper28BYJ48::getRevolutions() const {
  return (float)stepper.currentPosition() / (float)STEPS_PER_REV;
}

void Stepper28BYJ48::toJson(JsonObject& obj) const {
  MotorBase::toJson(obj);
  obj["targetPosition"] = stepper.targetPosition();
  obj["distanceToGo"] = stepper.distanceToGo();
  obj["speedRPM"] = maxSpeedRPM;
  obj["currentSpeedRPM"] = getSpeedRPM();
  obj["acceleration"] = acceleration;
  obj["stepsPerRev"] = STEPS_PER_REV;
  obj["revolutions"] = getRevolutions();
  obj["pins"] = serialized(String("[") + pins[0] + "," + pins[1] + "," + pins[2] + "," + pins[3] + "]");
}

float Stepper28BYJ48::rpmToStepsPerSecond(float rpm) const {
  // RPM to steps/second: (rpm * steps_per_rev) / 60
  return (rpm * STEPS_PER_REV) / 60.0f;
}

float Stepper28BYJ48::stepsPerSecondToRPM(float sps) const {
  // Steps/second to RPM: (sps * 60) / steps_per_rev
  return (sps * 60.0f) / STEPS_PER_REV;
}
