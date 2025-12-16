# ESP32 Modular Motor Controller

## Project Overview
This project is a production-ready, modular motor controller firmware for the ESP32 microcontroller. It is designed to drive up to 4 motors simultaneously, supporting mixed types (DC, Servo, Stepper) with a unified interface.

The system features:
- **Embedded Web Dashboard:** A responsive SPA for real-time control and configuration.
- **REST API:** A JSON-based API for automation and integration.
- **Motion Presets:** A system to record, save, and playback motion sequences.
- **Safety Systems:** Hardware E-Stop, software limits, and watchdog timers.
- **OTA Updates:** Wireless firmware flashing.

## Architecture

The firmware is built on the Arduino framework using FreeRTOS for task management.

### Directory Structure
- `firmware/motor_controller/`: Main firmware source code.
  - `core/`: System managers (Motors, Safety, Encoders, Presets).
  - `drivers/`: Hardware abstraction for different motor types (L298N, Servo, A4988, etc.).
  - `api/`: REST API implementation and route handling.
  - `tasks/`: FreeRTOS task definitions (Motor loop, Encoder polling).
  - `web/`: Generated C++ headers containing the Web UI assets.
- `webui/`: Source HTML/JS/CSS for the dashboard.
- `scripts/`: Build, upload, and utility scripts.
- `docs/`: Project documentation.

### Core Modules
1.  **MotorManager (`core/motor_manager`):** The central hub that manages the 4 motor slots. It handles the update loop and routes commands to the appropriate driver instance.
2.  **SafetyManager (`core/safety_manager`):** Monitors E-Stop (GPIO 0), system faults, and limits. Can globally disable all motors.
3.  **PresetManager (`core/preset_manager`):** Handles the storage (LittleFS) and execution of motion sequences.
4.  **Drivers (`drivers/`):** Polymorphic classes inheriting from `MotorBase` to implement specific motor control logic.

## Development Workflow

### Prerequisites
- **Arduino CLI:** Required for the build scripts.
- **Python 3:** Required for `generate_web_pages.py`.
- **Libraries:** `ArduinoJson`, `ESP32Servo`, `AccelStepper`, `ESP32Encoder`.

### Building & Flashing

The project uses shell scripts to wrap `arduino-cli` commands.

1.  **Build Firmware:**
    ```bash
    ./scripts/build.sh
    ```
    *This script also automatically runs `generate_web_pages.py` to embed the `webui/` files into `firmware/motor_controller/web/web_pages.h`.*

2.  **Upload via USB:**
    ```bash
    ./scripts/upload.sh [port]
    # Example: ./scripts/upload.sh /dev/ttyUSB0
    ```

3.  **Upload via OTA (WiFi):**
    ```bash
    ./scripts/ota_upload.sh <IP_ADDRESS>
    ```

### Configuration
-   **Hardware:** Pin definitions and system constants are in `firmware/motor_controller/config.h`.
-   **WiFi:** Credentials are set in `firmware/motor_controller/secrets.h` (copy from `secrets.h.example`).

## Web UI Development
The Web UI is located in `webui/`. It is a simple HTML/JS application.
-   To update the UI, edit the files in `webui/`.
-   Run `./scripts/build.sh` to regenerate the C++ headers and recompile the firmware.

## API Reference
The device exposes a REST API on port 80.

-   `GET /api/motors`: Status of all motors.
-   `POST /api/motors/{slot}/control`: Send movement commands.
-   `POST /api/presets/{name}/play`: Execute a stored preset.
-   `POST /api/system/estop`: Trigger emergency stop.

*See `docs/documentation.md` for the full API documentation.*
