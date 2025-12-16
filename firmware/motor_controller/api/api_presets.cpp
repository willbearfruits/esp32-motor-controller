#include "api_presets.h"
#include "api_server.h"
#include "../core/preset_manager.h"

namespace {
  WebServer* _presetsServer = nullptr;

  String getPresetNameFromUri() {
    String uri = _presetsServer->uri();
    int start = uri.indexOf("/api/presets/");
    if (start < 0) return "";

    start += 13;
    int end = uri.indexOf("/", start);
    if (end < 0) end = uri.length();

    return uri.substring(start, end);
  }
}

void ApiPresets::registerRoutes(WebServer& server) {
  _presetsServer = &server;

  server.on("/api/presets", HTTP_GET, handleListPresets);
  server.on("/api/presets", HTTP_POST, handleCreatePreset);
  server.on("/api/presets/stop", HTTP_POST, handleStopPlayback);
  server.on("/api/presets/status", HTTP_GET, handleGetStatus);
  server.on("/api/presets/record/start", HTTP_POST, handleStartRecording);
  server.on("/api/presets/record/step", HTTP_POST, handleRecordStep);
  server.on("/api/presets/record/stop", HTTP_POST, handleStopRecording);

  Serial.println("[API] Preset routes registered");
}

void ApiPresets::handleListPresets() {
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  PresetManager::toJson(obj);
  ApiServer::sendJson(200, doc);
}

void ApiPresets::handleGetPreset() {
  String name = getPresetNameFromUri();
  if (name.length() == 0) {
    ApiServer::sendError(400, "Preset name required");
    return;
  }

  if (!PresetManager::presetExists(name.c_str())) {
    ApiServer::sendError(404, "Preset not found");
    return;
  }

  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  if (PresetManager::presetToJson(name.c_str(), obj)) {
    ApiServer::sendJson(200, doc);
  } else {
    ApiServer::sendError(500, "Failed to load preset");
  }
}

void ApiPresets::handleCreatePreset() {
  JsonDocument doc;
  if (!ApiServer::parseJson(doc)) {
    ApiServer::sendError(400, "Invalid JSON");
    return;
  }

  const char* name = doc["name"] | "";
  if (strlen(name) == 0) {
    ApiServer::sendError(400, "Preset name required");
    return;
  }

  Preset preset;
  if (!PresetManager::presetFromJson(doc.as<JsonObject>(), preset)) {
    ApiServer::sendError(400, "Invalid preset format");
    return;
  }

  if (PresetManager::savePreset(name, preset)) {
    JsonDocument response;
    response["success"] = true;
    response["message"] = "Preset created";
    response["name"] = name;
    ApiServer::sendJson(201, response);
  } else {
    ApiServer::sendError(500, "Failed to save preset");
  }
}

void ApiPresets::handleDeletePreset() {
  String name = getPresetNameFromUri();
  if (name.length() == 0) {
    ApiServer::sendError(400, "Preset name required");
    return;
  }

  if (PresetManager::deletePreset(name.c_str())) {
    ApiServer::sendSuccess("Preset deleted");
  } else {
    ApiServer::sendError(404, "Preset not found");
  }
}

void ApiPresets::handlePlayPreset() {
  String name = getPresetNameFromUri();
  if (name.length() == 0) {
    ApiServer::sendError(400, "Preset name required");
    return;
  }

  if (!PresetManager::presetExists(name.c_str())) {
    ApiServer::sendError(404, "Preset not found");
    return;
  }

  PresetManager::playPreset(name.c_str());
  ApiServer::sendSuccess("Playback started");
}

void ApiPresets::handleStopPlayback() {
  PresetManager::stopPlayback();
  ApiServer::sendSuccess("Playback stopped");
}

void ApiPresets::handleStartRecording() {
  JsonDocument doc;
  if (!ApiServer::parseJson(doc)) {
    ApiServer::sendError(400, "Invalid JSON");
    return;
  }

  const char* name = doc["name"] | "Recording";

  PresetManager::startRecording(name);
  ApiServer::sendSuccess("Recording started");
}

void ApiPresets::handleRecordStep() {
  if (!PresetManager::isRecording()) {
    ApiServer::sendError(400, "Not recording");
    return;
  }

  PresetManager::recordMotorState();
  ApiServer::sendSuccess("Step recorded");
}

void ApiPresets::handleStopRecording() {
  if (!PresetManager::isRecording()) {
    ApiServer::sendError(400, "Not recording");
    return;
  }

  PresetManager::stopRecording();
  ApiServer::sendSuccess("Recording saved");
}

void ApiPresets::handleGetStatus() {
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  PresetManager::toJson(obj);
  ApiServer::sendJson(200, doc);
}
