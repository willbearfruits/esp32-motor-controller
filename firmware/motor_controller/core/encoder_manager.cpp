#include "encoder_manager.h"

// Static member initialization
ESP32Encoder EncoderManager::encoders[MAX_ENCODERS];
EncoderConfig EncoderManager::configs[MAX_ENCODERS];
int32_t EncoderManager::lastCounts[MAX_ENCODERS] = {0};
uint32_t EncoderManager::lastUpdateTime[MAX_ENCODERS] = {0};
float EncoderManager::velocities[MAX_ENCODERS] = {0};

void EncoderManager::init() {
  // Enable internal pull-ups for encoder pins
  ESP32Encoder::useInternalWeakPullResistors = UP;

  // Initialize default configurations
  for (uint8_t i = 0; i < MAX_ENCODERS; i++) {
    configs[i].enabled = false;
    configs[i].reversed = false;
    configs[i].pulsesPerRevolution = 400;  // Common encoder value (100 PPR x4 quadrature)
    configs[i].linkedMotorSlot = 255;
    lastCounts[i] = 0;
    lastUpdateTime[i] = millis();
    velocities[i] = 0;
  }

  // Set default pin configurations
  configs[0].pinA = ENC0_PIN_A;
  configs[0].pinB = ENC0_PIN_B;
  configs[1].pinA = ENC1_PIN_A;
  configs[1].pinB = ENC1_PIN_B;

  Serial.println("[ENCODER] Encoder Manager initialized");
}

void EncoderManager::update() {
  uint32_t now = millis();

  for (uint8_t i = 0; i < MAX_ENCODERS; i++) {
    if (!configs[i].enabled) continue;

    int32_t count = encoders[i].getCount();
    uint32_t dt = now - lastUpdateTime[i];

    if (dt > 0) {
      // Calculate velocity (counts per second)
      int32_t dCount = count - lastCounts[i];
      velocities[i] = (float)dCount * 1000.0f / (float)dt;

      lastCounts[i] = count;
      lastUpdateTime[i] = now;
    }
  }
}

bool EncoderManager::configureEncoder(uint8_t encoderId, uint8_t pinA, uint8_t pinB) {
  if (encoderId >= MAX_ENCODERS) return false;

  // Detach if already attached
  if (configs[encoderId].enabled) {
    encoders[encoderId].detach();
  }

  // Store configuration
  configs[encoderId].pinA = pinA;
  configs[encoderId].pinB = pinB;

  // Attach encoder using hardware PCNT
  encoders[encoderId].attachHalfQuad(pinA, pinB);
  encoders[encoderId].clearCount();

  configs[encoderId].enabled = true;
  lastCounts[encoderId] = 0;
  lastUpdateTime[encoderId] = millis();
  velocities[encoderId] = 0;

  Serial.printf("[ENCODER] Encoder %d configured (pins %d, %d)\n", encoderId, pinA, pinB);
  return true;
}

bool EncoderManager::disableEncoder(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS) return false;

  if (configs[encoderId].enabled) {
    encoders[encoderId].detach();
    configs[encoderId].enabled = false;
    velocities[encoderId] = 0;
    Serial.printf("[ENCODER] Encoder %d disabled\n", encoderId);
  }

  return true;
}

bool EncoderManager::linkToMotor(uint8_t encoderId, uint8_t motorSlot) {
  if (encoderId >= MAX_ENCODERS) return false;
  if (motorSlot >= MAX_MOTORS) return false;

  configs[encoderId].linkedMotorSlot = motorSlot;
  Serial.printf("[ENCODER] Encoder %d linked to motor slot %d\n", encoderId, motorSlot);
  return true;
}

void EncoderManager::unlinkFromMotor(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS) return;
  configs[encoderId].linkedMotorSlot = 255;
}

int32_t EncoderManager::getCount(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS || !configs[encoderId].enabled) return 0;

  int32_t count = encoders[encoderId].getCount();
  return configs[encoderId].reversed ? -count : count;
}

float EncoderManager::getRevolutions(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS || !configs[encoderId].enabled) return 0.0f;
  if (configs[encoderId].pulsesPerRevolution == 0) return 0.0f;

  return (float)getCount(encoderId) / (float)configs[encoderId].pulsesPerRevolution;
}

float EncoderManager::getRPM(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS || !configs[encoderId].enabled) return 0.0f;
  if (configs[encoderId].pulsesPerRevolution == 0) return 0.0f;

  // velocity is counts/second, convert to RPM
  float revPerSecond = velocities[encoderId] / (float)configs[encoderId].pulsesPerRevolution;
  return revPerSecond * 60.0f;
}

float EncoderManager::getVelocity(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS || !configs[encoderId].enabled) return 0.0f;
  return configs[encoderId].reversed ? -velocities[encoderId] : velocities[encoderId];
}

void EncoderManager::resetCount(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS || !configs[encoderId].enabled) return;
  encoders[encoderId].clearCount();
  lastCounts[encoderId] = 0;
  velocities[encoderId] = 0;
}

void EncoderManager::setCount(uint8_t encoderId, int32_t count) {
  if (encoderId >= MAX_ENCODERS || !configs[encoderId].enabled) return;
  encoders[encoderId].setCount(configs[encoderId].reversed ? -count : count);
  lastCounts[encoderId] = count;
}

void EncoderManager::setReversed(uint8_t encoderId, bool reversed) {
  if (encoderId >= MAX_ENCODERS) return;
  configs[encoderId].reversed = reversed;
}

void EncoderManager::setPulsesPerRevolution(uint8_t encoderId, int32_t ppr) {
  if (encoderId >= MAX_ENCODERS) return;
  configs[encoderId].pulsesPerRevolution = ppr;
}

bool EncoderManager::isEnabled(uint8_t encoderId) {
  if (encoderId >= MAX_ENCODERS) return false;
  return configs[encoderId].enabled;
}

void EncoderManager::toJson(JsonDocument& doc) {
  JsonArray arr = doc.createNestedArray("encoders");

  for (uint8_t i = 0; i < MAX_ENCODERS; i++) {
    JsonObject obj = arr.createNestedObject();
    encoderToJson(i, obj);
  }
}

void EncoderManager::encoderToJson(uint8_t encoderId, JsonObject& obj) {
  if (encoderId >= MAX_ENCODERS) return;

  obj["id"] = encoderId;
  obj["enabled"] = configs[encoderId].enabled;
  obj["pinA"] = configs[encoderId].pinA;
  obj["pinB"] = configs[encoderId].pinB;
  obj["reversed"] = configs[encoderId].reversed;
  obj["pulsesPerRevolution"] = configs[encoderId].pulsesPerRevolution;
  obj["linkedMotorSlot"] = configs[encoderId].linkedMotorSlot;

  if (configs[encoderId].enabled) {
    obj["count"] = getCount(encoderId);
    obj["revolutions"] = getRevolutions(encoderId);
    obj["rpm"] = getRPM(encoderId);
    obj["velocity"] = getVelocity(encoderId);
  }
}
