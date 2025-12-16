#include "preset_manager.h"
#include "motor_manager.h"
#include "safety_manager.h"

// Static member initialization
Preset PresetManager::currentPreset;
Preset PresetManager::recordingPreset;
char PresetManager::currentPresetName[32] = {0};
bool PresetManager::playing = false;
bool PresetManager::recording = false;
bool PresetManager::loopPlayback = false;
uint8_t PresetManager::currentStepIndex = 0;
uint32_t PresetManager::stepStartTime = 0;
TaskHandle_t PresetManager::playbackTask = nullptr;

void PresetManager::init() {
  // Ensure presets directory exists
  if (!LittleFS.exists("/presets")) {
    LittleFS.mkdir("/presets");
  }

  playing = false;
  recording = false;
  currentStepIndex = 0;

  Serial.println("[PRESET] Preset Manager initialized");
}

bool PresetManager::savePreset(const char* name, const Preset& preset) {
  String path = getPresetPath(name);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.printf("[PRESET] Failed to open %s for writing\n", path.c_str());
    return false;
  }

  JsonDocument doc;

  doc["name"] = preset.name;
  doc["stepCount"] = preset.stepCount;
  doc["loop"] = preset.loop;

  JsonArray stepsArr = doc.createNestedArray("steps");
  for (uint8_t i = 0; i < preset.stepCount; i++) {
    JsonObject stepObj = stepsArr.createNestedObject();
    stepObj["delayAfter"] = preset.steps[i].delayAfter;
    stepObj["commandCount"] = preset.steps[i].commandCount;

    JsonArray cmdsArr = stepObj.createNestedArray("commands");
    for (uint8_t j = 0; j < preset.steps[i].commandCount; j++) {
      JsonObject cmdObj = cmdsArr.createNestedObject();
      cmdObj["slot"] = preset.steps[i].commands[j].slot;
      cmdObj["command"] = static_cast<uint8_t>(preset.steps[i].commands[j].command);
      cmdObj["value"] = preset.steps[i].commands[j].value;
      cmdObj["duration"] = preset.steps[i].commands[j].duration;
    }
  }

  size_t written = serializeJson(doc, file);
  file.close();

  Serial.printf("[PRESET] Saved preset '%s' (%d steps, %d bytes)\n",
                name, preset.stepCount, written);
  return written > 0;
}

bool PresetManager::loadPreset(const char* name, Preset& preset) {
  String path = getPresetPath(name);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.printf("[PRESET] Preset '%s' not found\n", name);
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.printf("[PRESET] Failed to parse preset '%s': %s\n", name, error.c_str());
    return false;
  }

  // Clear preset
  memset(&preset, 0, sizeof(Preset));

  strlcpy(preset.name, doc["name"] | name, sizeof(preset.name));
  preset.stepCount = doc["stepCount"] | 0;
  preset.loop = doc["loop"] | false;

  JsonArray stepsArr = doc["steps"];
  uint8_t stepIdx = 0;
  for (JsonObject stepObj : stepsArr) {
    if (stepIdx >= MAX_SEQUENCE_STEPS) break;

    preset.steps[stepIdx].delayAfter = stepObj["delayAfter"] | 0;
    preset.steps[stepIdx].commandCount = stepObj["commandCount"] | 0;

    JsonArray cmdsArr = stepObj["commands"];
    uint8_t cmdIdx = 0;
    for (JsonObject cmdObj : cmdsArr) {
      if (cmdIdx >= MAX_MOTORS) break;

      preset.steps[stepIdx].commands[cmdIdx].slot = cmdObj["slot"] | 0;
      preset.steps[stepIdx].commands[cmdIdx].command =
        static_cast<CommandType>(cmdObj["command"] | 0);
      preset.steps[stepIdx].commands[cmdIdx].value = cmdObj["value"] | 0;
      preset.steps[stepIdx].commands[cmdIdx].duration = cmdObj["duration"] | 0;

      cmdIdx++;
    }
    preset.steps[stepIdx].commandCount = cmdIdx;
    stepIdx++;
  }
  preset.stepCount = stepIdx;

  Serial.printf("[PRESET] Loaded preset '%s' (%d steps)\n", name, preset.stepCount);
  return true;
}

bool PresetManager::deletePreset(const char* name) {
  String path = getPresetPath(name);

  if (LittleFS.remove(path)) {
    Serial.printf("[PRESET] Deleted preset '%s'\n", name);
    return true;
  }

  Serial.printf("[PRESET] Failed to delete preset '%s'\n", name);
  return false;
}

void PresetManager::listPresets(JsonArray& arr) {
  File dir = LittleFS.open("/presets");
  if (!dir || !dir.isDirectory()) {
    return;
  }

  File file = dir.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      String filename = file.name();
      if (filename.endsWith(".json")) {
        // Remove .json extension
        filename = filename.substring(0, filename.length() - 5);
        arr.add(filename);
      }
    }
    file = dir.openNextFile();
  }
}

uint16_t PresetManager::getPresetCount() {
  uint16_t count = 0;

  File dir = LittleFS.open("/presets");
  if (!dir || !dir.isDirectory()) {
    return 0;
  }

  File file = dir.openNextFile();
  while (file) {
    if (!file.isDirectory() && String(file.name()).endsWith(".json")) {
      count++;
    }
    file = dir.openNextFile();
  }

  return count;
}

bool PresetManager::presetExists(const char* name) {
  return LittleFS.exists(getPresetPath(name));
}

void PresetManager::playPreset(const char* name) {
  if (!loadPreset(name, currentPreset)) {
    return;
  }

  strlcpy(currentPresetName, name, sizeof(currentPresetName));
  playPreset(currentPreset);
}

void PresetManager::playPreset(const Preset& preset) {
  if (playing) {
    stopPlayback();
  }

  currentPreset = preset;
  playing = true;
  loopPlayback = preset.loop;
  currentStepIndex = 0;
  stepStartTime = millis();

  // Create playback task
  xTaskCreatePinnedToCore(
    playbackTaskFunc,
    "PlaybackTask",
    PLAYBACK_TASK_STACK,
    nullptr,
    PLAYBACK_TASK_PRIORITY,
    &playbackTask,
    PLAYBACK_TASK_CORE
  );

  Serial.printf("[PRESET] Playing preset '%s' (%d steps, loop=%d)\n",
                currentPreset.name, currentPreset.stepCount, loopPlayback);
}

void PresetManager::stopPlayback() {
  if (playing) {
    playing = false;

    // Wait for task to finish
    if (playbackTask != nullptr) {
      vTaskDelay(pdMS_TO_TICKS(100));
      playbackTask = nullptr;
    }

    // Stop all motors
    MotorManager::stopAll();

    Serial.println("[PRESET] Playback stopped");
  }
}

void PresetManager::tick() {
  // Playback is handled by FreeRTOS task, but we can do housekeeping here
  // Check if playback task ended unexpectedly
  if (playing && playbackTask == nullptr) {
    playing = false;
  }
}

bool PresetManager::isPlaying() {
  return playing;
}

uint8_t PresetManager::getCurrentStep() {
  return currentStepIndex;
}

const char* PresetManager::getCurrentPresetName() {
  return currentPresetName;
}

void PresetManager::startRecording(const char* name) {
  if (recording) {
    stopRecording();
  }

  memset(&recordingPreset, 0, sizeof(Preset));
  strlcpy(recordingPreset.name, name, sizeof(recordingPreset.name));
  recordingPreset.stepCount = 0;
  recordingPreset.loop = false;

  recording = true;
  Serial.printf("[PRESET] Recording started for '%s'\n", name);
}

void PresetManager::recordStep(const SequenceStep& step) {
  if (!recording) return;
  if (recordingPreset.stepCount >= MAX_SEQUENCE_STEPS) {
    Serial.println("[PRESET] Max steps reached, cannot record more");
    return;
  }

  recordingPreset.steps[recordingPreset.stepCount] = step;
  recordingPreset.stepCount++;

  Serial.printf("[PRESET] Recorded step %d\n", recordingPreset.stepCount);
}

void PresetManager::recordMotorState() {
  if (!recording) return;

  SequenceStep step;
  memset(&step, 0, sizeof(step));
  step.commandCount = 0;
  step.delayAfter = 500;  // Default 500ms delay

  for (uint8_t i = 0; i < MAX_MOTORS; i++) {
    MotorBase* motor = MotorManager::getMotor(i);
    if (motor == nullptr) continue;

    MotorCommand cmd;
    cmd.slot = i;
    cmd.duration = 0;

    MotorType type = motor->getType();

    if (type == MotorType::SERVO) {
      cmd.command = CommandType::SET_ANGLE;
      cmd.value = motor->getPosition();
    } else if (type == MotorType::STEPPER_A4988 || type == MotorType::STEPPER_DRV8825 ||
               type == MotorType::STEPPER_ULN2003) {
      cmd.command = CommandType::SET_POSITION;
      cmd.value = motor->getPosition();
    } else {
      cmd.command = CommandType::SET_SPEED;
      cmd.value = (int32_t)motor->getSpeed();
    }

    step.commands[step.commandCount++] = cmd;
  }

  recordStep(step);
}

void PresetManager::stopRecording() {
  if (!recording) return;

  recording = false;

  // Save the recorded preset
  if (recordingPreset.stepCount > 0) {
    savePreset(recordingPreset.name, recordingPreset);
  }

  Serial.printf("[PRESET] Recording stopped, %d steps saved\n", recordingPreset.stepCount);
}

bool PresetManager::isRecording() {
  return recording;
}

void PresetManager::toJson(JsonObject& obj) {
  obj["playing"] = playing;
  obj["recording"] = recording;
  obj["currentStep"] = currentStepIndex;
  obj["totalSteps"] = currentPreset.stepCount;
  obj["currentPreset"] = currentPresetName;
  obj["looping"] = loopPlayback;
  obj["presetCount"] = getPresetCount();

  JsonArray presetsArr = obj.createNestedArray("presets");
  listPresets(presetsArr);
}

bool PresetManager::presetToJson(const char* name, JsonObject& obj) {
  Preset preset;
  if (!loadPreset(name, preset)) {
    return false;
  }

  obj["name"] = preset.name;
  obj["stepCount"] = preset.stepCount;
  obj["loop"] = preset.loop;

  // Include full step data if requested
  JsonArray stepsArr = obj.createNestedArray("steps");
  for (uint8_t i = 0; i < preset.stepCount; i++) {
    JsonObject stepObj = stepsArr.createNestedObject();
    stepObj["index"] = i;
    stepObj["delayAfter"] = preset.steps[i].delayAfter;
    stepObj["commandCount"] = preset.steps[i].commandCount;

    JsonArray cmdsArr = stepObj.createNestedArray("commands");
    for (uint8_t j = 0; j < preset.steps[i].commandCount; j++) {
      JsonObject cmdObj = cmdsArr.createNestedObject();
      cmdObj["slot"] = preset.steps[i].commands[j].slot;
      cmdObj["command"] = static_cast<uint8_t>(preset.steps[i].commands[j].command);
      cmdObj["value"] = preset.steps[i].commands[j].value;
      cmdObj["duration"] = preset.steps[i].commands[j].duration;
    }
  }

  return true;
}

bool PresetManager::presetFromJson(const JsonObject& obj, Preset& preset) {
  memset(&preset, 0, sizeof(Preset));

  strlcpy(preset.name, obj["name"] | "Untitled", sizeof(preset.name));
  preset.loop = obj["loop"] | false;

  JsonArray stepsArr = obj["steps"];
  for (JsonObject stepObj : stepsArr) {
    if (preset.stepCount >= MAX_SEQUENCE_STEPS) break;

    SequenceStep& step = preset.steps[preset.stepCount];
    step.delayAfter = stepObj["delayAfter"] | 500;
    step.commandCount = 0;

    JsonArray cmdsArr = stepObj["commands"];
    for (JsonObject cmdObj : cmdsArr) {
      if (step.commandCount >= MAX_MOTORS) break;

      MotorCommand& cmd = step.commands[step.commandCount];
      cmd.slot = cmdObj["slot"] | 0;
      cmd.command = static_cast<CommandType>(cmdObj["command"] | 0);
      cmd.value = cmdObj["value"] | 0;
      cmd.duration = cmdObj["duration"] | 0;

      step.commandCount++;
    }

    preset.stepCount++;
  }

  return true;
}

String PresetManager::getPresetPath(const char* name) {
  String path = "/presets/";
  path += name;
  path += ".json";
  return path;
}

void PresetManager::playbackTaskFunc(void* param) {
  while (playing) {
    // Check for E-stop
    if (SafetyManager::isEstopActive()) {
      playing = false;
      break;
    }

    if (currentStepIndex >= currentPreset.stepCount) {
      if (loopPlayback) {
        currentStepIndex = 0;
        Serial.println("[PRESET] Looping preset");
      } else {
        playing = false;
        Serial.println("[PRESET] Playback complete");
        break;
      }
    }

    // Execute current step
    SequenceStep& step = currentPreset.steps[currentStepIndex];

    for (uint8_t i = 0; i < step.commandCount; i++) {
      MotorCommand& cmd = step.commands[i];
      MotorManager::sendCommand(cmd.slot, cmd.command, cmd.value, cmd.duration);
    }

    // Wait for delay
    if (step.delayAfter > 0) {
      vTaskDelay(pdMS_TO_TICKS(step.delayAfter));
    }

    currentStepIndex++;
  }

  playing = false;
  playbackTask = nullptr;
  vTaskDelete(nullptr);
}
