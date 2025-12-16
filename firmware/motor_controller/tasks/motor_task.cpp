#include "motor_task.h"
#include "../core/motor_manager.h"
#include "../core/safety_manager.h"

TaskHandle_t motorTaskHandle = nullptr;
static bool taskRunning = false;

void motorControlTask(void* pvParameters) {
  Serial.println("[TASK] Motor control task started on core " + String(xPortGetCoreID()));

  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t period = pdMS_TO_TICKS(MOTOR_TASK_INTERVAL_MS);

  // Counter for occasional yields to other tasks
  uint32_t loopCounter = 0;

  for (;;) {
    // Check for E-stop first
    if (SafetyManager::isEstopActive()) {
      // Motors are already stopped by SafetyManager
      // Just wait and check periodically
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    // Update all motors
    MotorManager::updateAll();

    // Increment loop counter
    loopCounter++;

    // Every 100 iterations (100ms at 1kHz), yield explicitly to ensure
    // other tasks get CPU time even if motor operations are intensive
    if (loopCounter % 100 == 0) {
      vTaskDelay(1);  // Minimal yield
    }

    // Wait for next cycle
    vTaskDelayUntil(&lastWakeTime, period);
  }
}

void startMotorTask() {
  if (motorTaskHandle != nullptr) {
    Serial.println("[TASK] Motor task already running");
    return;
  }

  BaseType_t result = xTaskCreatePinnedToCore(
    motorControlTask,
    "MotorTask",
    MOTOR_TASK_STACK,
    nullptr,
    MOTOR_TASK_PRIORITY,
    &motorTaskHandle,
    MOTOR_TASK_CORE
  );

  if (result == pdPASS) {
    taskRunning = true;
    Serial.println("[TASK] Motor task created successfully");
  } else {
    Serial.println("[TASK] Failed to create motor task!");
  }
}

void suspendMotorTask() {
  if (motorTaskHandle != nullptr && taskRunning) {
    vTaskSuspend(motorTaskHandle);
    taskRunning = false;
    Serial.println("[TASK] Motor task suspended");
  }
}

void resumeMotorTask() {
  if (motorTaskHandle != nullptr && !taskRunning) {
    vTaskResume(motorTaskHandle);
    taskRunning = true;
    Serial.println("[TASK] Motor task resumed");
  }
}

bool isMotorTaskRunning() {
  return taskRunning;
}
