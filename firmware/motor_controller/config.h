#pragma once

#include <Arduino.h>

// ============================================================================
// ESP32 Modular Motor Controller - Configuration
// ============================================================================

// === Version Info ===
#define FIRMWARE_VERSION "1.0.0"
#define FIRMWARE_NAME "ESP32 Motor Controller"

// === System Constants ===
constexpr uint8_t MAX_MOTORS = 4;
constexpr uint8_t MAX_ENCODERS = 2;
constexpr uint16_t MAX_PRESETS = 32;
constexpr uint16_t MAX_SEQUENCE_STEPS = 64;

// === Task Timing ===
constexpr uint32_t MOTOR_TASK_INTERVAL_MS = 1;     // 1kHz motor update
constexpr uint32_t ENCODER_TASK_INTERVAL_MS = 10;  // 100Hz encoder read
constexpr uint32_t API_POLL_INTERVAL_MS = 50;      // 20Hz API handling
constexpr uint32_t SAFETY_CHECK_INTERVAL_MS = 10;  // 100Hz safety checks

// === Task Stack Sizes ===
constexpr uint32_t MOTOR_TASK_STACK = 4096;
constexpr uint32_t ENCODER_TASK_STACK = 2048;
constexpr uint32_t PLAYBACK_TASK_STACK = 4096;

// === Task Priorities ===
constexpr uint8_t MOTOR_TASK_PRIORITY = 3;    // Highest - time critical
constexpr uint8_t ENCODER_TASK_PRIORITY = 2;
constexpr uint8_t PLAYBACK_TASK_PRIORITY = 1;

// === Core Assignments ===
constexpr uint8_t MOTOR_TASK_CORE = 0;    // Dedicated core for motor control
constexpr uint8_t ENCODER_TASK_CORE = 1;
constexpr uint8_t PLAYBACK_TASK_CORE = 1;

// ============================================================================
// Default Pin Assignments (can be overridden at runtime)
// ============================================================================

// Motor Slot 0 - Primary pins
constexpr uint8_t SLOT0_PIN_A = 25;   // Direction/Step/IN1
constexpr uint8_t SLOT0_PIN_B = 26;   // Direction/IN2
constexpr uint8_t SLOT0_PIN_EN = 27;  // Enable/PWM

// Motor Slot 1
constexpr uint8_t SLOT1_PIN_A = 14;
constexpr uint8_t SLOT1_PIN_B = 12;
constexpr uint8_t SLOT1_PIN_EN = 13;

// Motor Slot 2
constexpr uint8_t SLOT2_PIN_A = 32;
constexpr uint8_t SLOT2_PIN_B = 33;
constexpr uint8_t SLOT2_PIN_EN = 15;

// Motor Slot 3
constexpr uint8_t SLOT3_PIN_A = 16;
constexpr uint8_t SLOT3_PIN_B = 17;
constexpr uint8_t SLOT3_PIN_EN = 4;

// === ULN2003 Additional Pins (for 28BYJ-48 stepper) ===
// When using ULN2003, we need 4 pins per motor
// PIN_A = IN1, PIN_B = IN2, PIN_EN = IN3, and one extra
constexpr uint8_t SLOT0_PIN_EX = 2;   // IN4 for ULN2003
constexpr uint8_t SLOT1_PIN_EX = 5;
constexpr uint8_t SLOT2_PIN_EX = 18;
constexpr uint8_t SLOT3_PIN_EX = 19;

// === Encoder Pins (ADC1 only - compatible with WiFi) ===
constexpr uint8_t ENC0_PIN_A = 34;  // Input only
constexpr uint8_t ENC0_PIN_B = 35;  // Input only
constexpr uint8_t ENC1_PIN_A = 36;  // Input only (VP)
constexpr uint8_t ENC1_PIN_B = 39;  // Input only (VN)

// === Safety Pins ===
constexpr uint8_t ESTOP_PIN = 0;      // Boot button - active LOW
constexpr uint8_t STATUS_LED_PIN = 2; // Onboard LED

// ============================================================================
// Motor Type Enumeration
// ============================================================================

enum class MotorType : uint8_t {
  NONE = 0,
  DC_L298N = 1,        // DC motor with L298N H-bridge
  DC_L9110S = 2,       // DC motor with L9110S H-bridge
  SERVO = 3,           // Standard servo (PWM)
  STEPPER_A4988 = 4,   // NEMA17 with A4988 driver
  STEPPER_DRV8825 = 5, // NEMA17 with DRV8825 driver
  STEPPER_ULN2003 = 6  // 28BYJ-48 with ULN2003 driver
};

// Get motor type name string
inline const char* getMotorTypeName(MotorType type) {
  switch (type) {
    case MotorType::NONE: return "None";
    case MotorType::DC_L298N: return "DC (L298N)";
    case MotorType::DC_L9110S: return "DC (L9110S)";
    case MotorType::SERVO: return "Servo";
    case MotorType::STEPPER_A4988: return "Stepper (A4988)";
    case MotorType::STEPPER_DRV8825: return "Stepper (DRV8825)";
    case MotorType::STEPPER_ULN2003: return "Stepper (ULN2003)";
    default: return "Unknown";
  }
}

// ============================================================================
// Safety State Enumeration
// ============================================================================

enum class SafetyState : uint8_t {
  NORMAL = 0,
  ESTOP_ACTIVE = 1,
  LIMIT_REACHED = 2,
  OVERCURRENT = 3,
  FAULT = 4
};

inline const char* getSafetyStateName(SafetyState state) {
  switch (state) {
    case SafetyState::NORMAL: return "Normal";
    case SafetyState::ESTOP_ACTIVE: return "E-Stop Active";
    case SafetyState::LIMIT_REACHED: return "Limit Reached";
    case SafetyState::OVERCURRENT: return "Overcurrent";
    case SafetyState::FAULT: return "Fault";
    default: return "Unknown";
  }
}

// ============================================================================
// Motor Command Types
// ============================================================================

enum class CommandType : uint8_t {
  STOP = 0,
  SET_SPEED = 1,      // DC motors: -255 to +255
  SET_POSITION = 2,   // Steppers: absolute position
  SET_ANGLE = 3,      // Servos: 0-180 degrees
  MOVE_RELATIVE = 4,  // Steppers: relative steps
  BRAKE = 5,          // DC motors: active brake
  COAST = 6,          // DC motors: free spin
  ENABLE = 7,         // Steppers: enable driver
  DISABLE = 8,        // Steppers: disable driver
  HOME = 9            // Move to home position
};

// ============================================================================
// Slot Configuration Structure
// ============================================================================

struct SlotPins {
  uint8_t pinA;
  uint8_t pinB;
  uint8_t pinEn;
  uint8_t pinEx;  // Extra pin for ULN2003

  SlotPins() : pinA(255), pinB(255), pinEn(255), pinEx(255) {}
  SlotPins(uint8_t a, uint8_t b, uint8_t en, uint8_t ex = 255)
    : pinA(a), pinB(b), pinEn(en), pinEx(ex) {}
};

// Default pin configurations for each slot
inline SlotPins getDefaultSlotPins(uint8_t slot) {
  switch (slot) {
    case 0: return SlotPins(SLOT0_PIN_A, SLOT0_PIN_B, SLOT0_PIN_EN, SLOT0_PIN_EX);
    case 1: return SlotPins(SLOT1_PIN_A, SLOT1_PIN_B, SLOT1_PIN_EN, SLOT1_PIN_EX);
    case 2: return SlotPins(SLOT2_PIN_A, SLOT2_PIN_B, SLOT2_PIN_EN, SLOT2_PIN_EX);
    case 3: return SlotPins(SLOT3_PIN_A, SLOT3_PIN_B, SLOT3_PIN_EN, SLOT3_PIN_EX);
    default: return SlotPins();
  }
}

// ============================================================================
// Network Configuration
// ============================================================================

constexpr uint16_t WEB_SERVER_PORT = 80;
constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 10000;
constexpr uint32_t AP_CHANNEL = 1;
constexpr uint8_t AP_MAX_CONNECTIONS = 4;

// Default AP settings (used if STA fails or not configured)
#define DEFAULT_AP_SSID "MotorController"
#define DEFAULT_AP_PASS "motor1234"
#define DEFAULT_HOSTNAME "motor-ctrl"

// ============================================================================
// PWM Configuration
// ============================================================================

constexpr uint32_t PWM_FREQUENCY = 1000;     // 1kHz for DC motors
constexpr uint8_t PWM_RESOLUTION = 8;        // 8-bit (0-255)
constexpr uint32_t SERVO_FREQUENCY = 50;     // 50Hz for servos
constexpr uint16_t SERVO_MIN_PULSE = 500;    // microseconds
constexpr uint16_t SERVO_MAX_PULSE = 2400;   // microseconds

// ============================================================================
// Stepper Configuration
// ============================================================================

// NEMA17 defaults
constexpr uint16_t NEMA17_STEPS_PER_REV = 200;  // 1.8 degree steps
constexpr float DEFAULT_STEPPER_SPEED = 1000;    // steps/sec
constexpr float DEFAULT_STEPPER_ACCEL = 500;     // steps/sec^2
constexpr float MAX_STEPPER_SPEED = 4000;        // steps/sec

// 28BYJ-48 defaults
constexpr uint16_t ULN2003_STEPS_PER_REV = 2048; // With 64:1 gearbox, half-step
constexpr float DEFAULT_28BYJ_SPEED = 10;        // RPM
constexpr float MAX_28BYJ_SPEED = 15;            // RPM

// Microstepping modes
enum class MicrostepMode : uint8_t {
  FULL = 1,
  HALF = 2,
  QUARTER = 4,
  EIGHTH = 8,
  SIXTEENTH = 16,
  THIRTYSECOND = 32  // DRV8825 only
};

// ============================================================================
// Safety Limits
// ============================================================================

constexpr float DEFAULT_CURRENT_LIMIT = 2.0;     // Amps
constexpr int32_t DEFAULT_POSITION_MIN = -100000;
constexpr int32_t DEFAULT_POSITION_MAX = 100000;
constexpr uint32_t ESTOP_DEBOUNCE_MS = 50;
