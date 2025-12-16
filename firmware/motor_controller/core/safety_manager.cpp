#include "safety_manager.h"
#include "motor_manager.h"

// Static member initialization
SafetyState SafetyManager::state = SafetyState::NORMAL;
volatile bool SafetyManager::estopTriggered = false;
uint32_t SafetyManager::estopCount = 0;
uint32_t SafetyManager::lastEstopTime = 0;
uint32_t SafetyManager::lastDebounceTime = 0;
SafetyManager::EstopCallback SafetyManager::estopCallback = nullptr;
SafetyManager::EstopCallback SafetyManager::resetCallback = nullptr;

void IRAM_ATTR SafetyManager::estopISR() {
  // Simple debounce in ISR
  uint32_t now = millis();
  if (now - lastDebounceTime > ESTOP_DEBOUNCE_MS) {
    estopTriggered = true;
    lastDebounceTime = now;
  }
}

void SafetyManager::init() {
  // Configure E-stop pin (Boot button on most ESP32 boards)
  pinMode(ESTOP_PIN, INPUT_PULLUP);

  // Attach interrupt for falling edge (button press)
  attachInterrupt(digitalPinToInterrupt(ESTOP_PIN), estopISR, FALLING);

  // Configure status LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  state = SafetyState::NORMAL;
  estopTriggered = false;

  Serial.printf("[SAFETY] Initialized, E-stop on GPIO%d\n", ESTOP_PIN);
}

void SafetyManager::check() {
  // Check for E-stop trigger
  if (estopTriggered) {
    estopTriggered = false;

    if (state != SafetyState::ESTOP_ACTIVE) {
      triggerEstop();
    }
  }

  // Update status LED based on state
  switch (state) {
    case SafetyState::NORMAL:
      digitalWrite(STATUS_LED_PIN, LOW);  // Off
      break;
    case SafetyState::ESTOP_ACTIVE:
      // Blink rapidly
      digitalWrite(STATUS_LED_PIN, (millis() / 100) % 2);
      break;
    case SafetyState::LIMIT_REACHED:
    case SafetyState::OVERCURRENT:
      // Blink slowly
      digitalWrite(STATUS_LED_PIN, (millis() / 500) % 2);
      break;
    case SafetyState::FAULT:
      digitalWrite(STATUS_LED_PIN, HIGH);  // Solid on
      break;
  }
}

void SafetyManager::triggerEstop() {
  state = SafetyState::ESTOP_ACTIVE;
  estopCount++;
  lastEstopTime = millis();

  // Emergency stop all motors immediately
  MotorManager::emergencyStopAll();

  Serial.println("[SAFETY] E-STOP TRIGGERED!");

  // Call user callback if set
  if (estopCallback != nullptr) {
    estopCallback();
  }
}

void SafetyManager::resetEstop() {
  if (state == SafetyState::ESTOP_ACTIVE) {
    // Check if E-stop button is still pressed
    if (digitalRead(ESTOP_PIN) == LOW) {
      Serial.println("[SAFETY] Cannot reset - E-stop button still pressed");
      return;
    }

    state = SafetyState::NORMAL;
    Serial.println("[SAFETY] E-stop reset, system normal");

    // Call reset callback if set
    if (resetCallback != nullptr) {
      resetCallback();
    }
  }
}

bool SafetyManager::isEstopActive() {
  return state == SafetyState::ESTOP_ACTIVE;
}

void SafetyManager::setPositionLimits(uint8_t slot, int32_t min, int32_t max) {
  MotorBase* motor = MotorManager::getMotor(slot);
  if (motor != nullptr) {
    motor->setPositionLimits(min, max);
    Serial.printf("[SAFETY] Slot %d limits set: %ld to %ld\n", slot, min, max);
  }
}

void SafetyManager::clearPositionLimits(uint8_t slot) {
  MotorBase* motor = MotorManager::getMotor(slot);
  if (motor != nullptr) {
    motor->clearPositionLimits();
    Serial.printf("[SAFETY] Slot %d limits cleared\n", slot);
  }
}

bool SafetyManager::checkPositionLimits(uint8_t slot, int32_t position) {
  MotorBase* motor = MotorManager::getMotor(slot);
  if (motor != nullptr) {
    return motor->isWithinLimits(position);
  }
  return true;
}

SafetyState SafetyManager::getState() {
  return state;
}

const char* SafetyManager::getStateString() {
  return getSafetyStateName(state);
}

void SafetyManager::toJson(JsonObject& obj) {
  obj["state"] = static_cast<uint8_t>(state);
  obj["stateName"] = getStateString();
  obj["estopActive"] = isEstopActive();
  obj["estopCount"] = estopCount;
  obj["lastEstopTime"] = lastEstopTime;
  obj["estopPin"] = ESTOP_PIN;
  obj["estopPinState"] = digitalRead(ESTOP_PIN);
}

void SafetyManager::onEstop(EstopCallback callback) {
  estopCallback = callback;
}

void SafetyManager::onReset(EstopCallback callback) {
  resetCallback = callback;
}
