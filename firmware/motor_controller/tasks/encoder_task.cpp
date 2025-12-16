#include "encoder_task.h"
#include "../core/encoder_manager.h"

TaskHandle_t encoderTaskHandle = nullptr;
static bool taskRunning = false;

void encoderTask(void* pvParameters) {
  Serial.println("[TASK] Encoder task started on core " + String(xPortGetCoreID()));

  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t period = pdMS_TO_TICKS(ENCODER_TASK_INTERVAL_MS);

  for (;;) {
    // Update encoder readings
    EncoderManager::update();

    // Wait for next cycle
    vTaskDelayUntil(&lastWakeTime, period);
  }
}

void startEncoderTask() {
  if (encoderTaskHandle != nullptr) {
    Serial.println("[TASK] Encoder task already running");
    return;
  }

  BaseType_t result = xTaskCreatePinnedToCore(
    encoderTask,
    "EncoderTask",
    ENCODER_TASK_STACK,
    nullptr,
    ENCODER_TASK_PRIORITY,
    &encoderTaskHandle,
    ENCODER_TASK_CORE
  );

  if (result == pdPASS) {
    taskRunning = true;
    Serial.println("[TASK] Encoder task created successfully");
  } else {
    Serial.println("[TASK] Failed to create encoder task!");
  }
}

void suspendEncoderTask() {
  if (encoderTaskHandle != nullptr && taskRunning) {
    vTaskSuspend(encoderTaskHandle);
    taskRunning = false;
    Serial.println("[TASK] Encoder task suspended");
  }
}

void resumeEncoderTask() {
  if (encoderTaskHandle != nullptr && !taskRunning) {
    vTaskResume(encoderTaskHandle);
    taskRunning = true;
    Serial.println("[TASK] Encoder task resumed");
  }
}

bool isEncoderTaskRunning() {
  return taskRunning;
}
