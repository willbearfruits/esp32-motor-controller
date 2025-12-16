#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "../config.h"

// ============================================================================
// Preset Manager - Motion Sequence Storage and Playback
// ============================================================================

struct MotorCommand {
  uint8_t slot;
  CommandType command;
  int32_t value;
  uint16_t duration;  // ms, 0 = instant
};

struct SequenceStep {
  MotorCommand commands[MAX_MOTORS];
  uint8_t commandCount;
  uint16_t delayAfter;  // Delay after this step in ms
};

struct Preset {
  char name[32];
  SequenceStep steps[MAX_SEQUENCE_STEPS];
  uint8_t stepCount;
  bool loop;
};

class PresetManager {
public:
  // === Initialization ===
  static void init();

  // === Preset CRUD ===
  static bool savePreset(const char* name, const Preset& preset);
  static bool loadPreset(const char* name, Preset& preset);
  static bool deletePreset(const char* name);
  static void listPresets(JsonArray& arr);
  static uint16_t getPresetCount();
  static bool presetExists(const char* name);

  // === Playback ===
  static void playPreset(const char* name);
  static void playPreset(const Preset& preset);
  static void stopPlayback();
  static void tick();  // Call from main loop for playback processing
  static bool isPlaying();
  static uint8_t getCurrentStep();
  static const char* getCurrentPresetName();

  // === Recording ===
  static void startRecording(const char* name);
  static void recordStep(const SequenceStep& step);
  static void recordMotorState();  // Capture current state of all motors
  static void stopRecording();
  static bool isRecording();

  // === JSON ===
  static void toJson(JsonObject& obj);
  static bool presetToJson(const char* name, JsonObject& obj);
  static bool presetFromJson(const JsonObject& obj, Preset& preset);

private:
  static Preset currentPreset;
  static Preset recordingPreset;
  static char currentPresetName[32];

  static bool playing;
  static bool recording;
  static bool loopPlayback;
  static uint8_t currentStepIndex;
  static uint32_t stepStartTime;

  static TaskHandle_t playbackTask;
  static void playbackTaskFunc(void* param);

  static String getPresetPath(const char* name);
};
