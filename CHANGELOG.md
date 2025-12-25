# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added - Manual Compliance Review (December 2024)
- **Extended Modes Menu System**: Comprehensive menu system accessed via both buttons > 2s
  - Exit Mode (1 beep): Return to normal operation
  - Open Mode (2 beeps): Enter free access mode via menu
  - Silent Mode (3 beeps): Toggle cat detection beeps on/off
  - Set Light Level Mode (4 beeps): Store current light level as Night Mode threshold
  - Lock Return Time (5 beeps): Adjust door open time (1-25 seconds, default 4s)
  - Low Battery Lock State (6 beeps): Menu entry (requires battery monitoring hardware)
  - Timer Mode (7 beeps): Menu entry (requires RTC hardware)
- **Mode Navigation**: Red button = up, Green button = down, beep count indicates mode number
- **Enhanced Beep Functions**: Added beepShort(), beepLong(), beepSeries() for varied audio feedback
- **Mode Change Beeps**: Night Mode and Vet Mode now beep when toggling on/off
- **Power-On Indicator**: Both LEDs flash if no cats are programmed (per manual specification)
- **Key Pad Lock**: Red button > 30s toggles button lock to prevent accidental changes
- **Reset to Normal Mode**: Green button > 30s returns to Normal Mode with beep confirmation
- **Silent Mode**: Option to disable cat detection beeps while keeping system beeps
- **Configurable Lock Return Time**: Variable lock return time instead of hardcoded constant
- **anyCatsProgrammed()**: New function to check if device has stored cat IDs
- **MANUAL_COMPLIANCE.md**: Comprehensive document tracking compliance with official manual

### Changed - Manual Compliance
- **LED Patterns**: Fixed to match official manual specifications
  - Vet Mode: Changed from "Red flash" to "Green flash" (was incorrect)
  - Open Mode: Changed from "Both solid" to "Red flash" (was incorrect)
- **Default Lock Return Time**: Changed from 5 seconds to 4 seconds (per manual specification)
- **Cat Detection Beep**: Now respects silent mode setting (can be disabled)
- **Lock Return Time**: Now uses configurable variable instead of fixed OPEN_TIME constant

### Fixed - Manual Compliance
- Vet Mode LED indicator now correctly shows Green flashing (was Red flashing)
- Open Mode LED indicator now correctly shows Red flashing only (was both LEDs solid)
- Lock return time now defaults to 4 seconds as specified in manual (was 5 seconds)

### Added - Previous Updates
- Automated release workflow for incremental releases
- GitHub Actions workflow for creating releases from version tags
- Automatic attachment of .hex and .elf files to releases
- SHA256 checksums for release artifacts
- RELEASE.md documentation for release process
- Pre-built firmware downloads via GitHub Releases
- Serial echo functionality - received characters are echoed with hex display
- Verbose debugging output for all serial commands and errors

### Changed
- README.md updated with download instructions for pre-built firmware
- Release artifacts now versioned with tag names (e.g., PetSafe-CatFlap-v1.0.0.hex)
- **Serial protocol now uses human-readable text format instead of binary**:
  - Status command ('S') now outputs: `STATUS: Mode=X Light=Y Pos=Z Status=0xXXXX InLocked=X OutLocked=X`
  - Config commands now output: `CONFIG: Set/Read index=X value=Y`
  - Mode commands now output: `MODE: Changed to X`
  - Cat detection now outputs: `CAT_DETECTED: ID=XXXXXXXXXXXX CRC=0xXXXX`
  - All received characters are echoed with format: `RX: 'X' (0xXX)`
- Error messages are now descriptive instead of terse codes

### Fixed
- Serial communication now displays properly in terminals instead of garbled binary output

## [1.0.0] - Documentation Fork

### Added
- Comprehensive documentation (DEPLOYMENT.md, CODE_ARCHITECTURE.md)
- Standard Git project files (LICENSE, CONTRIBUTING.md, SECURITY.md, CODE_OF_CONDUCT.md)
- Enhanced README with better structure
- Improved .gitignore for embedded projects
- UART error detection and automatic recovery (framing errors, overrun errors)
- Error statistics tracking with UartErrors structure
- Startup banner displaying firmware information on serial initialization
- Improved ring buffer overflow protection

### Changed
- Repository structure improved with proper documentation
- Clarified project is a fork and not in active development
- **BREAKING**: Serial baud rate changed from 38400 to 9600 bps for improved reliability with crystal skew
- Enhanced UART interrupt handler with comprehensive error handling
- Improved serial.c code style to address cppcheck warnings

### Fixed
- UART framing and overrun errors now automatically cleared
- Ring buffer overflow detection prevents data corruption

## [1.0.0] - Original Release

### Added
- Initial firmware implementation
- RFID tag reading and management
- Multiple operating modes
- Serial communication interface
- Light sensor integration
- Optional flap position sensing

### Notes
- Tested by original developer on revision X4 hardware
- Works on original developer's cat flap

---

## Project Status

This is a **fork** of the original project, focused on documentation and structure improvements. The code is **not actively maintained** and testing has been limited to the original developer's hardware configuration.

Community contributions for bug fixes and testing reports are welcome.
