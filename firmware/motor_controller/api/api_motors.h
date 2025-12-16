#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ============================================================================
// Motor API Endpoints
// ============================================================================

namespace ApiMotors {
  void registerRoutes(WebServer& server);

  // GET /api/motors - List all motor slots
  void handleGetMotors();

  // GET /api/motors/{slot} - Get specific motor status
  void handleGetMotor();

  // POST /api/motors/{slot}/configure - Configure motor type
  // Body: { "type": 1, "pins": { "pinA": 25, "pinB": 26, "pinEn": 27 } }
  void handleConfigureMotor();

  // POST /api/motors/{slot}/control - Send control command
  // Body: { "command": "speed", "value": 128, "duration": 0 }
  void handleControlMotor();

  // POST /api/motors/{slot}/remove - Remove motor from slot
  void handleRemoveMotor();

  // POST /api/motors/stop-all - Emergency stop all motors
  void handleStopAll();

  // POST /api/motors/save-config - Save motor configuration
  void handleSaveConfig();

  // GET /api/status - Full system status (motors, safety, etc.)
  void handleGetStatus();
}
