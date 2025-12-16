#!/bin/bash
# Upload firmware via USB serial

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
FIRMWARE_DIR="$PROJECT_ROOT/firmware/motor_controller"

# Default board and port
BOARD="${BOARD:-esp32:esp32:esp32}"
PORT="${1:-}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  Motor Controller Upload${NC}"
echo -e "${GREEN}================================${NC}"
echo ""

# Auto-detect port if not specified
if [ -z "$PORT" ]; then
    echo -e "${YELLOW}Detecting ESP32...${NC}"

    # Try common port patterns
    for p in /dev/ttyUSB* /dev/ttyACM* /dev/cu.usbserial* /dev/cu.SLAB*; do
        if [ -e "$p" ]; then
            PORT="$p"
            break
        fi
    done

    if [ -z "$PORT" ]; then
        echo -e "${RED}Error: No serial port found${NC}"
        echo "Available ports:"
        arduino-cli board list
        echo ""
        echo "Usage: $0 [port]"
        exit 1
    fi

    echo -e "${GREEN}Found: $PORT${NC}"
fi

# Check port exists
if [ ! -e "$PORT" ]; then
    echo -e "${RED}Error: Port $PORT not found${NC}"
    echo "Available ports:"
    arduino-cli board list
    exit 1
fi

# Build first
echo ""
echo -e "${YELLOW}Building firmware...${NC}"
"$SCRIPT_DIR/build.sh"

# Upload
echo ""
echo -e "${GREEN}Uploading to $PORT...${NC}"
echo ""

arduino-cli upload \
    --fqbn "$BOARD" \
    --port "$PORT" \
    "$FIRMWARE_DIR"

echo ""
echo -e "${GREEN}Upload successful!${NC}"
echo ""
echo "To monitor serial: arduino-cli monitor -p $PORT -c baudrate=115200"
echo "Or: screen $PORT 115200"
