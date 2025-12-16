#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Update.h>
#include "../config.h"

// ============================================================================
// OTA Manager - Over-The-Air Firmware Updates
// ============================================================================

class OTAManager {
public:
  // === Initialization ===
  static void init();
  static void handle();

  // === Status ===
  static bool isUpdating();
  static uint8_t getProgress();
  static const char* getStatus();
  static const char* getLastError();

  // === Web-based OTA ===
  static bool handleFirmwareUpload(uint8_t* data, size_t len, size_t index, size_t total, bool final);
  static void abortUpdate();

  // === JSON ===
  static void toJson(JsonObject& obj);

private:
  static bool updating;
  static uint8_t progress;
  static char statusMsg[64];
  static char lastError[64];
  static size_t totalSize;
  static size_t receivedSize;
};
