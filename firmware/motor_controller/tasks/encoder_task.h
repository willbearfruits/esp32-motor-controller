#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../config.h"

// ============================================================================
// Encoder Reading Task - FreeRTOS Task for Encoder Updates
// ============================================================================
// Runs on Core 1, updates encoder readings and calculates velocity.

void encoderTask(void* pvParameters);

extern TaskHandle_t encoderTaskHandle;

void startEncoderTask();
void suspendEncoderTask();
void resumeEncoderTask();
bool isEncoderTaskRunning();
