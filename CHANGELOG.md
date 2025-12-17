# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
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
