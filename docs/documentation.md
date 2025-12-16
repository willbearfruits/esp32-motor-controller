# ESP32 Motor Controller - Full Documentation

## Table of Contents

1. [Overview](#overview)
2. [Hardware Requirements](#hardware-requirements)
3. [Installation](#installation)
4. [Configuration](#configuration)
5. [Motor Types](#motor-types)
6. [Web Interface](#web-interface)
7. [REST API Reference](#rest-api-reference)
8. [Presets System](#presets-system)
9. [Safety Features](#safety-features)
10. [Troubleshooting](#troubleshooting)

---

## Overview

The ESP32 Motor Controller is a production-ready firmware for controlling up to 4 motors simultaneously using an ESP32 microcontroller. It supports multiple motor types including DC motors, servo motors, and stepper motors with various driver configurations.

### Key Features

- **Modular Design**: Each of 4 motor slots can be independently configured for different motor types
- **Web Dashboard**: Responsive web UI for real-time control and monitoring
- **REST API**: Full HTTP API for integration with other systems
- **Motion Presets**: Record and playback complex motion sequences
- **OTA Updates**: Update firmware wirelessly
- **Encoder Support**: Hardware PCNT-based quadrature encoder reading
- **Safety First**: Hardware E-stop, position limits, and watchdog protection

---

## Hardware Requirements

### Supported Boards

- ESP32 (WROOM, WROVER)
- ESP32-S3
- ESP32-C3

### Motor Drivers

| Motor Type | Supported Drivers |
|------------|-------------------|
| DC Motor | L298N, L9110S |
| Servo | Direct PWM (5V tolerant GPIO) |
| NEMA17 Stepper | A4988, DRV8825 |
| 28BYJ-48 Stepper | ULN2003 |

### Power Requirements

- ESP32: 3.3V (USB or external)
- Servos: 5-6V (external power recommended for multiple servos)
- DC Motors: 6-12V (via motor driver)
- Stepper Motors: 12-24V (via driver board)

**Important**: Always use external power for motors. Do not power motors from the ESP32's 5V/3.3V pins.

---

## Installation

### Method 1: Web Flasher

The easiest method using ESP Web Tools:

1. Open Chrome or Edge browser
2. Navigate to the web flasher page
3. Connect ESP32 via USB
4. Click "Install Motor Controller"
5. Follow the prompts

### Method 2: Manual Flash

If the Web Flasher doesn't work for you, you can download the pre-compiled binaries from the `docs/downloads/` directory and flash them using `esptool`.

1. **Download binary**: Get the correct `.bin` file for your board (ESP32, S3, or C3).
2. **Install esptool**: `pip install esptool`
3. **Flash**:

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x0 motor_controller_esp32_v1.0.0.bin
```

### Method 3: Arduino CLI

```bash
# Install Arduino CLI
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Install ESP32 core
arduino-cli core install esp32:esp32

# Install required libraries
arduino-cli lib install ArduinoJson ESP32Servo AccelStepper ESP32Encoder

# Clone and build
git clone <repository-url>
cd motors

# Configure
cp firmware/motor_controller/secrets.h.example firmware/motor_controller/secrets.h
# Edit secrets.h with your WiFi settings

# Build and upload
./scripts/upload.sh /dev/ttyUSB0
```

### Method 4: PlatformIO

```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    bblanchon/ArduinoJson@^7.0.0
    madhephaestus/ESP32Servo@^3.0.0
    waspinator/AccelStepper@^1.64
    madhephaestus/ESP32Encoder@^0.11.0
```

---

## Configuration

### WiFi Settings (secrets.h)

```cpp
// Access Point mode (device creates its own network)
#define WIFI_SSID "MotorController"
#define WIFI_PASSWORD "motorctrl"
#define WIFI_AP_MODE true

// Station mode (device connects to existing network)
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASSWORD "YourPassword"
#define WIFI_AP_MODE false
```

### Pin Configuration (config.h)

Default pin assignments for each slot:

```cpp
// Slot 0
#define SLOT0_PIN_A    13   // IN1/IA/STEP/Signal
#define SLOT0_PIN_B    12   // IN2/IB/DIR
#define SLOT0_PIN_C    14   // ENA/ENABLE

// Slot 1
#define SLOT1_PIN_A    27
#define SLOT1_PIN_B    26
#define SLOT1_PIN_C    25

// Slot 2
#define SLOT2_PIN_A    33
#define SLOT2_PIN_B    32
#define SLOT2_PIN_C    35

// Slot 3
#define SLOT3_PIN_A    19
#define SLOT3_PIN_B    18
#define SLOT3_PIN_C    5
```

---

## Motor Types

### DC Motor with L298N

The L298N is a dual H-bridge driver capable of driving two DC motors.

**Wiring:**
| L298N Pin | ESP32 Pin | Description |
|-----------|-----------|-------------|
| IN1 | GPIO 13 | Direction 1 |
| IN2 | GPIO 12 | Direction 2 |
| ENA | GPIO 14 | PWM Speed |
| GND | GND | Common ground |
| +12V | External | Motor power |

**Control:**
- Speed: -255 to 255 (negative = reverse)
- Brake: Active stop (both inputs HIGH)
- Coast: Free stop (both inputs LOW)

### DC Motor with L9110S

The L9110S uses PWM on both inputs for direction control.

**Wiring:**
| L9110S Pin | ESP32 Pin |
|------------|-----------|
| IA | GPIO 13 |
| IB | GPIO 12 |
| GND | GND |
| VCC | External 5-12V |

### Servo Motor

Standard RC servo using ESP32's LEDC PWM.

**Wiring:**
| Servo Wire | Connection |
|------------|------------|
| Signal (Orange/Yellow) | GPIO 13 |
| VCC (Red) | 5V External |
| GND (Brown/Black) | GND |

**Control:**
- Angle: 0 to 180 degrees
- Smooth transitions with configurable duration

### Stepper Motor (NEMA17 with A4988/DRV8825)

**Wiring:**
| Driver Pin | ESP32 Pin |
|------------|-----------|
| STEP | GPIO 13 |
| DIR | GPIO 12 |
| ENABLE | GPIO 14 |
| GND | GND |
| VMOT | 12-24V External |

**Features:**
- Acceleration/deceleration profiles
- Microstepping support
- Position tracking

### Stepper Motor (28BYJ-48 with ULN2003)

**Wiring:**
| ULN2003 Pin | ESP32 Pin |
|-------------|-----------|
| IN1 | GPIO 13 |
| IN2 | GPIO 12 |
| IN3 | GPIO 14 |
| IN4 | GPIO 27 |

**Specifications:**
- 2048 steps per revolution (with gearbox)
- Half-step mode for smoother operation

---

## Web Interface

### Dashboard (/)

The main control interface showing:
- 4 motor slot cards
- Real-time status updates (500ms polling)
- Motor type configuration
- Control interfaces based on motor type
- E-stop button
- System information

### Presets Page (/presets)

Motion sequence management:
- Create/edit/delete presets
- Visual step editor
- Playback controls
- Recording mode

### Settings Page (/settings)

System configuration:
- System information display
- WiFi configuration
- OTA firmware upload
- Factory reset
- Reboot

---

## REST API Reference

### System Endpoints

#### GET /api/system/info
Returns system information.

**Response:**
```json
{
  "firmware": "Motor Controller",
  "version": "1.0.0",
  "chip": "ESP32",
  "cores": 2,
  "cpuFreq": 240,
  "heapFree": 180000,
  "heapTotal": 320000,
  "uptime": 3600
}
```

#### POST /api/system/reboot
Reboots the device.

#### POST /api/system/factory-reset
Erases all settings and reboots.

#### GET /api/system/wifi
Returns current WiFi configuration.

#### POST /api/system/wifi
Updates WiFi settings.

**Request:**
```json
{
  "ssid": "NetworkName",
  "password": "password",
  "apMode": false
}
```

#### POST /api/system/ota
Upload firmware binary (multipart form data).

### Motor Endpoints

#### GET /api/motors
Returns all motor slots.

**Response:**
```json
{
  "slots": [
    {
      "slot": 0,
      "configured": true,
      "type": 1,
      "typeName": "DC_L298N",
      "position": 0,
      "speed": 128,
      "moving": true,
      "enabled": true
    }
  ],
  "configuredCount": 2
}
```

#### GET /api/motors/{slot}
Returns single motor slot status.

#### POST /api/motors/{slot}/configure
Configure motor type and pins.

**Request:**
```json
{
  "type": 1,
  "pins": {
    "in1": 13,
    "in2": 12,
    "ena": 14
  }
}
```

#### POST /api/motors/{slot}/control
Send control command.

**Request:**
```json
{
  "command": "speed",
  "value": 200,
  "duration": 1000
}
```

**Commands:**
| Command | Description | Value |
|---------|-------------|-------|
| stop | Stop motor | - |
| speed | Set speed | -255 to 255 |
| position | Go to position | steps |
| angle | Set servo angle | 0-180 |
| relative | Move relative | steps |
| brake | Active brake | - |
| coast | Coast/free | - |
| enable | Enable driver | - |
| disable | Disable driver | - |
| home | Home position | - |

#### POST /api/motors/{slot}/remove
Remove motor configuration.

#### POST /api/motors/stop-all
Emergency stop all motors.

#### POST /api/motors/save-config
Save configuration to flash.

### Preset Endpoints

#### GET /api/presets
List all presets.

#### POST /api/presets
Create new preset.

**Request:**
```json
{
  "name": "wave",
  "loop": true,
  "steps": [
    {"slot": 0, "command": "angle", "value": 0},
    {"slot": -1, "command": "delay", "value": 500},
    {"slot": 0, "command": "angle", "value": 180}
  ]
}
```

#### GET /api/presets/{name}
Get preset details.

#### DELETE /api/presets/{name}
Delete preset.

#### POST /api/presets/{name}/play
Start preset playback.

#### POST /api/presets/stop
Stop current playback.

#### GET /api/presets/status
Get playback/recording status.

#### POST /api/presets/record/start
Start recording mode.

#### POST /api/presets/record/step
Capture current motor state.

#### POST /api/presets/record/stop
Stop recording and save.

### Safety Endpoints

#### POST /api/system/estop
Trigger emergency stop.

#### POST /api/system/estop/reset
Reset emergency stop.

---

## Presets System

### Preset Structure

Presets are stored as JSON files in LittleFS under `/presets/`.

```json
{
  "name": "demo",
  "loop": false,
  "stepCount": 5,
  "steps": [
    {"slot": 0, "command": "speed", "value": 200},
    {"slot": -1, "command": "delay", "value": 1000},
    {"slot": 0, "command": "speed", "value": -200},
    {"slot": -1, "command": "delay", "value": 1000},
    {"slot": 0, "command": "stop", "value": 0}
  ]
}
```

### Special Commands

- `slot: -1` - Applies to all motors or is a system command
- `command: "delay"` - Wait for specified milliseconds
- `value` - Command-specific parameter

### Recording Mode

1. Start recording via API or web UI
2. Move motors to desired positions using normal controls
3. Click "Capture Step" to record current state
4. Repeat for each keyframe
5. Stop recording to save preset

---

## Safety Features

### Emergency Stop (E-Stop)

- **Hardware trigger**: GPIO 0 (boot button)
- **Software trigger**: POST /api/system/estop
- **Behavior**: Immediately stops all motors, disables drivers
- **Reset**: Must be explicitly cleared via API or web UI

### Position Limits

Configure soft limits per motor:

```cpp
motor->setPositionLimits(-10000, 10000);
```

Motors automatically stop when reaching limits.

### Watchdog

If enabled, motors stop automatically if no command received within timeout period:

```cpp
#define MOTOR_WATCHDOG_TIMEOUT_MS 5000
```

### Status LED

- **Solid**: Normal operation
- **Fast blink**: E-stop active
- **Slow blink**: WiFi connecting

---

## Troubleshooting

### Build Errors

**"ESP32Servo.h not found"**
```bash
arduino-cli lib install ESP32Servo
```

**"AccelStepper.h not found"**
```bash
arduino-cli lib install AccelStepper
```

### Runtime Issues

**Motor doesn't respond**
1. Check motor type selection matches your hardware
2. Verify pin assignments
3. Check E-stop status (should not be active)
4. Verify power supply

**Stepper motor vibrates but doesn't move**
1. Check wiring order for coils
2. Reduce speed setting
3. Increase driver current limit

**Servo jitters**
1. Use external 5V power supply
2. Add capacitor across servo power
3. Check for PWM conflicts

**WiFi won't connect**
1. Verify credentials in secrets.h
2. Check signal strength
3. Try AP mode first
4. Monitor serial output for errors

**OTA upload fails**
1. Ensure device and computer on same network
2. Check available flash space
3. Try USB upload as fallback
4. Verify binary file is not corrupted

### Serial Debug

Enable verbose logging:
```cpp
#define DEBUG_LEVEL 2  // 0=off, 1=errors, 2=verbose
```

Monitor at 115200 baud:
```bash
arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=115200
```
