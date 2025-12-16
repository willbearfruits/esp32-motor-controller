#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../config.h"

// ============================================================================
// API Server - WebServer Setup and Routing
// ============================================================================

class ApiServer {
public:
  // === Initialization ===
  static void init();
  static void handle();

  // === Page Handlers ===
  static void handleRoot();
  static void handlePresets();
  static void handleSettings();
  static void handleNotFound();

  // === Response Helpers ===
  static void sendJson(int code, JsonDocument& doc);
  static void sendJson(int code, const char* json);
  static void sendSuccess(const char* message = "OK");
  static void sendError(int code, const char* message);
  static bool parseJson(JsonDocument& doc);

  // === Server Access ===
  static WebServer& getServer() { return server; }

private:
  static WebServer server;
  static void setupRoutes();
  static void addCorsHeaders();
};
