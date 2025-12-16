#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../config.h"

// ============================================================================
// Motor Control Task - High-Priority FreeRTOS Task
// ============================================================================
// Runs on Core 0 at high priority for smooth motor control.
// Updates all motors at ~1kHz for precise stepper timing.

void motorControlTask(void* pvParameters);

// Task handle for external control
extern TaskHandle_t motorTaskHandle;

// Task control functions
void startMotorTask();
void suspendMotorTask();
void resumeMotorTask();
bool isMotorTaskRunning();
