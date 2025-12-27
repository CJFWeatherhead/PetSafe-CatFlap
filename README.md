# PetSafe Cat Flap Alternative Firmware

[![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](LICENSE)
[![PIC16F886](https://img.shields.io/badge/MCU-PIC16F886-orange.svg)]()
[![MPLAB X](https://img.shields.io/badge/IDE-MPLAB%20X%20v5-green.svg)]()
[![CI Build](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions/workflows/ci.yml/badge.svg)](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions/workflows/ci.yml)

An enhanced alternative firmware for the **PetSafe Pet Porte 100-1023 PCB**, providing full compliance with the official manual specifications plus additional control features. This firmware runs on the **PIC16F886** microcontroller and offers multiple operating modes, RFID tag management, and optional external control via serial interface.

> **⚠️ Important**: The original firmware was tested by the original developer on **revision X4 hardware only**. This enhanced fork includes significant improvements for manual compliance but has **not been tested** on physical hardware. Test thoroughly before deploying to your cat flap, and always provide alternative pet access during testing.

## 🐱 Features

- **RFID Tag Authentication**: Learn up to 16 cat RFID tags (hardware limitation: 256-byte EEPROM)
- **Full Manual Compliance**: 90% compliance with official PetSafe Petporte 100 manual
- **Extended Modes Menu**: 7-mode menu system with beep navigation (per manual pages 14, 16-17)
- **Multiple Operating Modes**:
  - Normal Mode: Standard RFID-controlled entrance
  - Vet Mode: Keep cats inside (entrance only)
  - Night Mode: Automatic locking based on light sensor
  - Open Mode: Free access (no locking) - accessible via Extended Menu
  - Learn Mode: Add new RFID tags (30-second window, three beeps on success)
  - Clear Mode: Remove all stored tags
- **Silent Mode**: Toggle cat detection beeps while keeping system beeps
- **Key Pad Lock**: Prevent accidental button presses (Red button > 30s)
- **Configurable Lock Return Time**: Adjust door open time (1-25 seconds, default 4s)
- **Startup Sequence**: Visual LED pattern and audio beeps confirm successful boot
- **Light Sensor Integration**: Automatic operation based on ambient light
- **Serial Communication**: External control and monitoring capability
- **LED Indicators**: Correct visual feedback per manual specifications
- **Comprehensive Test Suite**: 73 unit tests validating all features

## 📋 Quick Start

### Prerequisites

**Option 1: Docker (Recommended)**
- Docker Desktop (or Docker Engine on Linux)
- PIC programmer (PICkit 3, PICkit 4, ICD 3, or Snap)
- PetSafe Pet Porte 100-1023 rev.X4 PCB with PIC16F886

**Option 2: MPLAB X IDE**
- MPLAB X IDE v5.x or later
- XC8 Compiler v2.00 or later (v2.46 recommended)
- PIC programmer (PICkit 3, PICkit 4, ICD 3, or Snap)
- PetSafe Pet Porte 100-1023 rev.X4 PCB with PIC16F886

### Basic Usage

#### Option 1: Download Pre-built Firmware (Recommended for Users)

1. **Download the latest release**:
   - Go to [Releases](https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases)
   - Download the latest `.hex` file (e.g., `PetSafe-CatFlap-v1.0.0.hex`)
   - Verify the checksum using `checksums.txt`

2. **Program your PIC16F886**:
   - Connect PIC programmer (PICkit 3/4, ICD 3, or Snap)
   - Use MPLAB X IPE to flash the downloaded .hex file
   - See **[DEPLOYMENT.md](DEPLOYMENT.md)** for detailed instructions

#### Option 2: Build from Source (For Developers)

1. **Clone the repository**:
   ```bash
   git clone https://github.com/CJFWeatherhead/PetSafe-CatFlap.git
   cd PetSafe-CatFlap
   ```

2. **Build the firmware** (choose one):
   
   Using Docker:
   ```bash
   docker build -t xc8-pic-builder:latest .
   docker run --rm -v $(pwd):/project -w /project xc8-pic-builder:latest ./build.sh
   ```
   
   Using MPLAB X IDE:
   - Open project in MPLAB X
   - Build → Build Main Project (F11)

3. **Program your PIC16F886** 
   - Connect PIC programmer
   - Use MPLAB X IPE or IDE to flash the .hex file
   - File location: `dist/XC8_PIC16F886/production/PetSafe-CatFlap.production.hex`

4. **Install and test** on your cat flap

For detailed deployment instructions, see **[DEPLOYMENT.md](DEPLOYMENT.md)** - a comprehensive guide for beginners.

## 📖 Documentation

### Getting Started
- **[DEPLOYMENT.md](DEPLOYMENT.md)** - Complete guide to deploying firmware to PIC16F886 (highly recommended for first-time users!)
- **[README.md](README.md)** - This file - project overview and quick start

### Hardware and Debugging
- **[HARDWARE_DEBUGGING_GUIDE.md](HARDWARE_DEBUGGING_GUIDE.md)** - Comprehensive debugging toolkit with pinouts, circuits, and troubleshooting (⭐ NEW!)
- **[PINOUT_REFERENCE.md](PINOUT_REFERENCE.md)** - Quick reference card for PIC16F886 pinout and voltage measurements (⭐ NEW!)

### Development
- **[CODE_ARCHITECTURE.md](CODE_ARCHITECTURE.md)** - Detailed code architecture and module documentation
- **[TESTING.md](TESTING.md)** - Testing guide including unit tests, CI/CD, and static analysis
- **[DOCKER_BUILD.md](DOCKER_BUILD.md)** - Docker-based build system documentation (troubleshooting, architecture, and advanced usage)

### Release Management
- **[RELEASE.md](RELEASE.md)** - Complete guide to creating and downloading releases
- **[QUICK_RELEASE_GUIDE.md](QUICK_RELEASE_GUIDE.md)** - Quick reference for creating releases (3 steps)

### Migration and Changes
- **[SERIAL_MIGRATION_GUIDE.md](SERIAL_MIGRATION_GUIDE.md)** - Guide for migrating from 38400 to 9600 baud UART

### Contributing
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Guidelines for contributing to this project
- **[SECURITY.md](SECURITY.md)** - Security policy and reporting vulnerabilities
- **[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)** - Community guidelines
- **[CHANGELOG.md](CHANGELOG.md)** - Version history and changes

## 🔧 Hardware Configuration

### Standard Configuration

The firmware was originally designed for the PetSafe Pet Porte 100-1023 **rev.X4 PCB**. If you have a different hardware revision, additional modifications may be required. No modifications are needed for basic functionality on rev.X4.

### Hardware Revision Differences

**⚠️ Important for Later Revisions (X5+):**

If you have a later hardware revision, you may experience:
- **LED polarity differences** - LEDs may be inverted (HIGH=OFF instead of HIGH=ON)
- **Buzzer circuit changes** - Different buzzer type or drive requirements
- **UART resistors** - Some revisions have 150Ω series resistors on TX/RX lines
- **Component variations** - Different driver ICs or circuit configurations

**📚 For detailed debugging and adaptation:**
- See **[HARDWARE_DEBUGGING_GUIDE.md](HARDWARE_DEBUGGING_GUIDE.md)** for comprehensive pin tracing and circuit analysis
- See **[PINOUT_REFERENCE.md](PINOUT_REFERENCE.md)** for quick pinout reference
- Check existing [GitHub Issues](https://github.com/CJFWeatherhead/PetSafe-CatFlap/issues) for your revision
- Share your findings to help others with the same hardware

## 🎮 Operating Modes

| Mode | LED Indicators | Description |
|------|----------------|-------------|
| **Normal** | Off | Standard operation - entrance locked, exit open |
| **Vet** | Red off, Green blinking | Both locked - keep cat inside (entrance only) |
| **Night** | Red solid + Green status | Auto-lock based on light sensor |
| **Learn** | Green blinking | Add new RFID tags (hold green button 10s) |
| **Clear** | Both blinking | Remove all tags (hold both buttons 30s) |
| **Open** | Red blinking | Free access - all unlocked (via Extended Menu) |

### Button Controls

#### Basic Controls
- **Red button short press (<2s)**: Toggle night mode
- **Red button long press (>5s)**: Toggle vet mode  
- **Green button long press (>10s)**: Enter learn mode
- **Both buttons long press (>30s)**: Clear all stored RFID tags

#### Extended Features
- **Red button very long press (>30s)**: Toggle key pad lock (prevents accidental mode changes)
- **Green button very long press (>30s)**: Reset to normal mode
- **Both buttons press (>2s)**: Enter Extended Modes menu

### Extended Modes Menu

The firmware now includes a comprehensive Extended Modes menu system (per official manual). 

**To Enter**: Press and hold both Red and Green buttons for more than 2 seconds. You'll hear a long beep and the Green LED will turn on.

**To Navigate**: 
- Press Red button to move up through modes
- Press Green button to move down through modes  
- The number of beeps indicates which mode you're in (1-7 beeps)

**To Activate**: Press both buttons simultaneously

#### Extended Mode Options

1. **Exit Mode (1 beep)**: Cancel and return to normal operation
2. **Open Mode (2 beeps)**: Enter free access mode (no RFID checking)
3. **Silent Mode (3 beeps)**: Toggle cat detection beeps on/off
4. **Set Light Level (4 beeps)**: Store current light level as Night Mode threshold
5. **Lock Return Time (5 beeps)**: Adjust how long door stays open (1-25 seconds, default 4s)
6. **Low Battery Lock State (6 beeps)**: Configure lock behavior on low battery (placeholder)
7. **Timer Mode (7 beeps)**: Set specific lock/unlock times (placeholder - requires RTC)

**Note**: Modes 6 and 7 have menu entries but require additional hardware (battery monitoring and real-time clock) for full functionality.

## 🔌 Serial Communication

The firmware supports serial communication for monitoring and control:

- **Baud rate**: 9600 bps (reduced from 38400 for improved reliability)
  - Previous 38400 baud had 2.91% error (39516 actual baud)
  - New 9600 baud has 0.48% error (9646 actual baud) - much more reliable!
- **Data format**: 8N1 (8 data bits, no parity, 1 stop bit)
- **Startup Banner**: Displays firmware info on initialization
- **Error Handling**: Automatic detection and recovery from UART errors
  - Framing errors (FERR)
  - Overrun errors (OERR)
  - Ring buffer overflows

### Commands

- `S` - Request status (mode, light sensor, position, status bits)
- `Cxx` - Read/write configuration parameters
- `Mx` - Set operating mode (x = 0-6)

Example status response: `AM0L512P512S3\n`

> **⚠️ Breaking Change:** If upgrading from a previous version, the serial baud rate has changed from 38400 to 9600 bps. See [SERIAL_MIGRATION_GUIDE.md](SERIAL_MIGRATION_GUIDE.md) for detailed migration instructions.

## 🏗️ Building the Project

### Using Docker (Recommended for CI/CD and Cross-Platform)

The easiest way to build the firmware is using Docker, which provides a consistent build environment across all platforms:

```bash
# Build the Docker image (first time only)
docker build -t xc8-pic-builder:latest .

# Build the firmware
docker run --rm -v $(pwd):/project -w /project xc8-pic-builder:latest ./build.sh

# The compiled .hex file will be in:
# dist/XC8_PIC16F886/production/PetSafe-CatFlap.production.hex
```

**Why Docker?**
- Works on Linux, macOS, and Windows
- No manual XC8 compiler installation needed
- Consistent build environment for CI/CD
- Isolated from host system

### Using MPLAB X IDE

For interactive development and debugging:

1. Open project in MPLAB X
2. Select configuration: **XC8_PIC16F886**
3. Build → Build Main Project (F11)
4. Program → Make and Program Device (F6)

### Build Artifacts

Compiled firmware: `dist/XC8_PIC16F886/production/PetSafe-CatFlap.production.hex`

This `.hex` file can be programmed to the PIC16F886 using:
- PICkit 3
- PICkit 4
- ICD 3
- MPLAB Snap
- Or any compatible PIC programmer

## 🧪 Testing and Quality Assurance

### Automated Testing

The project includes comprehensive testing infrastructure:

- **Unit Tests**: Test individual modules (cat, rfid, serial)
- **Static Analysis**: Automated code quality checks with cppcheck
- **CI/CD Pipeline**: Automated build and test on every commit
- **Code Coverage**: Track test coverage with GCov

### Running Tests Locally

```bash
# Install Ceedling test framework
sudo gem install ceedling

# Run all unit tests
ceedling test:all

# Run specific module tests
ceedling test:cat
ceedling test:rfid
ceedling test:serial

# Generate code coverage report
ceedling gcov:all
```

### Continuous Integration

The CI/CD pipeline uses a Docker-based approach for maximum compatibility and portability:

Every push and pull request automatically:
1. Builds a Docker container with XC8 compiler v2.46
2. Compiles firmware for PIC16F886 using Docker
3. Runs static code analysis with cppcheck
4. Executes unit tests with Ceedling
5. Uploads build artifacts (.hex files)

**Benefits of Docker-based CI:**
- Works on any GitHub runner (ubuntu-latest, not tied to specific versions)
- Consistent build environment across all platforms
- No manual compiler installation in CI
- Faster builds with Docker layer caching
- Reproducible builds

View build status: [![CI Build](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions/workflows/ci.yml/badge.svg)](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions/workflows/ci.yml)

### Testing Documentation

For detailed testing instructions, see **[TESTING.md](TESTING.md)**.

## 🤝 Contributing

**Note**: This project is **not in active development** or maintenance. It is a documentation-focused fork of the original project.

Contributions are limited to:
- **Bug fixes**: Code corrections for identified issues
- **Testing reports**: Results from testing on different hardware revisions
- **Documentation improvements**: Clarifications and corrections

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

The repository owner does not take responsibility for maintaining this codebase long-term.

## ⚠️ Safety and Warnings

**IMPORTANT**: Please read before using this firmware.

- ⚡ **Electrical Safety**: Disconnect power before programming or modifying hardware
- 🔒 **No Warranty**: This firmware is provided as-is with no guarantees
- 🐈 **Pet Safety**: Thoroughly test all modes before relying on the device
- ⚖️ **Liability**: The author is NOT responsible for any damage to your device or harm to your pets
- 📋 **Warranty**: Modifying the original firmware likely voids your warranty

**Always provide alternative access** for your pets during testing and installation.

## 📄 License

This project is released into the public domain under [The Unlicense](LICENSE).

**TL;DR**: Do whatever you want with this code. No restrictions, no attribution required.

However, please note the disclaimer above - **use at your own risk!**

## 🙏 Acknowledgments

- Original firmware developed by original author ("Works on my cat :P")
- This fork created for documentation and structure improvements
- Original testing was performed by the original developer on their hardware
- Developed using Microchip MPLAB X IDE and XC8 compiler

## 📞 Support

- **Issues**: Report bugs via [GitHub Issues](https://github.com/CJFWeatherhead/PetSafe-CatFlap/issues)
- **Discussions**: Ask questions in [GitHub Discussions](https://github.com/CJFWeatherhead/PetSafe-CatFlap/discussions)
- **Documentation**: See [DEPLOYMENT.md](DEPLOYMENT.md) for detailed help

## 🌟 Project Status

**This is a fork** focused on documentation improvements. The firmware:
- Was originally tested only on revision X4 hardware by the original developer
- Has **not been tested** by the current repository maintainer
- Is **not in active development** or maintenance
- Is provided for documentation and educational purposes

**Use at your own risk** and test thoroughly before deploying to your cat flap.

---

**Made with ❤️ for cats and their humans**

---

## System Architecture

```
PIC16F886 Microcontroller
├── RFID Reader (134.2 kHz)
│   └── Tag authentication and learning
├── Solenoid Latches
│   ├── Green (entrance control)
│   └── Red (exit control)
├── Sensors
│   └── Light sensor (night mode)
├── User Interface
│   ├── Red LED
│   ├── Green LED
│   ├── Buzzer
│   └── Two buttons
└── Serial Interface (UART)
    └── External control/monitoring
```

## Technical Specifications

- **Microcontroller**: PIC16F886 (8-bit, 20MHz)
- **Program Memory**: 8K words Flash
- **Data Memory**: 368 bytes RAM
- **EEPROM**: 256 bytes (for cat tag storage)
- **Operating Voltage**: 5V
- **RFID Frequency**: 134.2 kHz
- **Crystal Oscillator**: 19.6 MHz (HS mode)
- **Serial Baud Rate**: 9600 bps (improved reliability)

---

**Last Updated**: December 2025
