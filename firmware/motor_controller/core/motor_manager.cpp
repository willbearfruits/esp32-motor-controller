#include "motor_manager.h"
#include <LittleFS.h>
#include <Preferences.h>

// Static member initialization
MotorBase* MotorManager::motors[MAX_MOTORS] = {nullptr};
MotorType MotorManager::motorTypes[MAX_MOTORS] = {MotorType::NONE};
SlotPins MotorManager::slotPins[MAX_MOTORS];
SemaphoreHandle_t MotorManager::mutex = nullptr;

void MotorManager::init() {
  // Create mutex for thread safety
  mutex = xSemaphoreCreateMutex();

  // Initialize slot pins to defaults
  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    slotPins[i] = getDefaultSlotPins(i);
    motors[i] = nullptr;
    motorTypes[i] = MotorType::NONE;
  }

  // Load saved configuration
  loadConfig();

  Serial.println("[MOTOR] Motor Manager initialized");
}

bool MotorManager::configureSlot(uint8_t slot, MotorType type, const JsonObject& config) {
  if (slot >= MAX_MOTORS) return false;

  SlotPins pins = slotPins[slot];

  // Parse pin configuration from JSON if provided
  if (config.containsKey("pinA")) pins.pinA = config["pinA"];
  if (config.containsKey("pinB")) pins.pinB = config["pinB"];
  if (config.containsKey("pinEn")) pins.pinEn = config["pinEn"];
  if (config.containsKey("pinEx")) pins.pinEx = config["pinEx"];

  // For step/dir drivers
  if (config.containsKey("stepPin")) pins.pinA = config["stepPin"];
  if (config.containsKey("dirPin")) pins.pinB = config["dirPin"];
  if (config.containsKey("enablePin")) pins.pinEn = config["enablePin"];

  // For servo
  if (config.containsKey("pin")) pins.pinA = config["pin"];

  // For ULN2003
  if (config.containsKey("in1")) pins.pinA = config["in1"];
  if (config.containsKey("in2")) pins.pinB = config["in2"];
  if (config.containsKey("in3")) pins.pinEn = config["in3"];
  if (config.containsKey("in4")) pins.pinEx = config["in4"];

  return configureSlot(slot, type, pins);
}

bool MotorManager::configureSlot(uint8_t slot, MotorType type, const SlotPins& pins) {
  if (slot >= MAX_MOTORS) return false;

  xSemaphoreTake(mutex, portMAX_DELAY);

  // Remove existing motor if any
  destroyMotor(slot);

  if (type == MotorType::NONE) {
    slotPins[slot] = pins;
    motorTypes[slot] = MotorType::NONE;
    xSemaphoreGive(mutex);
    Serial.printf("[MOTOR] Slot %d cleared\n", slot);
    return true;
  }

  // Create new motor
  MotorBase* motor = createMotor(slot, type, pins);
  if (motor == nullptr) {
    xSemaphoreGive(mutex);
    Serial.printf("[MOTOR] Failed to create motor for slot %d\n", slot);
    return false;
  }

  motors[slot] = motor;
  motorTypes[slot] = type;
  slotPins[slot] = pins;

  // Initialize the motor
  motor->init();

  xSemaphoreGive(mutex);

  Serial.printf("[MOTOR] Slot %d configured as %s\n", slot, getMotorTypeName(type));
  return true;
}

bool MotorManager::removeMotor(uint8_t slot) {
  if (slot >= MAX_MOTORS) return false;

  xSemaphoreTake(mutex, portMAX_DELAY);
  destroyMotor(slot);
  motorTypes[slot] = MotorType::NONE;
  xSemaphoreGive(mutex);

  return true;
}

MotorBase* MotorManager::getMotor(uint8_t slot) {
  if (slot >= MAX_MOTORS) return nullptr;
  return motors[slot];
}

MotorType MotorManager::getMotorType(uint8_t slot) {
  if (slot >= MAX_MOTORS) return MotorType::NONE;
  return motorTypes[slot];
}

bool MotorManager::isSlotConfigured(uint8_t slot) {
  if (slot >= MAX_MOTORS) return false;
  return motors[slot] != nullptr;
}

void MotorManager::stopAll() {
  xSemaphoreTake(mutex, portMAX_DELAY);
  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    if (motors[i] != nullptr) {
      motors[i]->stop();
    }
  }
  xSemaphoreGive(mutex);
  Serial.println("[MOTOR] All motors stopped");
}

void MotorManager::emergencyStopAll() {
  // Don't wait for mutex in emergency - just stop
  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    if (motors[i] != nullptr) {
      motors[i]->emergencyStop();
    }
  }
  Serial.println("[MOTOR] EMERGENCY STOP - All motors");
}

void MotorManager::updateAll() {
  // Called from motor task - should be fast
  if (xSemaphoreTake(mutex, pdMS_TO_TICKS(1)) == pdTRUE) {
    for (uint8_t i = 0; i < MAX_MOTORS; i++) {
      if (motors[i] != nullptr) {
        motors[i]->update();
      }
    }
    xSemaphoreGive(mutex);
  }
}

bool MotorManager::sendCommand(uint8_t slot, CommandType cmd, int32_t value, uint16_t duration) {
  if (slot >= MAX_MOTORS || motors[slot] == nullptr) return false;

  xSemaphoreTake(mutex, portMAX_DELAY);
  MotorBase* motor = motors[slot];
  MotorType type = motorTypes[slot];

  switch (cmd) {
    case CommandType::STOP:
      motor->stop();
      break;

    case CommandType::SET_SPEED:
      if (type == MotorType::DC_L298N || type == MotorType::DC_L9110S) {
        static_cast<DCMotor*>(motor)->setSpeed(value);
      } else if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825) {
        static_cast<StepperNema17*>(motor)->setSpeed(value);
      } else if (type == MotorType::STEPPER_ULN2003) {
        static_cast<Stepper28BYJ48*>(motor)->setSpeedSteps(value);
      }
      break;

    case CommandType::SET_POSITION:
      if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825) {
        static_cast<StepperNema17*>(motor)->moveTo(value);
      } else if (type == MotorType::STEPPER_ULN2003) {
        static_cast<Stepper28BYJ48*>(motor)->moveTo(value);
      }
      break;

    case CommandType::SET_ANGLE:
      if (type == MotorType::SERVO) {
        if (duration > 0) {
          static_cast<ServoMotor*>(motor)->setAngleSmooth(value, duration);
        } else {
          static_cast<ServoMotor*>(motor)->setAngle(value);
        }
      }
      break;

    case CommandType::MOVE_RELATIVE:
      if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825) {
        static_cast<StepperNema17*>(motor)->moveRelative(value);
      } else if (type == MotorType::STEPPER_ULN2003) {
        static_cast<Stepper28BYJ48*>(motor)->moveRelative(value);
      }
      break;

    case CommandType::BRAKE:
      if (type == MotorType::DC_L298N || type == MotorType::DC_L9110S) {
        static_cast<DCMotor*>(motor)->brake();
      }
      break;

    case CommandType::COAST:
      if (type == MotorType::DC_L298N || type == MotorType::DC_L9110S) {
        static_cast<DCMotor*>(motor)->coast();
      }
      break;

    case CommandType::ENABLE:
      if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825) {
        static_cast<StepperNema17*>(motor)->enable();
      } else if (type == MotorType::SERVO) {
        static_cast<ServoMotor*>(motor)->attach();
      }
      break;

    case CommandType::DISABLE:
      if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825) {
        static_cast<StepperNema17*>(motor)->disable();
      } else if (type == MotorType::SERVO) {
        static_cast<ServoMotor*>(motor)->detach();
      }
      break;

    case CommandType::HOME:
      if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825) {
        static_cast<StepperNema17*>(motor)->moveTo(0);
      } else if (type == MotorType::STEPPER_ULN2003) {
        static_cast<Stepper28BYJ48*>(motor)->moveTo(0);
      } else if (type == MotorType::SERVO) {
        static_cast<ServoMotor*>(motor)->setAngle(90);
      }
      break;

    default:
      xSemaphoreGive(mutex);
      return false;
  }

  xSemaphoreGive(mutex);
  return true;
}

void MotorManager::toJson(JsonDocument& doc) {
  JsonArray slots = doc.createNestedArray("slots");

  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    JsonObject slotObj = slots.createNestedObject();
    slotToJson(i, slotObj);
  }

  doc["configuredCount"] = getConfiguredCount();
}

void MotorManager::slotToJson(uint8_t slot, JsonObject& obj) {
  if (slot >= MAX_MOTORS) return;

  obj["slot"] = slot;
  obj["configured"] = motors[slot] != nullptr;
  obj["type"] = static_cast<uint8_t>(motorTypes[slot]);
  obj["typeName"] = getMotorTypeName(motorTypes[slot]);

  // Add pin configuration
  JsonObject pins = obj.createNestedObject("pins");
  pins["pinA"] = slotPins[slot].pinA;
  pins["pinB"] = slotPins[slot].pinB;
  pins["pinEn"] = slotPins[slot].pinEn;
  pins["pinEx"] = slotPins[slot].pinEx;

  // Add motor-specific data if configured
  if (motors[slot] != nullptr) {
    motors[slot]->toJson(obj);
  }
}

uint8_t MotorManager::getConfiguredCount() {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    if (motors[i] != nullptr) count++;
  }
  return count;
}

void MotorManager::saveConfig() {
  Preferences prefs;
  prefs.begin("motors", false);

  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    char key[16];

    snprintf(key, sizeof(key), "type%d", i);
    prefs.putUChar(key, static_cast<uint8_t>(motorTypes[i]));

    snprintf(key, sizeof(key), "pinA%d", i);
    prefs.putUChar(key, slotPins[i].pinA);

    snprintf(key, sizeof(key), "pinB%d", i);
    prefs.putUChar(key, slotPins[i].pinB);

    snprintf(key, sizeof(key), "pinEn%d", i);
    prefs.putUChar(key, slotPins[i].pinEn);

    snprintf(key, sizeof(key), "pinEx%d", i);
    prefs.putUChar(key, slotPins[i].pinEx);
  }

  prefs.end();
  Serial.println("[MOTOR] Configuration saved");
}

void MotorManager::loadConfig() {
  Preferences prefs;
  prefs.begin("motors", true);  // Read-only

  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    char key[16];

    snprintf(key, sizeof(key), "type%d", i);
    MotorType type = static_cast<MotorType>(prefs.getUChar(key, 0));

    snprintf(key, sizeof(key), "pinA%d", i);
    slotPins[i].pinA = prefs.getUChar(key, getDefaultSlotPins(i).pinA);

    snprintf(key, sizeof(key), "pinB%d", i);
    slotPins[i].pinB = prefs.getUChar(key, getDefaultSlotPins(i).pinB);

    snprintf(key, sizeof(key), "pinEn%d", i);
    slotPins[i].pinEn = prefs.getUChar(key, getDefaultSlotPins(i).pinEn);

    snprintf(key, sizeof(key), "pinEx%d", i);
    slotPins[i].pinEx = prefs.getUChar(key, getDefaultSlotPins(i).pinEx);

    // Restore motor configuration
    if (type != MotorType::NONE) {
      configureSlot(i, type, slotPins[i]);
    }
  }

  prefs.end();
  Serial.println("[MOTOR] Configuration loaded");
}

MotorBase* MotorManager::createMotor(uint8_t slot, MotorType type, const SlotPins& pins) {
  switch (type) {
    case MotorType::DC_L298N:
      return new DCMotor(slot, DCDriverType::L298N, pins.pinA, pins.pinB, pins.pinEn);

    case MotorType::DC_L9110S:
      return new DCMotor(slot, DCDriverType::L9110S, pins.pinA, pins.pinB);

    case MotorType::SERVO:
      return new ServoMotor(slot, pins.pinA);

    case MotorType::STEPPER_A4988:
      return new StepperNema17(slot, StepperDriver::A4988, pins.pinA, pins.pinB, pins.pinEn);

    case MotorType::STEPPER_DRV8825:
      return new StepperNema17(slot, StepperDriver::DRV8825, pins.pinA, pins.pinB, pins.pinEn);

    case MotorType::STEPPER_ULN2003:
      return new Stepper28BYJ48(slot, pins.pinA, pins.pinB, pins.pinEn, pins.pinEx);

    default:
      return nullptr;
  }
}

void MotorManager::destroyMotor(uint8_t slot) {
  if (motors[slot] != nullptr) {
    motors[slot]->emergencyStop();
    delete motors[slot];
    motors[slot] = nullptr;
  }
}
