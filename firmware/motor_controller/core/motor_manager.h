#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "../config.h"
#include "../drivers/motor_base.h"
#include "../drivers/dc_motor.h"
#include "../drivers/servo_motor.h"
#include "../drivers/stepper_nema17.h"
#include "../drivers/stepper_28byj48.h"

// ============================================================================
// Motor Manager - Centralized Motor Slot Management
// ============================================================================
// Manages 4 motor slots with runtime motor type configuration.
// Uses factory pattern to create appropriate driver instances.
// Thread-safe access via mutex for FreeRTOS compatibility.

class MotorManager {
public:
  // === Initialization ===
  static void init();

  // === Slot Management ===
  static bool configureSlot(uint8_t slot, MotorType type, const JsonObject& config);
  static bool configureSlot(uint8_t slot, MotorType type, const SlotPins& pins);
  static bool removeMotor(uint8_t slot);

  // === Motor Access ===
  static MotorBase* getMotor(uint8_t slot);
  static MotorType getMotorType(uint8_t slot);
  static bool isSlotConfigured(uint8_t slot);

  // === Batch Operations ===
  static void stopAll();
  static void emergencyStopAll();
  static void updateAll();  // Called from motor task

  // === Control Commands ===
  static bool sendCommand(uint8_t slot, CommandType cmd, int32_t value = 0, uint16_t duration = 0);

  // === Status ===
  static void toJson(JsonDocument& doc);
  static void slotToJson(uint8_t slot, JsonObject& obj);
  static uint8_t getConfiguredCount();

  // === Configuration Persistence ===
  static void saveConfig();
  static void loadConfig();

private:
  static MotorBase* motors[MAX_MOTORS];
  static MotorType motorTypes[MAX_MOTORS];
  static SlotPins slotPins[MAX_MOTORS];
  static SemaphoreHandle_t mutex;

  static MotorBase* createMotor(uint8_t slot, MotorType type, const SlotPins& pins);
  static void destroyMotor(uint8_t slot);
};
