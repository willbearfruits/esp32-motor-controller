#!/bin/bash
# Upload firmware via OTA (WiFi)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
FIRMWARE_DIR="$PROJECT_ROOT/firmware/motor_controller"

# Default IP for AP mode
IP="${1:-192.168.4.1}"
BOARD="${BOARD:-esp32:esp32:esp32}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  Motor Controller OTA Upload${NC}"
echo -e "${GREEN}================================${NC}"
echo ""

# Check curl
if ! command -v curl &> /dev/null; then
    echo -e "${RED}Error: curl not found${NC}"
    exit 1
fi

# Build first
echo -e "${YELLOW}Building firmware...${NC}"
"$SCRIPT_DIR/build.sh"

# Find the binary
BUILD_DIR="$FIRMWARE_DIR/build/esp32.esp32.esp32"
if [ ! -d "$BUILD_DIR" ]; then
    BUILD_DIR=$(find "$FIRMWARE_DIR" -name "*.ino.bin" -type f -printf '%h\n' 2>/dev/null | head -1)
fi

BIN_FILE=$(find "$FIRMWARE_DIR" -name "*.ino.bin" -type f 2>/dev/null | head -1)

if [ -z "$BIN_FILE" ] || [ ! -f "$BIN_FILE" ]; then
    echo -e "${RED}Error: Binary file not found${NC}"
    echo "Build may have failed or binary is in unexpected location"
    exit 1
fi

echo ""
echo -e "${GREEN}Binary: $BIN_FILE${NC}"
echo -e "${GREEN}Size: $(stat -c%s "$BIN_FILE" 2>/dev/null || stat -f%z "$BIN_FILE") bytes${NC}"
echo ""

# Check device is reachable
echo -e "${YELLOW}Checking device at $IP...${NC}"
if ! curl -s --connect-timeout 5 "http://$IP/api/system/info" > /dev/null; then
    echo -e "${RED}Error: Cannot reach device at $IP${NC}"
    echo ""
    echo "Make sure:"
    echo "  1. You're connected to the device's WiFi network"
    echo "  2. The device is powered on and running"
    echo "  3. The IP address is correct"
    echo ""
    echo "Usage: $0 [ip_address]"
    exit 1
fi

echo -e "${GREEN}Device found!${NC}"
echo ""

# Get current version
CURRENT=$(curl -s "http://$IP/api/system/info" | grep -o '"version":"[^"]*"' | cut -d'"' -f4)
echo "Current firmware version: $CURRENT"
echo ""

# Upload
echo -e "${GREEN}Uploading firmware via OTA...${NC}"
echo ""

# Use curl to upload the firmware
RESPONSE=$(curl -X POST \
    -F "firmware=@$BIN_FILE" \
    --progress-bar \
    "http://$IP/api/system/ota" 2>&1)

echo ""
echo "Response: $RESPONSE"
echo ""

if echo "$RESPONSE" | grep -q "success"; then
    echo -e "${GREEN}OTA upload successful!${NC}"
    echo "Device is rebooting with new firmware..."
    echo ""
    echo "Wait a few seconds, then check: http://$IP/"
else
    echo -e "${RED}OTA upload may have failed${NC}"
    echo "Check the device serial output for details"
fi
