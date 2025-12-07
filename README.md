# PetSafe Cat Flap Alternative Firmware

[![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](LICENSE)
[![PIC16F886](https://img.shields.io/badge/MCU-PIC16F886-orange.svg)]()
[![MPLAB X](https://img.shields.io/badge/IDE-MPLAB%20X%20v5-green.svg)]()

An alternative firmware for the **PetSafe Pet Porte 100-1023 rev.X4 PCB**, providing enhanced control and features for your cat flap. This firmware runs on the **PIC16F886** microcontroller and offers multiple operating modes, RFID tag management, and optional external control via serial interface.

## 🐱 Features

- **RFID Tag Authentication**: Learn up to multiple cat RFID tags
- **Multiple Operating Modes**:
  - Normal Mode: Standard RFID-controlled entrance
  - Vet Mode: Keep cats inside (entrance locked)
  - Night Mode: Automatic locking based on light sensor
  - Open Mode: Free access (no locking)
  - Learn Mode: Add new RFID tags
  - Clear Mode: Remove all stored tags
- **Light Sensor Integration**: Automatic operation based on ambient light
- **Optional Flap Position Sensing**: Track flap movement with external potentiometer
- **Serial Communication**: External control and monitoring capability
- **LED Indicators**: Visual feedback for different modes
- **Configurable Settings**: Adjustable thresholds and parameters

## 📋 Quick Start

### Prerequisites

- MPLAB X IDE v5.x or later
- XC8 Compiler v2.00 or later
- PIC programmer (PICkit 3, PICkit 4, ICD 3, or Snap)
- PetSafe Pet Porte 100-1023 rev.X4 PCB with PIC16F886

### Basic Usage

1. **Clone the repository**:
   ```bash
   git clone https://github.com/CJFWeatherhead/PetSafe-CatFlap.git
   cd PetSafe-CatFlap
   ```

2. **Open project in MPLAB X IDE**

3. **Build the project** (F11 or Build button)

4. **Program your PIC16F886** (F6 or Make and Program Device)

5. **Install and test** on your cat flap

For detailed deployment instructions, see **[DEPLOYMENT.md](DEPLOYMENT.md)** - a comprehensive guide for beginners.

## 📖 Documentation

- **[DEPLOYMENT.md](DEPLOYMENT.md)** - Complete guide to deploying firmware to PIC16F886 (highly recommended for first-time users!)
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Guidelines for contributing to this project
- **[SECURITY.md](SECURITY.md)** - Security policy and reporting vulnerabilities
- **[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)** - Community guidelines
- **[CHANGELOG.md](CHANGELOG.md)** - Version history and changes

## 🔧 Hardware Configuration

### Standard Configuration

The firmware is designed for the PetSafe Pet Porte 100-1023 rev.X4 PCB with no modifications required for basic functionality.

### Optional: Flap Position Potentiometer

For enhanced flap position sensing:

1. Connect a 10kΩ potentiometer to RA1 (Pin 3)
2. Enable the `FLAP_POT` macro when building
3. Configure flap position thresholds via serial interface

To disable flap position sensing, ensure `FLAP_POT` is not defined in your build configuration.

## 🎮 Operating Modes

| Mode | LED Indicators | Description |
|------|----------------|-------------|
| **Normal** | Off | Standard operation - entrance locked, exit open |
| **Vet** | Red blinking | Both locked - keep cat inside |
| **Night** | Red solid + Green status | Auto-lock based on light sensor |
| **Learn** | Green blinking | Add new RFID tags (hold green button 10s) |
| **Clear** | Both blinking | Remove all tags (hold both buttons 30s) |
| **Open** | Both solid | Free access - all unlocked |

### Button Controls

- **Red button short press**: Toggle night mode
- **Red button long press (>5s)**: Toggle vet mode  
- **Green button long press (>10s)**: Enter learn mode
- **Both buttons (>30s)**: Clear all stored RFID tags

## 🔌 Serial Communication

The firmware supports serial communication for monitoring and control:

- **Baud rate**: 9600
- **Data format**: 8N1 (8 data bits, no parity, 1 stop bit)

### Commands

- `S` - Request status (mode, light sensor, position, status bits)
- `Cxx` - Read/write configuration parameters
- `Mx` - Set operating mode (x = 0-6)

Example status response: `AM0L512P512S3\n`

## 🏗️ Building the Project

### Using MPLAB X IDE

1. Open project in MPLAB X
2. Select configuration: **XC8_PIC16F886**
3. Build → Build Main Project (F11)
4. Program → Make and Program Device (F6)

### Build Artifacts

Compiled firmware: `dist/XC8_PIC16F886/production/PetSafe-CatFlap.production.hex`

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Ways to Contribute

- Report bugs and issues
- Suggest new features
- Submit pull requests
- Improve documentation
- Test on different hardware configurations

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

- Original firmware developed for personal use ("Works on my cat :P")
- Developed using Microchip MPLAB X IDE and XC8 compiler
- Community contributions and feedback welcome

## 📞 Support

- **Issues**: Report bugs via [GitHub Issues](https://github.com/CJFWeatherhead/PetSafe-CatFlap/issues)
- **Discussions**: Ask questions in [GitHub Discussions](https://github.com/CJFWeatherhead/PetSafe-CatFlap/discussions)
- **Documentation**: See [DEPLOYMENT.md](DEPLOYMENT.md) for detailed help

## 🌟 Project Status

This project is functional and tested on the author's cat flap. Community testing and contributions are encouraged to improve compatibility and features.

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
│   ├── Light sensor (night mode)
│   └── Flap position (optional)
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
- **Serial Baud Rate**: 9600 bps

---

**Last Updated**: December 2025
