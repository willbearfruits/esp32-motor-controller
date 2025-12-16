#include "api_server.h"
#include "api_motors.h"
#include "api_presets.h"
#include "api_system.h"
#include "../web/web_pages.h"

WebServer ApiServer::server(WEB_SERVER_PORT);

void ApiServer::init() {
  setupRoutes();
  server.begin();
  Serial.printf("[API] Server started on port %d\n", WEB_SERVER_PORT);
}

void ApiServer::handle() {
  server.handleClient();
}

void ApiServer::setupRoutes() {
  // === HTML Pages ===
  server.on("/", HTTP_GET, handleRoot);
  server.on("/presets", HTTP_GET, handlePresets);
  server.on("/settings", HTTP_GET, handleSettings);

  // === Motor API ===
  ApiMotors::registerRoutes(server);

  // === Presets API ===
  ApiPresets::registerRoutes(server);

  // === System API ===
  ApiSystem::registerRoutes(server);

  // === 404 Handler ===
  server.onNotFound(handleNotFound);

  Serial.println("[API] Routes configured");
}

void ApiServer::handleRoot() {
  addCorsHeaders();
  server.send_P(200, "text/html", PAGE_INDEX);
}

void ApiServer::handlePresets() {
  addCorsHeaders();
  server.send_P(200, "text/html", PAGE_PRESETS);
}

void ApiServer::handleSettings() {
  addCorsHeaders();
  server.send_P(200, "text/html", PAGE_SETTINGS);
}

void ApiServer::handleNotFound() {
  addCorsHeaders();

  String message = "Not Found\n\n";
  message += "URI: " + server.uri() + "\n";
  message += "Method: " + String(server.method() == HTTP_GET ? "GET" : "POST") + "\n";

  server.send(404, "text/plain", message);
}

void ApiServer::sendJson(int code, JsonDocument& doc) {
  addCorsHeaders();
  String output;
  serializeJson(doc, output);
  server.send(code, "application/json", output);
}

void ApiServer::sendJson(int code, const char* json) {
  addCorsHeaders();
  server.send(code, "application/json", json);
}

void ApiServer::sendSuccess(const char* message) {
  addCorsHeaders();
  JsonDocument doc;
  doc["success"] = true;
  doc["message"] = message;
  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void ApiServer::sendError(int code, const char* message) {
  addCorsHeaders();
  JsonDocument doc;
  doc["success"] = false;
  doc["error"] = message;
  String output;
  serializeJson(doc, output);
  server.send(code, "application/json", output);
}

bool ApiServer::parseJson(JsonDocument& doc) {
  if (!server.hasArg("plain")) {
    return false;
  }

  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Serial.printf("[API] JSON parse error: %s\n", error.c_str());
    return false;
  }

  return true;
}

void ApiServer::addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}
