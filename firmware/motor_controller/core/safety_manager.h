#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "../config.h"

// ============================================================================
// Safety Manager - Emergency Stop and Limit Handling
// ============================================================================

class SafetyManager {
public:
  // === Initialization ===
  static void init();
  static void check();  // Called from main loop

  // === Emergency Stop ===
  static void triggerEstop();
  static void resetEstop();
  static bool isEstopActive();

  // === Position Limits ===
  static void setPositionLimits(uint8_t slot, int32_t min, int32_t max);
  static void clearPositionLimits(uint8_t slot);
  static bool checkPositionLimits(uint8_t slot, int32_t position);

  // === State ===
  static SafetyState getState();
  static const char* getStateString();
  static void toJson(JsonObject& obj);

  // === Callbacks ===
  using EstopCallback = void (*)();
  static void onEstop(EstopCallback callback);
  static void onReset(EstopCallback callback);

  // === Statistics ===
  static uint32_t getEstopCount() { return estopCount; }
  static uint32_t getLastEstopTime() { return lastEstopTime; }

private:
  static SafetyState state;
  static volatile bool estopTriggered;
  static uint32_t estopCount;
  static uint32_t lastEstopTime;
  static uint32_t lastDebounceTime;

  static EstopCallback estopCallback;
  static EstopCallback resetCallback;

  static void IRAM_ATTR estopISR();
};
