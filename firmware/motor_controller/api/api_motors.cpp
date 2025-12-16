#include "api_motors.h"
#include "api_server.h"
#include "../core/motor_manager.h"
#include "../core/safety_manager.h"
#include "../core/encoder_manager.h"

static WebServer* webServer = nullptr;

void ApiMotors::registerRoutes(WebServer& server) {
  webServer = &server;

  server.on("/api/motors", HTTP_GET, handleGetMotors);
  server.on("/api/status", HTTP_GET, handleGetStatus);
  server.on("/api/motors/stop-all", HTTP_POST, handleStopAll);
  server.on("/api/motors/save-config", HTTP_POST, handleSaveConfig);

  // Routes with slot parameter
  server.on("/api/motors/0", HTTP_GET, handleGetMotor);
  server.on("/api/motors/1", HTTP_GET, handleGetMotor);
  server.on("/api/motors/2", HTTP_GET, handleGetMotor);
  server.on("/api/motors/3", HTTP_GET, handleGetMotor);

  server.on("/api/motors/0/configure", HTTP_POST, handleConfigureMotor);
  server.on("/api/motors/1/configure", HTTP_POST, handleConfigureMotor);
  server.on("/api/motors/2/configure", HTTP_POST, handleConfigureMotor);
  server.on("/api/motors/3/configure", HTTP_POST, handleConfigureMotor);

  server.on("/api/motors/0/control", HTTP_POST, handleControlMotor);
  server.on("/api/motors/1/control", HTTP_POST, handleControlMotor);
  server.on("/api/motors/2/control", HTTP_POST, handleControlMotor);
  server.on("/api/motors/3/control", HTTP_POST, handleControlMotor);

  server.on("/api/motors/0/remove", HTTP_POST, handleRemoveMotor);
  server.on("/api/motors/1/remove", HTTP_POST, handleRemoveMotor);
  server.on("/api/motors/2/remove", HTTP_POST, handleRemoveMotor);
  server.on("/api/motors/3/remove", HTTP_POST, handleRemoveMotor);

  Serial.println("[API] Motor routes registered");
}

// Extract slot number from URI like "/api/motors/2/control"
static int getSlotFromUri() {
  String uri = webServer->uri();
  int slotStart = uri.indexOf("/api/motors/") + 12;
  if (slotStart < 12) return -1;

  char slotChar = uri.charAt(slotStart);
  if (slotChar >= '0' && slotChar <= '3') {
    return slotChar - '0';
  }
  return -1;
}

void ApiMotors::handleGetMotors() {
  JsonDocument doc;
  MotorManager::toJson(doc);
  ApiServer::sendJson(200, doc);
}

void ApiMotors::handleGetMotor() {
  int slot = getSlotFromUri();
  if (slot < 0 || slot >= MAX_MOTORS) {
    ApiServer::sendError(400, "Invalid slot");
    return;
  }

  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  MotorManager::slotToJson(slot, obj);
  ApiServer::sendJson(200, doc);
}

void ApiMotors::handleConfigureMotor() {
  int slot = getSlotFromUri();
  if (slot < 0 || slot >= MAX_MOTORS) {
    ApiServer::sendError(400, "Invalid slot");
    return;
  }

  JsonDocument doc;
  if (!ApiServer::parseJson(doc)) {
    ApiServer::sendError(400, "Invalid JSON");
    return;
  }

  // Get motor type
  uint8_t typeVal = doc["type"] | 0;
  MotorType type = static_cast<MotorType>(typeVal);

  if (type == MotorType::NONE || typeVal > 6) {
    ApiServer::sendError(400, "Invalid motor type");
    return;
  }

  // Get pin configuration
  JsonObject pinsObj = doc["pins"];

  if (MotorManager::configureSlot(slot, type, pinsObj)) {
    JsonDocument response;
    response["success"] = true;
    response["message"] = "Motor configured";
    response["slot"] = slot;
    response["type"] = typeVal;
    response["typeName"] = getMotorTypeName(type);
    ApiServer::sendJson(200, response);
  } else {
    ApiServer::sendError(500, "Failed to configure motor");
  }
}

void ApiMotors::handleControlMotor() {
  int slot = getSlotFromUri();
  if (slot < 0 || slot >= MAX_MOTORS) {
    ApiServer::sendError(400, "Invalid slot");
    return;
  }

  if (!MotorManager::isSlotConfigured(slot)) {
    ApiServer::sendError(400, "Slot not configured");
    return;
  }

  // Check for E-stop
  if (SafetyManager::isEstopActive()) {
    ApiServer::sendError(403, "E-stop active");
    return;
  }

  JsonDocument doc;
  if (!ApiServer::parseJson(doc)) {
    ApiServer::sendError(400, "Invalid JSON");
    return;
  }

  // Parse command
  String cmdStr = doc["command"] | "";
  int32_t value = doc["value"] | 0;
  uint16_t duration = doc["duration"] | 0;

  CommandType cmd;
  if (cmdStr == "stop") cmd = CommandType::STOP;
  else if (cmdStr == "speed") cmd = CommandType::SET_SPEED;
  else if (cmdStr == "position") cmd = CommandType::SET_POSITION;
  else if (cmdStr == "angle") cmd = CommandType::SET_ANGLE;
  else if (cmdStr == "relative") cmd = CommandType::MOVE_RELATIVE;
  else if (cmdStr == "brake") cmd = CommandType::BRAKE;
  else if (cmdStr == "coast") cmd = CommandType::COAST;
  else if (cmdStr == "enable") cmd = CommandType::ENABLE;
  else if (cmdStr == "disable") cmd = CommandType::DISABLE;
  else if (cmdStr == "home") cmd = CommandType::HOME;
  else {
    ApiServer::sendError(400, "Invalid command");
    return;
  }

  if (MotorManager::sendCommand(slot, cmd, value, duration)) {
    ApiServer::sendSuccess("Command sent");
  } else {
    ApiServer::sendError(500, "Command failed");
  }
}

void ApiMotors::handleRemoveMotor() {
  int slot = getSlotFromUri();
  if (slot < 0 || slot >= MAX_MOTORS) {
    ApiServer::sendError(400, "Invalid slot");
    return;
  }

  if (MotorManager::removeMotor(slot)) {
    ApiServer::sendSuccess("Motor removed");
  } else {
    ApiServer::sendError(500, "Failed to remove motor");
  }
}

void ApiMotors::handleStopAll() {
  MotorManager::emergencyStopAll();
  ApiServer::sendSuccess("All motors stopped");
}

void ApiMotors::handleSaveConfig() {
  MotorManager::saveConfig();
  ApiServer::sendSuccess("Configuration saved");
}

void ApiMotors::handleGetStatus() {
  JsonDocument doc;

  // Motors
  JsonObject motorsObj = doc.createNestedObject("motors");
  JsonDocument motorsDoc;
  MotorManager::toJson(motorsDoc);
  motorsObj.set(motorsDoc.as<JsonObject>());

  // Safety
  JsonObject safetyObj = doc.createNestedObject("safety");
  SafetyManager::toJson(safetyObj);

  // Encoders
  JsonDocument encodersDoc;
  EncoderManager::toJson(encodersDoc);
  doc["encoders"] = encodersDoc["encoders"];

  // System info
  JsonObject sysObj = doc.createNestedObject("system");
  sysObj["heap"] = ESP.getFreeHeap();
  sysObj["uptime"] = millis() / 1000;
  sysObj["version"] = FIRMWARE_VERSION;

  ApiServer::sendJson(200, doc);
}
