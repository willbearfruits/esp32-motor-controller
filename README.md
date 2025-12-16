# ESP32 Modular Motor Controller

Production-ready modular motor controller for ESP32 supporting multiple motor types with web UI, REST API, OTA updates, motion presets, encoder feedback, and safety features.

![Motor Controller Dashboard](docs/assets/dashboard-preview.png)

## Features

- **4 Motor Slots** - Control up to 4 motors simultaneously
- **Multiple Motor Types**:
  - DC Motors (L298N, L9110S H-bridge drivers)
  - Servo Motors (standard RC servos, 0-180°)
  - Stepper Motors (NEMA17 with A4988/DRV8825, 28BYJ-48 with ULN2003)
- **Web Dashboard** - Beautiful responsive UI for configuration and control
- **REST API** - Full API for automation and integration
- **OTA Updates** - Update firmware wirelessly
- **Motion Presets** - Record and playback motion sequences
- **Encoder Feedback** - Hardware PCNT encoder support
- **Safety Features** - Hardware E-stop, position limits, fail-safe shutdown

## Quick Start

### Option 1: Web Flasher (Easiest)

1. Open [Web Flasher](https://yourusername.github.io/esp32-motor-controller/flasher.html) in Chrome/Edge
2. Connect ESP32 via USB
3. Click "Install Motor Controller"
4. Connect to WiFi network "MotorController" (password: `motorctrl`)
5. Open `http://192.168.4.1` in browser

### Option 2: Arduino CLI

```bash
# Clone repository
git clone https://github.com/yourusername/esp32-motor-controller.git
cd esp32-motor-controller/motors

# Configure WiFi credentials
cp firmware/motor_controller/secrets.h.example firmware/motor_controller/secrets.h
# Edit secrets.h with your settings

# Build and upload
./scripts/upload.sh /dev/ttyUSB0
```

### Option 3: Arduino IDE

1. Open `firmware/motor_controller/motor_controller.ino`
2. Install required libraries (see Dependencies)
3. Select your ESP32 board
4. Click Upload

## Dependencies

Install via Arduino Library Manager or arduino-cli:

```bash
arduino-cli lib install ArduinoJson ESP32Servo AccelStepper ESP32Encoder
```

| Library | Version | Purpose |
|---------|---------|---------|
| ArduinoJson | 7.x | REST API JSON handling |
| ESP32Servo | 3.x | Servo motor control |
| AccelStepper | 1.64+ | Stepper acceleration |
| ESP32Encoder | 0.11+ | Hardware PCNT encoders |

## Wiring

### Default Pin Assignments

| Slot | DC (L298N) | DC (L9110S) | Servo | Stepper (A4988) |
|------|------------|-------------|-------|-----------------|
| 0 | IN1:13, IN2:12, ENA:14 | IA:13, IB:12 | Signal:13 | STEP:13, DIR:12, EN:14 |
| 1 | IN1:27, IN2:26, ENA:25 | IA:27, IB:26 | Signal:27 | STEP:27, DIR:26, EN:25 |
| 2 | IN1:33, IN2:32, ENA:35 | IA:33, IB:32 | Signal:33 | STEP:33, DIR:32, EN:35 |
| 3 | IN1:19, IN2:18, ENA:5 | IA:19, IB:18 | Signal:19 | STEP:19, DIR:18, EN:5 |

### E-Stop Button
- GPIO 0 (Boot button) - Normally open, internal pull-up

### Encoders
- Encoder 0: A=34, B=35
- Encoder 1: A=36, B=39

## REST API

### Motor Control

```bash
# Get all motor status
curl http://192.168.4.1/api/motors

# Configure motor slot
curl -X POST http://192.168.4.1/api/motors/0/configure \
  -H "Content-Type: application/json" \
  -d '{"type": 1, "pins": {"in1": 13, "in2": 12, "ena": 14}}'

# Control motor
curl -X POST http://192.168.4.1/api/motors/0/control \
  -H "Content-Type: application/json" \
  -d '{"command": "speed", "value": 200}'

# Emergency stop all
curl -X POST http://192.168.4.1/api/motors/stop-all
```

### Motor Types
| Type | Value | Description |
|------|-------|-------------|
| NONE | 0 | Unconfigured |
| DC_L298N | 1 | DC motor with L298N |
| DC_L9110S | 2 | DC motor with L9110S |
| SERVO | 3 | RC Servo |
| STEPPER_A4988 | 4 | NEMA17 with A4988 |
| STEPPER_DRV8825 | 5 | NEMA17 with DRV8825 |
| STEPPER_ULN2003 | 6 | 28BYJ-48 with ULN2003 |

### Commands
| Command | Description | Value |
|---------|-------------|-------|
| stop | Stop motor | - |
| speed | Set speed | -255 to 255 |
| position | Go to position | steps |
| angle | Set servo angle | 0-180 |
| relative | Move relative | steps |
| brake | Active brake | - |
| coast | Coast stop | - |
| home | Home position | - |

### Presets

```bash
# List presets
curl http://192.168.4.1/api/presets

# Create preset
curl -X POST http://192.168.4.1/api/presets \
  -H "Content-Type: application/json" \
  -d '{"name": "wave", "loop": true, "steps": [
    {"slot": 0, "command": "angle", "value": 0},
    {"slot": -1, "command": "delay", "value": 500},
    {"slot": 0, "command": "angle", "value": 180}
  ]}'

# Play preset
curl -X POST http://192.168.4.1/api/presets/wave/play

# Stop playback
curl -X POST http://192.168.4.1/api/presets/stop
```

### System

```bash
# System info
curl http://192.168.4.1/api/system/info

# WiFi config
curl -X POST http://192.168.4.1/api/system/wifi \
  -H "Content-Type: application/json" \
  -d '{"ssid": "MyNetwork", "password": "secret", "apMode": false}'

# OTA update
curl -X POST http://192.168.4.1/api/system/ota \
  -F "firmware=@firmware.bin"

# Reboot
curl -X POST http://192.168.4.1/api/system/reboot

# Factory reset
curl -X POST http://192.168.4.1/api/system/factory-reset
```

## Project Structure

```
motors/
├── firmware/motor_controller/
│   ├── motor_controller.ino    # Main sketch
│   ├── config.h                # Configuration
│   ├── secrets.h.example       # WiFi template
│   ├── drivers/                # Motor drivers
│   │   ├── motor_base.h        # Abstract interface
│   │   ├── dc_motor.h/cpp      # DC motor driver
│   │   ├── servo_motor.h/cpp   # Servo driver
│   │   ├── stepper_nema17.h/cpp
│   │   └── stepper_28byj48.h/cpp
│   ├── core/                   # System modules
│   │   ├── motor_manager.h/cpp # Slot management
│   │   ├── safety_manager.h/cpp
│   │   ├── encoder_manager.h/cpp
│   │   ├── preset_manager.h/cpp
│   │   └── ota_manager.h/cpp
│   ├── api/                    # REST endpoints
│   │   ├── api_server.h/cpp
│   │   ├── api_motors.h/cpp
│   │   ├── api_presets.h/cpp
│   │   └── api_system.h/cpp
│   ├── tasks/                  # FreeRTOS tasks
│   │   ├── motor_task.h/cpp
│   │   └── encoder_task.h/cpp
│   └── web/web_pages.h         # PROGMEM HTML
├── webui/                      # Source HTML
│   ├── index.html              # Dashboard
│   ├── presets.html            # Preset editor
│   └── settings.html           # System settings
├── scripts/
│   ├── build.sh                # Compile
│   ├── upload.sh               # USB upload
│   ├── ota_upload.sh           # OTA upload
│   ├── build_release.sh        # Release builds
│   └── generate_web_pages.py   # HTML to PROGMEM
├── docs/                       # GitHub Pages
│   ├── index.html              # Landing page
│   ├── flasher.html            # Web flasher
│   └── downloads/              # Binaries
└── README.md
```

## Build Scripts

```bash
# Compile firmware
./scripts/build.sh

# Upload via USB (auto-detect port)
./scripts/upload.sh

# Upload via USB (specific port)
./scripts/upload.sh /dev/ttyUSB0

# Upload via OTA
./scripts/ota_upload.sh 192.168.4.1

# Build release binaries
./scripts/build_release.sh
```

## Configuration

### secrets.h

```cpp
#define WIFI_SSID "MotorController"
#define WIFI_PASSWORD "motorctrl"
#define WIFI_AP_MODE true  // true=AP, false=connect to existing network
```

### config.h

Key settings:
- `MAX_MOTORS` - Number of motor slots (default: 4)
- `MOTOR_UPDATE_RATE_HZ` - Control loop frequency (default: 1000)
- `DEFAULT_HOSTNAME` - mDNS hostname
- Pin definitions for each slot

## Safety Features

### E-Stop
- Hardware button on GPIO 0 (boot button)
- Instantly stops all motors
- Must be explicitly reset via API or web UI
- LED indicator on GPIO 2

### Position Limits
- Configurable min/max position per motor
- Automatic stop when limits reached
- Soft limits (configurable) and hard limits (emergency stop)

### Watchdog
- Motor command timeout
- Automatic stop if no commands received
- Configurable timeout period

## Troubleshooting

### Motor not moving
1. Check wiring connections
2. Verify motor type selection matches your driver
3. Check power supply voltage
4. Ensure E-stop is not active

### WiFi connection issues
1. Check credentials in secrets.h
2. Try AP mode first (default)
3. Check signal strength for STA mode
4. Use serial monitor to see connection status

### OTA upload fails
1. Ensure on same network as device
2. Check device IP address
3. Try smaller firmware binary
4. Check available flash space

### Stepper missing steps
1. Reduce max speed
2. Increase acceleration time
3. Check motor current limit on driver
4. Verify microstepping settings

## License

MIT License - see LICENSE file

## Contributing

1. Fork the repository
2. Create feature branch
3. Make changes
4. Test thoroughly
5. Submit pull request

## Acknowledgments

- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)
- [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/)
- [ESP32Encoder](https://github.com/madhephaestus/ESP32Encoder)
- [ArduinoJson](https://arduinojson.org/)
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/)
