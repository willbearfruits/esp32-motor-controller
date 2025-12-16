#include "dc_motor.h"

// Static counter for PWM channels
static uint8_t nextPwmChannel = 0;

DCMotor::DCMotor(uint8_t slot, DCDriverType driver, uint8_t pA, uint8_t pB, uint8_t pEn)
  : MotorBase(slot), driverType(driver), pinA(pA), pinB(pB), pinEn(pEn) {
  pwmChannel = nextPwmChannel++;
  if (nextPwmChannel > 15) nextPwmChannel = 0; // Wrap around
}

DCMotor::~DCMotor() {
  emergencyStop();
}

void DCMotor::init() {
  if (driverType == DCDriverType::L298N) {
    // L298N: IN1, IN2 are digital, EN is PWM
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);

    if (pinEn != 255) {
      // Use new Arduino-ESP32 3.x API
      ledcAttach(pinEn, PWM_FREQUENCY, PWM_RESOLUTION);
    }
  } else {
    // L9110S: Both pins are PWM (no separate enable)
    ledcAttach(pinA, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttach(pinB, PWM_FREQUENCY, PWM_RESOLUTION);
  }

  enabled = true;
  currentSpeed = 0;
  targetSpeed = 0;
  brakeMode = false;

  Serial.printf("[MOTOR] DC Motor slot %d initialized (%s)\n",
                slotId, driverType == DCDriverType::L298N ? "L298N" : "L9110S");
}

void DCMotor::update() {
  if (!enabled) return;

  // Ramp speed towards target
  if (currentSpeed < targetSpeed) {
    currentSpeed = min(currentSpeed + rampRate, (int)targetSpeed);
  } else if (currentSpeed > targetSpeed) {
    currentSpeed = max(currentSpeed - rampRate, (int)targetSpeed);
  }

  applySpeed();
}

void DCMotor::stop() {
  targetSpeed = 0;
  // Let update() ramp down gradually
}

void DCMotor::emergencyStop() {
  targetSpeed = 0;
  currentSpeed = 0;
  brakeMode = false;

  if (driverType == DCDriverType::L298N) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    if (pinEn != 255) {
      ledcWrite(pinEn, 0);
    }
  } else {
    ledcWrite(pinA, 0);
    ledcWrite(pinB, 0);
  }
}

void DCMotor::setSpeed(int16_t speed) {
  brakeMode = false;
  targetSpeed = constrain(speed, -255, 255);

  // Apply minimum speed threshold (dead band)
  if (abs(targetSpeed) < minSpeed && targetSpeed != 0) {
    targetSpeed = (targetSpeed > 0) ? minSpeed : -minSpeed;
  }
}

void DCMotor::setDirection(bool forward) {
  if (targetSpeed == 0) {
    targetSpeed = forward ? 128 : -128; // Default medium speed
  } else {
    targetSpeed = forward ? abs(targetSpeed) : -abs(targetSpeed);
  }
  brakeMode = false;
}

void DCMotor::brake() {
  brakeMode = true;
  targetSpeed = 0;
  currentSpeed = 0;

  // Active brake: both sides HIGH or both LOW depending on driver
  if (driverType == DCDriverType::L298N) {
    // L298N: IN1=IN2=HIGH for brake
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    if (pinEn != 255) {
      ledcWrite(pinEn, 255);
    }
  } else {
    // L9110S: Both LOW for brake
    ledcWrite(pinA, 0);
    ledcWrite(pinB, 0);
  }
}

void DCMotor::coast() {
  brakeMode = false;
  targetSpeed = 0;
  currentSpeed = 0;

  // Coast: disable all outputs
  if (driverType == DCDriverType::L298N) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    if (pinEn != 255) {
      ledcWrite(pinEn, 0);
    }
  } else {
    ledcWrite(pinA, 0);
    ledcWrite(pinB, 0);
  }
}

bool DCMotor::isMoving() const {
  return currentSpeed != 0;
}

float DCMotor::getSpeed() const {
  return static_cast<float>(currentSpeed);
}

MotorType DCMotor::getType() const {
  return driverType == DCDriverType::L298N ? MotorType::DC_L298N : MotorType::DC_L9110S;
}

const char* DCMotor::getTypeName() const {
  return driverType == DCDriverType::L298N ? "DC (L298N)" : "DC (L9110S)";
}

void DCMotor::toJson(JsonObject& obj) const {
  MotorBase::toJson(obj);
  obj["driverType"] = driverType == DCDriverType::L298N ? "L298N" : "L9110S";
  obj["targetSpeed"] = targetSpeed;
  obj["currentSpeed"] = currentSpeed;
  obj["braking"] = brakeMode;
  obj["direction"] = currentSpeed >= 0 ? "forward" : "reverse";
  obj["pinA"] = pinA;
  obj["pinB"] = pinB;
  obj["pinEn"] = pinEn;
}

void DCMotor::applySpeed() {
  if (brakeMode) return; // Don't override brake mode

  if (driverType == DCDriverType::L298N) {
    applyL298N();
  } else {
    applyL9110S();
  }
}

void DCMotor::applyL298N() {
  uint8_t duty = abs(currentSpeed);

  if (currentSpeed > 0) {
    // Forward
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
  } else if (currentSpeed < 0) {
    // Reverse
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
  } else {
    // Stop
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
  }

  if (pinEn != 255) {
    ledcWrite(pinEn, duty);
  }
}

void DCMotor::applyL9110S() {
  uint8_t duty = abs(currentSpeed);

  if (currentSpeed > 0) {
    // Forward: pinA = PWM, pinB = 0
    ledcWrite(pinA, duty);
    ledcWrite(pinB, 0);
  } else if (currentSpeed < 0) {
    // Reverse: pinA = 0, pinB = PWM
    ledcWrite(pinA, 0);
    ledcWrite(pinB, duty);
  } else {
    // Stop
    ledcWrite(pinA, 0);
    ledcWrite(pinB, 0);
  }
}
