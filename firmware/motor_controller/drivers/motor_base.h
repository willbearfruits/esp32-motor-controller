#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "../config.h"

// ============================================================================
// Abstract Base Class for All Motor Types
// ============================================================================
// All motor drivers inherit from this class, providing a uniform interface
// for the MotorManager to control any motor type polymorphically.

class MotorBase {
public:
  MotorBase(uint8_t slot) : slotId(slot) {}
  virtual ~MotorBase() = default;

  // === Core Control Methods (must implement) ===
  virtual void init() = 0;
  virtual void update() = 0;      // Called from motor task at high frequency
  virtual void stop() = 0;        // Graceful stop
  virtual void emergencyStop() = 0; // Immediate stop, ignore limits

  // === Status Methods ===
  virtual bool isMoving() const = 0;
  virtual bool isEnabled() const { return enabled; }

  // Position (steppers/encoders)
  virtual int32_t getPosition() const { return 0; }
  virtual void setPosition(int32_t pos) {}

  // Speed
  virtual float getSpeed() const { return 0.0f; }
  virtual float getTargetSpeed() const { return 0.0f; }

  // === Type Information ===
  virtual MotorType getType() const = 0;
  virtual const char* getTypeName() const = 0;

  // === JSON Serialization ===
  virtual void toJson(JsonObject& obj) const {
    obj["slot"] = slotId;
    obj["type"] = static_cast<uint8_t>(getType());
    obj["typeName"] = getTypeName();
    obj["enabled"] = enabled;
    obj["moving"] = isMoving();
    obj["position"] = getPosition();
    obj["speed"] = getSpeed();
    obj["error"] = errorMessage;
  }

  // === Safety Limits ===
  void setPositionLimits(int32_t min, int32_t max) {
    posMin = min;
    posMax = max;
    limitsEnabled = true;
  }

  void clearPositionLimits() {
    limitsEnabled = false;
    posMin = INT32_MIN;
    posMax = INT32_MAX;
  }

  bool areLimitsEnabled() const { return limitsEnabled; }
  int32_t getMinPosition() const { return posMin; }
  int32_t getMaxPosition() const { return posMax; }

  void setCurrentLimit(float amps) { currentLimit = amps; }
  float getCurrentLimit() const { return currentLimit; }

  // Check if position is within limits
  bool isWithinLimits(int32_t position) const {
    if (!limitsEnabled) return true;
    return position >= posMin && position <= posMax;
  }

  // Clamp position to limits
  int32_t clampToLimits(int32_t position) const {
    if (!limitsEnabled) return position;
    return constrain(position, posMin, posMax);
  }

  // === Error Handling ===
  bool hasError() const { return errorMessage[0] != '\0'; }
  const char* getError() const { return errorMessage; }
  void clearError() { errorMessage[0] = '\0'; }

  // === Slot Information ===
  uint8_t getSlot() const { return slotId; }

protected:
  uint8_t slotId;
  bool enabled = false;
  bool limitsEnabled = false;
  int32_t posMin = INT32_MIN;
  int32_t posMax = INT32_MAX;
  float currentLimit = DEFAULT_CURRENT_LIMIT;
  char errorMessage[64] = {0};

  void setError(const char* msg) {
    strncpy(errorMessage, msg, sizeof(errorMessage) - 1);
    errorMessage[sizeof(errorMessage) - 1] = '\0';
  }
};
