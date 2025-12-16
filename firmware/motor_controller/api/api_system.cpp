#include "api_system.h"
#include "api_server.h"
#include "../core/ota_manager.h"
#include "../core/safety_manager.h"
#include "../core/motor_manager.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <Preferences.h>

static WebServer* webServer = nullptr;

void ApiSystem::registerRoutes(WebServer& server) {
  webServer = &server;

  server.on("/api/system/info", HTTP_GET, handleGetInfo);
  server.on("/api/system/reboot", HTTP_POST, handleReboot);
  server.on("/api/system/factory-reset", HTTP_POST, handleFactoryReset);
  server.on("/api/system/estop", HTTP_POST, handleEstop);
  server.on("/api/system/estop/reset", HTTP_POST, handleEstopReset);
  server.on("/api/system/wifi", HTTP_GET, handleGetWifi);
  server.on("/api/system/wifi", HTTP_POST, handleSetWifi);

  // OTA upload endpoint with upload handler
  server.on("/api/system/ota", HTTP_POST, []() {
    ApiServer::sendSuccess(OTAManager::getStatus());
  }, []() {
    HTTPUpload& upload = webServer->upload();

    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("[OTA] Upload start: %s\n", upload.filename.c_str());
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      OTAManager::handleFirmwareUpload(
        upload.buf,
        upload.currentSize,
        upload.totalSize - upload.currentSize,
        upload.totalSize,
        false
      );
    } else if (upload.status == UPLOAD_FILE_END) {
      OTAManager::handleFirmwareUpload(
        nullptr, 0,
        upload.totalSize,
        upload.totalSize,
        true
      );
    }
  });

  Serial.println("[API] System routes registered");
}

void ApiSystem::handleGetInfo() {
  JsonDocument doc;

  // Firmware info
  doc["firmware"] = FIRMWARE_NAME;
  doc["version"] = FIRMWARE_VERSION;

  // Chip info
  doc["chip"] = ESP.getChipModel();
  doc["cores"] = ESP.getChipCores();
  doc["cpuFreq"] = ESP.getCpuFreqMHz();
  doc["revision"] = ESP.getChipRevision();

  // Memory
  doc["heapFree"] = ESP.getFreeHeap();
  doc["heapTotal"] = ESP.getHeapSize();
  doc["heapMin"] = ESP.getMinFreeHeap();

  // Flash
  doc["flashSize"] = ESP.getFlashChipSize();
  doc["flashSpeed"] = ESP.getFlashChipSpeed();

  // Runtime
  doc["uptime"] = millis() / 1000;
  doc["uptimeStr"] = String(millis() / 3600000) + "h " +
                     String((millis() / 60000) % 60) + "m " +
                     String((millis() / 1000) % 60) + "s";

  // OTA status
  JsonObject otaObj = doc.createNestedObject("ota");
  OTAManager::toJson(otaObj);

  // Safety status
  JsonObject safetyObj = doc.createNestedObject("safety");
  SafetyManager::toJson(safetyObj);

  // Network
  JsonObject netObj = doc.createNestedObject("network");
  netObj["mode"] = WiFi.getMode() == WIFI_AP ? "AP" : "STA";
  netObj["ip"] = WiFi.getMode() == WIFI_AP ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
  netObj["hostname"] = DEFAULT_HOSTNAME;

  // Motors
  doc["configuredMotors"] = MotorManager::getConfiguredCount();

  ApiServer::sendJson(200, doc);
}

void ApiSystem::handleReboot() {
  ApiServer::sendSuccess("Rebooting...");
  delay(100);
  ESP.restart();
}

void ApiSystem::handleFactoryReset() {
  // Stop all motors
  MotorManager::emergencyStopAll();

  // Clear preferences
  Preferences prefs;
  prefs.begin("motors", false);
  prefs.clear();
  prefs.end();

  prefs.begin("wifi", false);
  prefs.clear();
  prefs.end();

  // Clear presets directory
  File dir = LittleFS.open("/presets");
  if (dir && dir.isDirectory()) {
    File file = dir.openNextFile();
    while (file) {
      String path = String("/presets/") + file.name();
      file.close();
      LittleFS.remove(path);
      file = dir.openNextFile();
    }
  }

  ApiServer::sendSuccess("Factory reset complete, rebooting...");
  delay(100);
  ESP.restart();
}

void ApiSystem::handleEstop() {
  SafetyManager::triggerEstop();
  ApiServer::sendSuccess("Emergency stop triggered");
}

void ApiSystem::handleEstopReset() {
  SafetyManager::resetEstop();

  if (SafetyManager::isEstopActive()) {
    ApiServer::sendError(400, "Cannot reset - E-stop still active");
  } else {
    ApiServer::sendSuccess("E-stop reset");
  }
}

void ApiSystem::handleGetWifi() {
  JsonDocument doc;

  doc["mode"] = WiFi.getMode() == WIFI_AP ? "AP" : "STA";

  if (WiFi.getMode() == WIFI_AP) {
    doc["ssid"] = WiFi.softAPSSID();
    doc["ip"] = WiFi.softAPIP().toString();
    doc["stations"] = WiFi.softAPgetStationNum();
    doc["channel"] = WiFi.channel();
  } else {
    doc["ssid"] = WiFi.SSID();
    doc["ip"] = WiFi.localIP().toString();
    doc["gateway"] = WiFi.gatewayIP().toString();
    doc["subnet"] = WiFi.subnetMask().toString();
    doc["dns"] = WiFi.dnsIP().toString();
    doc["rssi"] = WiFi.RSSI();
    doc["connected"] = WiFi.isConnected();
  }

  doc["mac"] = WiFi.macAddress();
  doc["hostname"] = DEFAULT_HOSTNAME;

  ApiServer::sendJson(200, doc);
}

void ApiSystem::handleSetWifi() {
  JsonDocument doc;
  if (!ApiServer::parseJson(doc)) {
    ApiServer::sendError(400, "Invalid JSON");
    return;
  }

  const char* ssid = doc["ssid"] | "";
  const char* password = doc["password"] | "";
  bool apMode = doc["apMode"] | false;

  if (strlen(ssid) == 0) {
    ApiServer::sendError(400, "SSID required");
    return;
  }

  // Save to preferences
  Preferences prefs;
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("password", password);
  prefs.putBool("apMode", apMode);
  prefs.end();

  ApiServer::sendSuccess("WiFi configuration saved, rebooting...");
  delay(100);
  ESP.restart();
}
