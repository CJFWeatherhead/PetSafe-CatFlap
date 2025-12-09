#!/bin/bash
# Build script for PIC16F886 firmware using XC8 compiler
# This script compiles all source files and links them into a .hex file

set -e  # Exit on error

# Configuration
MCU="16F886"
PROJECT_NAME="PetSafe-CatFlap"
BUILD_DIR="build/production"
OUTPUT_DIR="dist/XC8_PIC16F886/production"

# XC8 compiler settings
XC8_CC="xc8-cc"
CHIP_FLAG="-mcpu=${MCU}"
DEFINES="-D_XTAL_FREQ=19600000 -DFLAP_POT=1"
OPTIMIZATION="-O2"

# Source files to compile
SOURCE_FILES=(
    "configuration_bits.c"
    "interrupts.c"
    "main.c"
    "user.c"
    "serial.c"
    "rfid.c"
    "peripherials.c"
    "cat.c"
)

# Create output directories
mkdir -p "${BUILD_DIR}"
mkdir -p "${OUTPUT_DIR}"

echo "========================================="
echo "Building ${PROJECT_NAME} for PIC${MCU}"
echo "========================================="
echo ""

# Compile and link in one step (XC8 v2.x approach)
echo "Compiling and linking..."
${XC8_CC} ${CHIP_FLAG} ${DEFINES} ${OPTIMIZATION} \
    -o "${OUTPUT_DIR}/${PROJECT_NAME}.production.hex" \
    "${SOURCE_FILES[@]}"

# Check if compilation was successful
if [ -f "${OUTPUT_DIR}/${PROJECT_NAME}.production.hex" ]; then
    echo ""
    echo "========================================="
    echo "Build successful!"
    echo "Output: ${OUTPUT_DIR}/${PROJECT_NAME}.production.hex"
    echo "File size: $(wc -c < "${OUTPUT_DIR}/${PROJECT_NAME}.production.hex") bytes"
    echo "========================================="
    echo ""
    echo "This .hex file can be programmed to PIC16F886 using:"
    echo "  - PICkit 3"
    echo "  - PICkit 4"
    echo "  - ICD 3"
    echo "  - MPLAB Snap"
    echo "  - or any compatible PIC programmer"
    exit 0
else
    echo ""
    echo "ERROR: Build failed - .hex file not created"
    exit 1
fi
