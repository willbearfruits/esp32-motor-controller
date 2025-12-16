/**
 * ESP32 Modular Motor Controller
 *
 * Production-ready motor controller supporting:
 * - DC Motors (L298N, L9110S)
 * - Servo Motors
 * - Stepper Motors (NEMA17 A4988/DRV8825, 28BYJ-48 ULN2003)
 *
 * Features:
 * - 4 simultaneous motor slots
 * - Web UI Dashboard
 * - REST API
 * - OTA Updates
 * - Motion Presets/Sequences
 * - Encoder Feedback
 * - E-Stop Safety
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Preferences.h>

#include "config.h"
#include "secrets.h"

// Core managers
#include "core/motor_manager.h"
#include "core/safety_manager.h"
#include "core/encoder_manager.h"
#include "core/preset_manager.h"
#include "core/ota_manager.h"

// API handlers
#include "api/api_server.h"
#include "api/api_motors.h"
#include "api/api_presets.h"
#include "api/api_system.h"

// Web pages
#include "web/web_pages.h"

// Include all .cpp files for Arduino build system
#include "drivers/dc_motor.cpp"
#include "drivers/servo_motor.cpp"
#include "drivers/stepper_nema17.cpp"
#include "drivers/stepper_28byj48.cpp"
#include "core/motor_manager.cpp"
#include "core/safety_manager.cpp"
#include "core/encoder_manager.cpp"
#include "core/preset_manager.cpp"
#include "core/ota_manager.cpp"
#include "api/api_server.cpp"
#include "api/api_motors.cpp"
#include "api/api_presets.cpp"
#include "api/api_system.cpp"

// ============================================================================
// Global Objects
// ============================================================================

WebServer server(80);
Preferences preferences;

// Task handles
TaskHandle_t motorTaskHandle = NULL;
TaskHandle_t encoderTaskHandle = NULL;

// ============================================================================
// FreeRTOS Tasks
// ============================================================================

void motorTaskFunc(void* param) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t interval = pdMS_TO_TICKS(1);  // 1ms = 1kHz

  while (true) {
    if (!SafetyManager::isEstopActive()) {
      MotorManager::updateAll();
    }
    vTaskDelayUntil(&lastWakeTime, interval);
  }
}

void encoderTaskFunc(void* param) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t interval = pdMS_TO_TICKS(10);  // 10ms = 100Hz

  while (true) {
    EncoderManager::update();
    vTaskDelayUntil(&lastWakeTime, interval);
  }
}

// ============================================================================
// WiFi Setup
// ============================================================================

void setupWiFi() {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", USE_AP_MODE ? AP_SSID : WIFI_SSID);
  String password = preferences.getString("password", USE_AP_MODE ? AP_PASS : WIFI_PASS);
  bool apMode = preferences.getBool("apMode", USE_AP_MODE);
  preferences.end();

  if (apMode) {
    // Access Point mode
    Serial.println("[WiFi] Starting Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password.c_str());
    WiFi.softAPsetHostname(DEFAULT_HOSTNAME);

    Serial.printf("[WiFi] AP SSID: %s\n", ssid.c_str());
    Serial.printf("[WiFi] AP IP: %s\n", WiFi.softAPIP().toString().c_str());
  } else {
    // Station mode - connect to existing network
    Serial.println("[WiFi] Connecting to network...");
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(DEFAULT_HOSTNAME);
    WiFi.begin(ssid.c_str(), password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.printf("[WiFi] Connected to: %s\n", ssid.c_str());
      Serial.printf("[WiFi] IP Address: %s\n", WiFi.localIP().toString().c_str());
    } else {
      Serial.println();
      Serial.println("[WiFi] Connection failed! Starting AP fallback...");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(DEFAULT_HOSTNAME, "motorctrl");
      Serial.printf("[WiFi] Fallback AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    }
  }
}

// ============================================================================
// Web Server Routes
// ============================================================================

void setupWebRoutes() {
  // Serve web pages
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", PAGE_INDEX);
  });

  server.on("/presets", HTTP_GET, []() {
    server.send_P(200, "text/html", PAGE_PRESETS);
  });

  server.on("/settings", HTTP_GET, []() {
    server.send_P(200, "text/html", PAGE_SETTINGS);
  });

  // Register API routes
  ApiServer::init();
  ApiMotors::registerRoutes(server);
  ApiPresets::registerRoutes(server);
  ApiSystem::registerRoutes(server);

  // Handle preset dynamic routes (GET/DELETE/PLAY)
  server.onNotFound([]() {
    String uri = server.uri();

    // Handle /api/presets/{name} routes
    if (uri.startsWith("/api/presets/") && uri.length() > 13) {
      String remainder = uri.substring(13);
      int slashPos = remainder.indexOf('/');

      if (slashPos < 0) {
        // GET /api/presets/{name} or DELETE /api/presets/{name}
        if (server.method() == HTTP_GET) {
          ApiPresets::handleGetPreset();
          return;
        } else if (server.method() == HTTP_DELETE) {
          ApiPresets::handleDeletePreset();
          return;
        }
      } else if (remainder.endsWith("/play")) {
        // POST /api/presets/{name}/play
        if (server.method() == HTTP_POST) {
          ApiPresets::handlePlayPreset();
          return;
        }
      }
    }

    // Default 404
    server.send(404, "application/json", "{\"error\":\"Not found\"}");
  });

  server.begin();
  Serial.println("[Server] HTTP server started on port 80");
}

// ============================================================================
// Setup
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("========================================");
  Serial.printf("  %s v%s\n", FIRMWARE_NAME, FIRMWARE_VERSION);
  Serial.println("  ESP32 Modular Motor Controller");
  Serial.println("========================================");
  Serial.println();

  // Initialize LittleFS for presets
  if (!LittleFS.begin(true)) {
    Serial.println("[FS] LittleFS mount failed!");
  } else {
    Serial.println("[FS] LittleFS mounted");
  }

  // Initialize managers
  Serial.println("[Init] Initializing safety manager...");
  SafetyManager::init();

  Serial.println("[Init] Initializing motor manager...");
  MotorManager::init();

  Serial.println("[Init] Initializing encoder manager...");
  EncoderManager::init();

  Serial.println("[Init] Initializing preset manager...");
  PresetManager::init();

  Serial.println("[Init] Initializing OTA manager...");
  OTAManager::init();

  // Start FreeRTOS tasks
  Serial.println("[Init] Starting motor task...");
  xTaskCreatePinnedToCore(
    motorTaskFunc,
    "MotorTask",
    4096,
    NULL,
    configMAX_PRIORITIES - 1,
    &motorTaskHandle,
    0  // Core 0
  );

  Serial.println("[Init] Starting encoder task...");
  xTaskCreatePinnedToCore(
    encoderTaskFunc,
    "EncoderTask",
    2048,
    NULL,
    configMAX_PRIORITIES - 2,
    &encoderTaskHandle,
    1  // Core 1
  );

  // Setup WiFi
  setupWiFi();

  // Setup web server
  setupWebRoutes();

  Serial.println();
  Serial.println("[Ready] Motor Controller is ready!");
  Serial.println("[Ready] Open web browser to configure motors");
  Serial.println();
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
  // Handle HTTP requests
  server.handleClient();

  // Handle OTA updates
  OTAManager::handle();

  // Update preset playback
  PresetManager::tick();

  // Small delay to prevent WDT issues
  delay(1);
}
