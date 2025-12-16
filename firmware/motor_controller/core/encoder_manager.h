#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Encoder.h>
#include "../config.h"

// ============================================================================
// Encoder Manager - Hardware PCNT Encoder Interface
// ============================================================================

struct EncoderConfig {
  uint8_t pinA;
  uint8_t pinB;
  bool enabled;
  bool reversed;
  int32_t pulsesPerRevolution;
  uint8_t linkedMotorSlot;  // 255 = not linked
};

class EncoderManager {
public:
  // === Initialization ===
  static void init();
  static void update();  // Called from encoder task

  // === Encoder Configuration ===
  static bool configureEncoder(uint8_t encoderId, uint8_t pinA, uint8_t pinB);
  static bool disableEncoder(uint8_t encoderId);
  static bool linkToMotor(uint8_t encoderId, uint8_t motorSlot);
  static void unlinkFromMotor(uint8_t encoderId);

  // === Reading ===
  static int32_t getCount(uint8_t encoderId);
  static float getRevolutions(uint8_t encoderId);
  static float getRPM(uint8_t encoderId);
  static float getVelocity(uint8_t encoderId);  // counts per second

  // === Control ===
  static void resetCount(uint8_t encoderId);
  static void setCount(uint8_t encoderId, int32_t count);
  static void setReversed(uint8_t encoderId, bool reversed);
  static void setPulsesPerRevolution(uint8_t encoderId, int32_t ppr);

  // === Status ===
  static bool isEnabled(uint8_t encoderId);
  static void toJson(JsonDocument& doc);
  static void encoderToJson(uint8_t encoderId, JsonObject& obj);

private:
  static ESP32Encoder encoders[MAX_ENCODERS];
  static EncoderConfig configs[MAX_ENCODERS];

  // For velocity calculation
  static int32_t lastCounts[MAX_ENCODERS];
  static uint32_t lastUpdateTime[MAX_ENCODERS];
  static float velocities[MAX_ENCODERS];
};
