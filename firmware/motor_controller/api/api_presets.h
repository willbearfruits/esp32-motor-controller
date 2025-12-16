#pragma once

#include <Arduino.h>
#include <WebServer.h>

// ============================================================================
// Presets API Endpoints
// ============================================================================

namespace ApiPresets {
  void registerRoutes(WebServer& server);

  // GET /api/presets - List all presets
  void handleListPresets();

  // GET /api/presets/{name} - Get preset details
  void handleGetPreset();

  // POST /api/presets - Create new preset
  void handleCreatePreset();

  // DELETE /api/presets/{name} - Delete preset
  void handleDeletePreset();

  // POST /api/presets/{name}/play - Play preset
  void handlePlayPreset();

  // POST /api/presets/stop - Stop playback
  void handleStopPlayback();

  // POST /api/presets/record/start - Start recording
  void handleStartRecording();

  // POST /api/presets/record/step - Record current state
  void handleRecordStep();

  // POST /api/presets/record/stop - Stop recording and save
  void handleStopRecording();

  // GET /api/presets/status - Get playback/recording status
  void handleGetStatus();
}
