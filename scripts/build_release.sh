#!/bin/bash
# Build release binaries for multiple ESP32 variants

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
FIRMWARE_DIR="$PROJECT_ROOT/firmware/motor_controller"
DOCS_DIR="$PROJECT_ROOT/docs/downloads"

# Get version from config.h
VERSION=$(grep '#define FIRMWARE_VERSION' "$FIRMWARE_DIR/config.h" | cut -d'"' -f2)

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  Motor Controller Release Build${NC}"
echo -e "${GREEN}  Version: $VERSION${NC}"
echo -e "${GREEN}================================${NC}"
echo ""

# Board configurations
declare -A BOARDS
BOARDS["esp32"]="esp32:esp32:esp32"
BOARDS["esp32s3"]="esp32:esp32:esp32s3"
BOARDS["esp32c3"]="esp32:esp32:esp32c3"

# Create output directory
mkdir -p "$DOCS_DIR"

# Generate web pages
echo -e "${YELLOW}Generating web pages...${NC}"
python3 "$SCRIPT_DIR/generate_web_pages.py" --minify

# Check for secrets.h
if [ ! -f "$FIRMWARE_DIR/secrets.h" ]; then
    cp "$FIRMWARE_DIR/secrets.h.example" "$FIRMWARE_DIR/secrets.h"
fi

# Build for each board
for board_name in "${!BOARDS[@]}"; do
    board_fqbn="${BOARDS[$board_name]}"

    echo ""
    echo -e "${GREEN}Building for $board_name ($board_fqbn)...${NC}"

    # Clean previous build
    rm -rf "$FIRMWARE_DIR/build"

    # Compile
    if arduino-cli compile \
        --fqbn "$board_fqbn" \
        --build-property "build.partitions=default" \
        --warnings none \
        --output-dir "$FIRMWARE_DIR/build" \
        "$FIRMWARE_DIR" 2>/dev/null; then

        # Find and copy binary
        BIN_FILE=$(find "$FIRMWARE_DIR/build" -name "*.merged.bin" -type f | head -1)
        if [ -f "$BIN_FILE" ]; then
            OUTPUT_NAME="motor_controller_${board_name}_v${VERSION}.bin"
            cp "$BIN_FILE" "$DOCS_DIR/$OUTPUT_NAME"
            SIZE=$(stat -c%s "$DOCS_DIR/$OUTPUT_NAME" 2>/dev/null || stat -f%z "$DOCS_DIR/$OUTPUT_NAME")
            echo -e "${GREEN}  Created: $OUTPUT_NAME ($SIZE bytes)${NC}"
        fi
    else
        echo -e "${YELLOW}  Skipped: $board_name (build failed)${NC}"
    fi
done

# Generate manifest.json for ESP Web Tools
echo ""
echo -e "${YELLOW}Generating manifest.json...${NC}"

cat > "$DOCS_DIR/manifest.json" << EOF
{
  "name": "ESP32 Motor Controller",
  "version": "$VERSION",
  "home_assistant_domain": "esphome",
  "new_install_prompt_erase": true,
  "builds": [
    {
      "chipFamily": "ESP32",
      "parts": [
        { "path": "motor_controller_esp32_v${VERSION}.bin", "offset": 0 }
      ]
    },
    {
      "chipFamily": "ESP32-S3",
      "parts": [
        { "path": "motor_controller_esp32s3_v${VERSION}.bin", "offset": 0 }
      ]
    },
    {
      "chipFamily": "ESP32-C3",
      "parts": [
        { "path": "motor_controller_esp32c3_v${VERSION}.bin", "offset": 0 }
      ]
    }
  ]
}
EOF

echo ""
echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  Release build complete!${NC}"
echo -e "${GREEN}================================${NC}"
echo ""
echo "Output directory: $DOCS_DIR"
echo ""
ls -la "$DOCS_DIR"
