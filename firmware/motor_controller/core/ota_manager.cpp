#include "ota_manager.h"
#include "motor_manager.h"
#include "safety_manager.h"
#include <ArduinoJson.h>

// Static member initialization
bool OTAManager::updating = false;
uint8_t OTAManager::progress = 0;
char OTAManager::statusMsg[64] = "Ready";
char OTAManager::lastError[64] = "";
size_t OTAManager::totalSize = 0;
size_t OTAManager::receivedSize = 0;

void OTAManager::init() {
  // Set hostname for OTA
  ArduinoOTA.setHostname(DEFAULT_HOSTNAME);

  // Optional password
  #ifdef OTA_PASSWORD
  if (strlen(OTA_PASSWORD) > 0) {
    ArduinoOTA.setPassword(OTA_PASSWORD);
  }
  #endif

  ArduinoOTA.onStart([]() {
    updating = true;
    progress = 0;
    strcpy(statusMsg, "OTA started");

    // Stop all motors during update
    MotorManager::stopAll();

    String type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
    Serial.printf("[OTA] Start updating %s\n", type.c_str());
  });

  ArduinoOTA.onEnd([]() {
    updating = false;
    progress = 100;
    strcpy(statusMsg, "OTA complete, rebooting");
    Serial.println("\n[OTA] Update complete");
  });

  ArduinoOTA.onProgress([](unsigned int prog, unsigned int total) {
    progress = (prog * 100) / total;
    snprintf(statusMsg, sizeof(statusMsg), "Updating: %u%%", progress);

    // Print progress bar occasionally
    static uint8_t lastPrint = 0;
    if (progress / 10 != lastPrint / 10) {
      Serial.printf("[OTA] Progress: %u%%\n", progress);
      lastPrint = progress;
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    updating = false;
    progress = 0;

    const char* errorMsg;
    switch (error) {
      case OTA_AUTH_ERROR: errorMsg = "Auth Failed"; break;
      case OTA_BEGIN_ERROR: errorMsg = "Begin Failed"; break;
      case OTA_CONNECT_ERROR: errorMsg = "Connect Failed"; break;
      case OTA_RECEIVE_ERROR: errorMsg = "Receive Failed"; break;
      case OTA_END_ERROR: errorMsg = "End Failed"; break;
      default: errorMsg = "Unknown Error"; break;
    }

    snprintf(lastError, sizeof(lastError), "OTA Error: %s", errorMsg);
    strcpy(statusMsg, lastError);
    Serial.printf("[OTA] Error[%u]: %s\n", error, errorMsg);
  });

  ArduinoOTA.begin();
  Serial.println("[OTA] ArduinoOTA initialized");
}

void OTAManager::handle() {
  ArduinoOTA.handle();
}

bool OTAManager::isUpdating() {
  return updating;
}

uint8_t OTAManager::getProgress() {
  return progress;
}

const char* OTAManager::getStatus() {
  return statusMsg;
}

const char* OTAManager::getLastError() {
  return lastError;
}

bool OTAManager::handleFirmwareUpload(uint8_t* data, size_t len, size_t index, size_t total, bool final) {
  // First chunk - initialize update
  if (index == 0) {
    updating = true;
    progress = 0;
    totalSize = total;
    receivedSize = 0;
    lastError[0] = '\0';

    // Stop all motors
    MotorManager::stopAll();

    Serial.printf("[OTA] Web update started, size: %u bytes\n", total);

    if (!Update.begin(total, U_FLASH)) {
      snprintf(lastError, sizeof(lastError), "Begin failed: %s", Update.errorString());
      Serial.printf("[OTA] %s\n", lastError);
      updating = false;
      return false;
    }

    strcpy(statusMsg, "Upload started");
  }

  // Write chunk
  if (Update.write(data, len) != len) {
    snprintf(lastError, sizeof(lastError), "Write failed: %s", Update.errorString());
    Serial.printf("[OTA] %s\n", lastError);
    Update.abort();
    updating = false;
    return false;
  }

  receivedSize += len;
  progress = (receivedSize * 100) / totalSize;
  snprintf(statusMsg, sizeof(statusMsg), "Uploading: %u%%", progress);

  // Final chunk - finish update
  if (final) {
    if (!Update.end(true)) {
      snprintf(lastError, sizeof(lastError), "End failed: %s", Update.errorString());
      Serial.printf("[OTA] %s\n", lastError);
      updating = false;
      return false;
    }

    progress = 100;
    strcpy(statusMsg, "Update complete, rebooting...");
    Serial.println("[OTA] Web update complete, rebooting...");

    // Reboot after short delay
    delay(100);
    ESP.restart();
  }

  return true;
}

void OTAManager::abortUpdate() {
  if (updating) {
    Update.abort();
    updating = false;
    progress = 0;
    strcpy(statusMsg, "Update aborted");
    strcpy(lastError, "User aborted");
    Serial.println("[OTA] Update aborted");
  }
}

void OTAManager::toJson(JsonObject& obj) {
  obj["updating"] = updating;
  obj["progress"] = progress;
  obj["status"] = statusMsg;
  obj["lastError"] = lastError;
  obj["hostname"] = DEFAULT_HOSTNAME;
}
