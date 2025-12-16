#include "servo_motor.h"

ServoMotor::ServoMotor(uint8_t slot, uint8_t p, uint16_t minP, uint16_t maxP)
  : MotorBase(slot), pin(p), minPulse(minP), maxPulse(maxP) {
}

ServoMotor::~ServoMotor() {
  detach();
}

void ServoMotor::init() {
  // ESP32Servo library handles timer allocation
  servo.attach(pin, minPulse, maxPulse);
  servo.write(90);  // Center position
  currentAngle = 90;
  targetAngle = 90;
  enabled = true;

  Serial.printf("[MOTOR] Servo slot %d initialized on pin %d\n", slotId, pin);
}

void ServoMotor::update() {
  if (!enabled || !servo.attached()) return;

  if (smoothMode) {
    currentAngle = calculateSmoothAngle();
    servo.write(currentAngle);

    // Check if movement complete
    if (currentAngle == targetAngle) {
      smoothMode = false;
    }
  }
}

void ServoMotor::stop() {
  // Stop any smooth movement
  smoothMode = false;
  targetAngle = currentAngle;
}

void ServoMotor::emergencyStop() {
  smoothMode = false;
  targetAngle = currentAngle;
  // Servo stays at current position
}

void ServoMotor::setAngle(uint8_t angle) {
  angle = constrain(angle, 0, 180);

  // Check position limits if enabled
  if (limitsEnabled) {
    angle = constrain(angle, (uint8_t)posMin, (uint8_t)posMax);
  }

  smoothMode = false;
  currentAngle = angle;
  targetAngle = angle;

  if (servo.attached()) {
    servo.write(angle);
  }
}

void ServoMotor::setAngleSmooth(uint8_t angle, uint16_t durationMs) {
  angle = constrain(angle, 0, 180);

  // Check position limits if enabled
  if (limitsEnabled) {
    angle = constrain(angle, (uint8_t)posMin, (uint8_t)posMax);
  }

  if (durationMs == 0) {
    // Instant movement
    setAngle(angle);
    return;
  }

  targetAngle = angle;
  sweepStartAngle = currentAngle;
  sweepStartTime = millis();
  sweepDuration = durationMs;
  smoothMode = true;
}

void ServoMotor::setPulseWidth(uint16_t microseconds) {
  microseconds = constrain(microseconds, minPulse, maxPulse);

  if (servo.attached()) {
    servo.writeMicroseconds(microseconds);
  }

  // Update angle estimate
  currentAngle = map(microseconds, minPulse, maxPulse, 0, 180);
  targetAngle = currentAngle;
  smoothMode = false;
}

void ServoMotor::detach() {
  if (servo.attached()) {
    servo.detach();
    enabled = false;
    Serial.printf("[MOTOR] Servo slot %d detached\n", slotId);
  }
}

void ServoMotor::attach() {
  if (!servo.attached()) {
    servo.attach(pin, minPulse, maxPulse);
    servo.write(currentAngle);
    enabled = true;
    Serial.printf("[MOTOR] Servo slot %d attached\n", slotId);
  }
}

bool ServoMotor::isMoving() const {
  return smoothMode && currentAngle != targetAngle;
}

float ServoMotor::getSpeed() const {
  if (!smoothMode || sweepDuration == 0) return 0.0f;

  // Calculate current speed in degrees per second
  int angleDiff = abs((int)targetAngle - (int)sweepStartAngle);
  return (float)angleDiff * 1000.0f / (float)sweepDuration;
}

void ServoMotor::toJson(JsonObject& obj) const {
  MotorBase::toJson(obj);
  obj["currentAngle"] = currentAngle;
  obj["targetAngle"] = targetAngle;
  obj["minPulse"] = minPulse;
  obj["maxPulse"] = maxPulse;
  obj["attached"] = servo.attached();
  obj["smoothMode"] = smoothMode;
  obj["pin"] = pin;
}

uint8_t ServoMotor::calculateSmoothAngle() {
  if (!smoothMode) return currentAngle;

  uint32_t elapsed = millis() - sweepStartTime;

  if (elapsed >= sweepDuration) {
    // Movement complete
    return targetAngle;
  }

  // Linear interpolation
  float progress = (float)elapsed / (float)sweepDuration;
  int angleDiff = (int)targetAngle - (int)sweepStartAngle;

  return sweepStartAngle + (int)(angleDiff * progress);
}
