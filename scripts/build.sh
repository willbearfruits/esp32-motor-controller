#!/bin/bash
# Build firmware using Arduino CLI

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
FIRMWARE_DIR="$PROJECT_ROOT/firmware/motor_controller"

# Default board
BOARD="${BOARD:-esp32:esp32:esp32}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  Motor Controller Build${NC}"
echo -e "${GREEN}================================${NC}"
echo ""

# Check arduino-cli
if ! command -v arduino-cli &> /dev/null; then
    echo -e "${RED}Error: arduino-cli not found${NC}"
    echo "Install with: curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh"
    exit 1
fi

# Check for ESP32 core
if ! arduino-cli core list | grep -q "esp32:esp32"; then
    echo -e "${YELLOW}Installing ESP32 core...${NC}"
    arduino-cli core install esp32:esp32
fi

# Check/install required libraries
LIBS=("ArduinoJson" "ESP32Servo" "AccelStepper" "ESP32Encoder")
for lib in "${LIBS[@]}"; do
    if ! arduino-cli lib list | grep -q "$lib"; then
        echo -e "${YELLOW}Installing $lib library...${NC}"
        arduino-cli lib install "$lib"
    fi
done

# Generate web pages if needed
echo -e "${YELLOW}Generating web pages...${NC}"
python3 "$SCRIPT_DIR/generate_web_pages.py"

# Check for secrets.h
if [ ! -f "$FIRMWARE_DIR/secrets.h" ]; then
    echo -e "${YELLOW}Creating secrets.h from example...${NC}"
    cp "$FIRMWARE_DIR/secrets.h.example" "$FIRMWARE_DIR/secrets.h"
    echo -e "${YELLOW}Note: Edit secrets.h to set your WiFi credentials${NC}"
fi

# Build
echo ""
echo -e "${GREEN}Compiling for $BOARD...${NC}"
echo ""

arduino-cli compile \
    --fqbn "$BOARD" \
    --build-property "build.partitions=default" \
    --build-property "build.flash_size=4MB" \
    --warnings default \
    "$FIRMWARE_DIR"

echo ""
echo -e "${GREEN}Build successful!${NC}"
echo ""
echo "To upload: ./scripts/upload.sh [port]"
echo "To upload via OTA: ./scripts/ota_upload.sh [ip]"
