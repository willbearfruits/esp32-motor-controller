#pragma once

#include <Arduino.h>
#include <WebServer.h>

// ============================================================================
// System API Endpoints
// ============================================================================

namespace ApiSystem {
  void registerRoutes(WebServer& server);

  // GET /api/system/info - System information
  void handleGetInfo();

  // POST /api/system/ota - Upload firmware
  void handleOtaUpload();

  // POST /api/system/reboot - Reboot device
  void handleReboot();

  // POST /api/system/factory-reset - Clear all settings
  void handleFactoryReset();

  // POST /api/system/estop - Trigger emergency stop
  void handleEstop();

  // POST /api/system/estop/reset - Reset emergency stop
  void handleEstopReset();

  // GET /api/system/wifi - Get WiFi status
  void handleGetWifi();

  // POST /api/system/wifi - Configure WiFi
  void handleSetWifi();
}
