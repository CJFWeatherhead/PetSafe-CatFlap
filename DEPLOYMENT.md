# PIC16F886 Deployment Guide for PetSafe Cat Flap Firmware

This comprehensive guide will walk you through deploying the alternative firmware to your PetSafe Pet Porte 100-1023 PCB. This guide assumes no prior experience with embedded systems or PIC microcontroller programming.

> **⚠️ Important Notice**: This firmware was originally tested by the original developer on **revision X4 hardware only**. This documentation fork has **not been tested** on any hardware. Different PCB revisions may require modifications. Always test thoroughly and maintain a backup of your original firmware.

## Table of Contents

1. [Overview](#overview)
2. [Hardware Requirements](#hardware-requirements)
3. [Software Requirements](#software-requirements)
4. [Understanding the PIC16F886](#understanding-the-pic16f886)
5. [Setting Up the Development Environment](#setting-up-the-development-environment)
6. [**Backing Up Original Firmware**](#backing-up-original-firmware) ⭐ **Important!**
7. [Building the Firmware](#building-the-firmware)
8. [Programming the PIC16F886](#programming-the-pic16f886)
9. [Hardware Connections](#hardware-connections)
10. [Verification and Testing](#verification-and-testing)
11. [Troubleshooting](#troubleshooting)
12. [Safety and Precautions](#safety-and-precautions)

---

## Overview

This project provides alternative firmware for the PetSafe Pet Porte cat flap, which uses a **PIC16F886** microcontroller. The PIC16F886 is an 8-bit microcontroller from Microchip Technology with:

- 8K words of program memory (Flash)
- 368 bytes of RAM
- 256 bytes of EEPROM data memory
- 20 MHz maximum operating frequency
- 28-pin DIP or SOIC package
- Built-in analog-to-digital converter (ADC)
- Hardware USART for serial communication

The firmware controls:
- RFID tag reading (134.2 kHz)
- Solenoid latch mechanisms (entrance/exit control)
- LED indicators and buttons
- Light sensor for night mode
- Optional flap position potentiometer
- Serial communication for external control

---

## Hardware Requirements

### Essential Hardware

1. **PIC Programmer/Debugger** (choose one):
   - **PICkit 3** - Recommended for beginners (~$50-70)
   - **PICkit 4** - Latest version with more features (~$75-100)
   - **ICD 3** - In-Circuit Debugger (as referenced in project configuration)
   - **Snap** - Budget-friendly option (~$20-30)
   
   These devices connect your computer to the PIC microcontroller for programming and debugging.

2. **Connection Method** (choose one):
   - **ICSP (In-Circuit Serial Programming) Header**: Most convenient if your PCB has one
   - **Programming Adapter**: For direct chip programming if removed from board
   - **Pogo Pin Adapter**: For temporary connections to the PCB

3. **Cables**:
   - USB cable (usually included with programmer)
   - ICSP connection cable (usually included with programmer)

4. **The PetSafe Pet Porte PCB** with PIC16F886 microcontroller

### Optional but Recommended Hardware

- **Anti-static wrist strap**: Prevents damage from electrostatic discharge (ESD)
- **Multimeter**: For voltage verification and troubleshooting
- **USB-to-Serial adapter**: For monitoring serial communication (debugging)
- **Breadboard and jumper wires**: For testing before final installation
- **Magnifying glass or loupe**: For inspecting small components and solder joints

### Tools You'll Need

- Small screwdrivers (for opening the cat flap enclosure)
- Soldering iron and solder (if you need to add ICSP header pins)
- Wire strippers/cutters (if modifying connections)
- Helping hands or PCB holder (to keep board steady during programming)

---

## Software Requirements

### Required Software

1. **MPLAB X IDE** (Integrated Development Environment)
   - Version 5.x or later (project developed with v5.x)
   - Free download from [Microchip's website](https://www.microchip.com/mplab/mplab-x-ide)
   - Available for Windows, macOS, and Linux
   - Size: ~500MB-1GB depending on platform

2. **XC8 Compiler**
   - Version 2.00 or later (as specified in project configuration)
   - Free download from [Microchip's website](https://www.microchip.com/mplab/compilers)
   - Required to compile C code for PIC16 series microcontrollers
   - Size: ~200-400MB

3. **Git** (for cloning the repository)
   - Download from [git-scm.com](https://git-scm.com/)
   - Alternative: Download ZIP file directly from GitHub

### Optional Software

- **Terminal emulator** (for serial communication):
  - Windows: PuTTY, Tera Term, or Realterm
  - macOS/Linux: screen, minicom, or CoolTerm
  - Baud rate: 38400 bps (as defined in serial.h)

---

## Understanding the PIC16F886

### What is the PIC16F886?

The PIC16F886 is a microcontroller - essentially a tiny computer on a single chip. Unlike a regular computer, it's designed to run a single program repeatedly, making it perfect for embedded applications like the cat flap controller.

### Pin Configuration

The PIC16F886 has 28 pins in a DIP (Dual In-line Package) format. Key pins used in this project:

- **Pin 1 (MCLR/VPP)**: Master Clear/Reset pin - Used for programming and reset
- **Pin 9-10 (OSC1/OSC2)**: Crystal oscillator connections (19.6 MHz crystal)
- **Pin 20 (GND)**: Ground connection
- **Pin 1 (VDD)**: Power supply (+5V typically)
- **RA0-RA7, RB0-RB7, RC0-RC7**: General purpose I/O pins

### ICSP Programming Interface

The **ICSP (In-Circuit Serial Programming)** interface requires only 5 connections:

1. **VPP/MCLR** (Pin 1) - Programming voltage
2. **VDD** - Power supply (+5V)
3. **VSS** - Ground
4. **PGD** (Pin 13, RB7) - Programming Data
5. **PGC** (Pin 12, RB6) - Programming Clock

These pins allow you to program the chip without removing it from the circuit.

---

## Setting Up the Development Environment

### Step 1: Install MPLAB X IDE

1. **Download MPLAB X IDE**:
   - Visit: https://www.microchip.com/mplab/mplab-x-ide
   - Select your operating system (Windows, macOS, or Linux)
   - Download the installer (approximately 500MB-1GB)

2. **Run the installer**:
   - Windows: Run the `.exe` file as administrator
   - macOS: Open the `.dmg` file and drag to Applications
   - Linux: Extract and run the `.sh` script with sudo

3. **Follow the installation wizard**:
   - Accept the license agreement
   - Choose installation directory (default is recommended)
   - Installation takes 5-15 minutes depending on your system

### Step 2: Install XC8 Compiler

1. **Download XC8 Compiler**:
   - Visit: https://www.microchip.com/mplab/compilers
   - Select "MPLAB XC8 Compiler"
   - Choose your operating system
   - Download the installer (approximately 200-400MB)

2. **Run the installer**:
   - Windows: Run as administrator
   - macOS/Linux: Follow platform-specific instructions

3. **License selection**:
   - Select "Free" license (sufficient for this project)
   - Paid licenses offer optimizations but aren't required

4. **Verify installation**:
   - The installer should automatically detect MPLAB X
   - Compiler will integrate with MPLAB X IDE

### Step 3: Install Programmer Drivers

1. **Connect your PIC programmer to your computer**
   - Use the provided USB cable
   - Wait for your OS to recognize the device

2. **Install drivers** (if not automatic):
   - Windows: Drivers usually install automatically via Windows Update
   - macOS: Generally works without additional drivers
   - Linux: May require adding udev rules

3. **Verify programmer connection**:
   - Open MPLAB X IDE
   - Go to: Window → Output → Device Output
   - Your programmer should appear in the hardware tools list

### Step 4: Clone the Project Repository

Using Git:
```bash
git clone https://github.com/CJFWeatherhead/PetSafe-CatFlap.git
cd PetSafe-CatFlap
```

Or download as ZIP:
1. Visit the GitHub repository
2. Click "Code" → "Download ZIP"
3. Extract the ZIP file to your desired location

---

## Backing Up Original Firmware

**⚠️ CRITICAL: DO THIS BEFORE PROGRAMMING NEW FIRMWARE**

Before flashing this alternative firmware, you **must** back up your original firmware. This allows you to restore factory functionality if needed.

### Why Backup is Essential

- **Different hardware revisions**: This firmware was tested on rev.X4 only. Your revision may differ.
- **Restore capability**: Return to original firmware if issues occur
- **Safety net**: Preserve working configuration
- **No official source**: Original firmware may not be available elsewhere

### How to Backup Original Firmware

#### Using MPLAB X IDE

1. **Connect programmer to computer and PCB** (see [Hardware Connections](#hardware-connections))

2. **Power the board**:
   - Ensure programmer can detect the PIC
   - Board must be powered for reading

3. **Open MPLAB X IDE**

4. **Select your programmer**:
   - Window → Target Memory Views → Device Memory
   - Or: Click "Read Device Memory" icon in toolbar

5. **Read the current firmware**:
   - Go to: Run → Read Device Memory
   - Or: Right-click on project → "Read Device Memory"
   - Wait for read operation to complete

6. **Save the firmware file**:
   - File → Export → Hex/Unified Hex File
   - Or: Right-click memory view → "Export Table..."
   - Choose location: `original_firmware_backup_YYYYMMDD.hex`
   - **Important**: Use a descriptive filename with date
   - Save in a safe location (not in the project folder)

7. **Verify the backup**:
   - Check file size (should be several KB)
   - Open in text editor - should see Intel HEX format (`:` at start of lines)
   - **Keep multiple copies** (USB drive, cloud backup, etc.)

#### Alternative: Using PICkit Programmer Software

If using standalone PICkit programmer software:

1. **Launch PICkit programmer application**
2. **Connect to device**
3. **Select "Read Device"**
4. **Save as HEX file**: `original_firmware_backup_YYYYMMDD.hex`
5. **Verify file was created successfully**

### Backup Checklist

Before proceeding, verify:

- [ ] Backup HEX file created successfully
- [ ] File size is reasonable (not empty)
- [ ] Filename includes date for reference
- [ ] File copied to safe location (not just project folder)
- [ ] Backup file accessible from other computer (if possible)
- [ ] You can locate this file later

### Restoring Original Firmware

If you need to restore the original firmware:

1. **Open MPLAB X IDE**
2. **Create new project** or use existing
3. **Load your backup HEX file**:
   - Right-click project → Properties
   - Under "Loading", specify your backup HEX file
4. **Program device**: Run → Make and Program Device
5. **Verify operation**: Test your cat flap functionality

### Configuration Bits Warning

The backup includes configuration bits. When restoring:
- Configuration bits will be restored to original values
- This may differ from the alternative firmware configuration
- Original functionality should be fully restored

### If You Skip the Backup...

**Don't skip this step!** If you encounter problems without a backup:

- You may not be able to return to factory firmware
- PetSafe may not provide firmware files
- Your cat flap could be unusable
- You would need another working device to extract firmware from

**Take 5 minutes now to save potential hours of frustration later.**

---

## Building the Firmware

### Opening the Project in MPLAB X

1. **Launch MPLAB X IDE**

2. **Open the project**:
   - Go to: File → Open Project
   - Navigate to the cloned/extracted repository folder
   - Select the folder (you should see the MPLAB project icon)
   - Click "Open Project"

3. **Set as main project** (if not already):
   - Right-click the project name in the Projects window
   - Select "Set as Main Project"

### Configuring Build Options

The project is pre-configured, but you should verify settings:

1. **Select build configuration**:
   - Right-click project → Properties
   - Verify "Configuration" is set to "XC8_PIC16F886"

2. **Optional: Enable/Disable FLAP_POT feature**:
   
   The firmware supports an optional potentiometer to measure flap position.
   
   **To enable flap position sensing:**
   - Open `nbproject/Makefile-XC8_PIC16F886.mk`
   - Find the compiler flags section
   - Ensure `-DFLAP_POT` is present in `CFLAGS`
   
   **To disable (default hardware):**
   - Remove `-DFLAP_POT` from compiler flags, or
   - The Makefile should handle this automatically if not defined

3. **Verify programmer selection**:
   - Right-click project → Properties
   - Under "Conf: [XC8_PIC16F886]" → Select "ICD3" (or your programmer)
   - If using a different programmer:
     - Click "Hardware Tool" category
     - Select your programmer from the dropdown (PICkit 3, PICkit 4, Snap, etc.)

### Building the Firmware

1. **Clean the project** (recommended for first build):
   - Right-click project → Clean
   - Or: Click the clean icon (broom) in toolbar
   - Or: Run → Clean and Build Main Project

2. **Build the project**:
   - Click the "Build" button (hammer icon) in toolbar
   - Or: Run → Build Main Project
   - Or: Press F11

3. **Monitor the build process**:
   - Watch the "Output" window at the bottom
   - A successful build will show:
     ```
     BUILD SUCCESSFUL (total time: Xs)
     ```
   - Look for: `BUILD SUCCESSFUL` message

4. **Locate the compiled firmware**:
   - After successful build, the firmware file is located at:
   ```
   dist/XC8_PIC16F886/production/PetSafe-CatFlap.production.hex
   ```
   - This `.hex` file contains the compiled firmware ready for programming

### Understanding Build Errors

If the build fails, common issues include:

- **Compiler not found**: Verify XC8 installation
- **Missing files**: Ensure all source files are present
- **Syntax errors**: Check if any files were modified incorrectly
- **Configuration issues**: Verify project properties match your setup

---

## Programming the PIC16F886

### Safety First: Before Programming

⚠️ **CRITICAL SAFETY STEPS:**

1. **Disconnect power** from the cat flap completely
2. **Remove batteries** if battery-powered
3. **Verify voltage levels** with multimeter (should be 0V)
4. **Use anti-static protection** (wrist strap or touch grounded metal)
5. **Work on a non-conductive surface**

### Locating the PIC16F886 on the PCB

The PIC16F886 on the PetSafe Pet Porte 100-1023 rev.X4 PCB:

1. **Identify the chip**:
   - Look for a 28-pin IC (Integrated Circuit)
   - Should be labeled "PIC16F886" or "16F886"
   - May be in a DIP socket or soldered directly to the board

2. **Locate Pin 1**:
   - Look for a small dot or notch on the chip
   - Pin 1 is typically marked with a dot
   - Pins are numbered counter-clockwise from Pin 1

### Method 1: In-Circuit Programming (Recommended)

**If your PCB has an ICSP header** (5 or 6 pins in a row):

1. **Identify ICSP header pins**:
   - Look for labels: VPP, VDD, VSS, PGD, PGC
   - Or: 1=MCLR, 2=VDD, 3=VSS, 4=PGD, 5=PGC

2. **Connect the programmer**:
   ```
   Programmer          →    ICSP Header/PIC
   ─────────────────────────────────────────
   VPP/MCLR            →    Pin 1 (MCLR)
   VDD                 →    Pin 1 (VDD) - Power
   VSS                 →    Pin 20 (VSS) - Ground
   PGD (ICSPDAT)       →    Pin 13 (RB7)
   PGC (ICSPCLK)       →    Pin 12 (RB6)
   ```

3. **Verify connections**:
   - Double-check each wire connection
   - Ensure no pins are bridged (touching each other)
   - Use multimeter to verify continuity if unsure

4. **Power the board** (only one method):
   - Option A: Programmer provides power (check programmer settings)
   - Option B: External power supply (5V regulated)
   - ⚠️ **Never use both simultaneously!**

### Method 2: Socket Programming

**If the chip is in a socket** (can be removed):

1. **Carefully remove the PIC**:
   - Use a chip puller or small flathead screwdriver
   - Gently pry from both ends alternately
   - Never force or bend pins

2. **Insert into programmer's socket** (if available):
   - Align Pin 1 with marker on socket
   - Press down evenly
   - Ensure all pins are in the socket

3. **Program the chip** (follow programming steps below)

4. **Re-insert programmed chip**:
   - Align Pin 1 correctly
   - Press down evenly
   - Verify all pins are inserted

### Programming Steps in MPLAB X

1. **Ensure project is built** (see Building the Firmware section)

2. **Connect programmer to computer and PCB**

3. **Open programming interface**:
   - In MPLAB X: Window → Target Memory Views → Configuration Bits
   - Or: Click the "Make and Program Device" icon

4. **Verify programmer detection**:
   - Bottom-right corner should show programmer name
   - Status should be "Connected"
   - Target device should show "PIC16F886"

5. **Program the device**:
   - Click "Make and Program Device Main Project" button
   - Or: Run → Make and Program Device Main Project
   - Or: Press F6

6. **Monitor programming progress**:
   - Output window shows programming status:
     ```
     Programming...
     Downloading data...
     Programming complete
     ```

7. **Verify programming**:
   - MPLAB X performs automatic verification
   - Look for: "Programming/Verify complete" message
   - If verification fails, try programming again

### Configuration Bits

Configuration bits are special settings programmed into the PIC that control:

- **Oscillator type**: HS (High-Speed crystal, 19.6 MHz)
- **Watchdog timer**: Disabled
- **Code protection**: Disabled
- **Brown-out reset**: Disabled
- **Low-voltage programming**: Disabled

These are already set in `configuration_bits.c` and are programmed automatically with the firmware.

---

## Hardware Connections

### Standard Connections (No Modifications)

The firmware uses these connections on the PIC16F886:

#### Port A (PORTA):
- **RA0 (Pin 2)**: Analog input for light sensor
- **RA1 (Pin 3)**: Optional flap position potentiometer (if FLAP_POT enabled)
- **RA2 (Pin 4)**: RFID demodulated stream input
- **RA5 (Pin 7)**: Red latch solenoid control
- **RA6 (Pin 10)**: OSC2/CLKOUT - Crystal oscillator
- **RA7 (Pin 9)**: OSC1/CLKIN - Crystal oscillator

#### Port B (PORTB):
- **RB0 (Pin 21)**: Door switch input (with internal pull-up)
- **RB1 (Pin 22)**: Common lock output
- **RB2 (Pin 23)**: L293D 1/2 outputs enable
- **RB3 (Pin 24)**: LM324 op-amp power
- **RB4 (Pin 25)**: Red LED output
- **RB5 (Pin 26)**: Green LED output
- **RB6 (Pin 27)**: Red button input (with internal pull-up) / ICSP-PGC
- **RB7 (Pin 28)**: Green button input (with internal pull-up) / ICSP-PGD

#### Port C (PORTC):
- **RC0 (Pin 11)**: RFID/Red-Latch enable (L293D 3/4 enable)
- **RC1 (Pin 12)**: Buzzer output
- **RC2 (Pin 13)**: RFID excitation frequency (CCP1 PWM)
- **RC3 (Pin 14)**: L293D logic power control
- **RC4 (Pin 15)**: Green latch solenoid control
- **RC5 (Pin 16)**: Serial interface (not used in basic setup)
- **RC6 (Pin 17)**: TX - UART transmit (serial output)
- **RC7 (Pin 18)**: RX - UART receive (serial input)

### Optional Modification: External Potentiometer

If you want to add flap position sensing:

1. **Hardware addition**:
   - Connect a 10kΩ potentiometer to RA1 (Pin 3)
   - Connect potentiometer ends to GND and VDD
   - Connect wiper to RA1

2. **Mechanical mounting**:
   - Mount potentiometer shaft to flap mechanism
   - Ensure smooth rotation as flap moves

3. **Software configuration**:
   - Enable `FLAP_POT` macro in Makefile or compiler settings
   - Rebuild firmware with FLAP_POT defined

### Serial Communication Setup (Optional)

For debugging and external control:

1. **Hardware connection**:
   - Connect USB-to-Serial adapter:
     - Adapter TX → PIC RC7 (RX)
     - Adapter RX → PIC RC6 (TX)
     - Adapter GND → PIC VSS (GND)

2. **Serial settings**:
   - Baud rate: 38400 bps
   - Data bits: 8
   - Parity: None
   - Stop bits: 1
   - Flow control: None

3. **Communication protocol**:
   - **'S'**: Request status
   - **'C'**: Configuration read/write
   - **'M'**: Mode change
   - Response format documented in serial.c

---

## Verification and Testing

### Initial Power-Up Test

1. **Visual inspection**:
   - Remove programmer connections
   - Check for any loose wires or incorrect connections
   - Verify all components are properly seated

2. **Power up the device**:
   - Connect power supply or insert batteries
   - Observe LED behavior:
     - LEDs should illuminate briefly on power-up
     - System enters normal mode (LEDs off)

3. **Listen for buzzer**:
   - Present RFID tag (if already programmed)
   - Should hear brief beep when recognized

### Testing Each Mode

The cat flap has several operating modes:

#### Normal Mode (Default):
- LEDs: Off
- Behavior: Entrance locked (green latch), exit unlocked (red latch)
- RFID: Unlocks entrance when recognized tag detected

#### Vet Mode:
- LEDs: Red LED blinking
- Behavior: Both entrance and exit locked
- Purpose: Keep cat inside

#### Night Mode:
- LEDs: Red LED solid, Green LED indicates lock status
- Behavior: Automatically locks based on light sensor
- Test: Cover light sensor → should lock after 5 seconds

#### Learn Mode:
- How to enter: Hold green button for 10+ seconds
- LEDs: Green LED blinking
- Behavior: Learns new RFID tags
- Test: Present RFID tag → hear beep if successful

#### Clear Mode:
- How to enter: Hold both buttons for 30+ seconds
- Behavior: Erases all stored RFID tags
- Use cautiously: This removes all registered cats!

### Button Testing

- **Red button short press (<2s)**: Toggle night mode
- **Red button long press (>5s)**: Toggle vet mode
- **Green button long press (>10s)**: Enter learn mode
- **Both buttons (>30s)**: Clear all stored tags

### Serial Communication Testing (Optional)

If you set up serial connection:

1. **Open terminal program**:
   - Set baud rate to 38400
   - Connect to appropriate COM port

2. **Send test commands**:
   ```
   S     → Request status (should return mode, light, position, status bits)
   M0    → Set normal mode
   M1    → Set vet mode
   M3    → Set night mode
   ```

3. **Observe responses**:
   - Commands return formatted data starting with 'A'
   - Example: `AM0L512P512S3\n` (Mode 0, Light 512, Position 512, Status bits 3)

### Functional Testing

1. **RFID reading test**:
   - Present known RFID tag to reader area
   - Should hear beep
   - Green latch should unlock briefly (5 seconds)
   - Verify entrance opens

2. **Light sensor test**:
   - Switch to night mode
   - Cover light sensor (simulate darkness)
   - Green LED should turn on (indicating lock)
   - Remove cover → LED off after delay

3. **Flap position test** (if FLAP_POT enabled):
   - Manually move flap
   - Monitor serial output for position changes
   - Verify position values change appropriately

4. **Latch operation test**:
   - Listen/feel for solenoid clicks
   - Verify physical latch engagement
   - Test both entrance and exit latches

### Troubleshooting Programming Issues

If programming fails or device doesn't work:

1. **Programming fails**:
   - Verify all ICSP connections
   - Check voltage levels (should be 5V ±0.25V)
   - Try reducing programming speed in MPLAB X settings
   - Ensure chip is not damaged
   - Try a different programmer if available

2. **Device doesn't power up**:
   - Check power supply voltage with multimeter
   - Verify crystal oscillator connections
   - Check for shorts or incorrect components
   - Verify configuration bits are set correctly

3. **RFID not working**:
   - Check RA2 connection to RFID demodulator
   - Verify RFID excitation frequency output on RC2
   - Check L293D driver connections and power

4. **LEDs don't work**:
   - Verify RB4 (Red) and RB5 (Green) connections
   - Check LED polarity (if replaced)
   - Test with multimeter in diode mode

5. **Buttons don't respond**:
   - Check RB6 and RB7 connections
   - Verify pull-up resistors are enabled in code
   - Test button continuity with multimeter

6. **Serial communication not working**:
   - Verify baud rate matches (38400)
   - Check TX/RX connections (crossed correctly)
   - Ensure ground connection between devices
   - Test with loopback (TX→RX) on adapter

---

## Safety and Precautions

### Electrical Safety

⚠️ **WARNING**: Working with electronics requires care and attention.

1. **Before any work**:
   - Disconnect ALL power sources
   - Remove batteries
   - Wait 30 seconds for capacitors to discharge
   - Verify 0V with multimeter

2. **Electrostatic discharge (ESD) protection**:
   - Wear anti-static wrist strap
   - Work on anti-static mat
   - Touch grounded metal before handling chips
   - Store components in anti-static bags

3. **Power supply**:
   - Use regulated 5V power supply only
   - Never exceed 5.5V (can damage PIC)
   - Check polarity before connecting
   - Use current-limited supply (max 500mA)

4. **Soldering safety** (if adding ICSP header):
   - Use appropriate temperature (300-350°C)
   - Work in ventilated area
   - Wear safety glasses
   - Allow joints to cool before handling

### Pet Safety

⚠️ **IMPORTANT**: This controls your pet's access.

1. **Testing requirements**:
   - Thoroughly test all modes before installation
   - Verify RFID reading is reliable
   - Ensure latches operate smoothly
   - Test emergency manual override

2. **Installation considerations**:
   - Always provide alternative access during testing
   - Monitor behavior for first 24-48 hours
   - Keep original firmware backed up
   - Document any hardware modifications

3. **Failure modes**:
   - Consider what happens if power fails
   - Ensure flap can be manually opened
   - Have backup plan for cat access

### Legal and Warranty

⚠️ **DISCLAIMER**:

- This is alternative firmware - **USE AT YOUR OWN RISK**
- Modifying the device likely **VOIDS WARRANTY**
- Author is **NOT RESPONSIBLE** for any damage
- Test thoroughly before relying on device
- Keep original firmware if possible

### Best Practices

1. **Documentation**:
   - Take photos before disassembly
   - Label all wires before disconnecting
   - Document any modifications made
   - Keep notes on programming dates and versions

2. **Backup**:
   - Read and save original firmware before programming
   - In MPLAB X: Right-click device → Read Device Memory
   - Save as .hex file for restoration

3. **Version control**:
   - Note the firmware version programmed
   - Keep track of any custom modifications
   - Document configuration settings

---

## Additional Resources

### Official Documentation

- **PIC16F886 Datasheet**: [Microchip website](https://www.microchip.com/wwwproducts/en/PIC16F886)
- **MPLAB X User's Guide**: Included with MPLAB X installation
- **XC8 Compiler Guide**: Included with XC8 installation
- **PICkit 3 User's Guide**: Available on Microchip website

### Community Resources

- **Microchip Forums**: https://www.microchip.com/forums/
- **Project Repository**: https://github.com/CJFWeatherhead/PetSafe-CatFlap
- **PIC Microcontroller Tutorials**: Many available online for beginners

### Getting Help

If you encounter issues:

1. **Check troubleshooting section above**
2. **Search existing GitHub issues** in the repository
3. **Open a new issue** with:
   - Detailed description of problem
   - Photos of hardware setup
   - Error messages or unexpected behavior
   - Steps to reproduce

### Recommended Learning Path for Beginners

If this is your first embedded project:

1. **Start with breadboard testing**:
   - Get a PIC16F886 chip
   - Build simple LED blink circuit
   - Practice programming before working on actual device

2. **Learn fundamentals**:
   - Basic electronics (voltage, current, resistance)
   - Digital logic (HIGH/LOW states)
   - C programming basics
   - Datasheets reading

3. **Practice with examples**:
   - MPLAB X includes many example projects
   - Start with simple I/O examples
   - Progress to interrupts and timers

4. **Then tackle this project**:
   - You'll have confidence and skills
   - Understand what the code does
   - Troubleshoot issues effectively

---

## Quick Reference: Programming Checklist

Use this checklist when programming:

- [ ] Repository cloned and project opened in MPLAB X
- [ ] XC8 compiler installed and detected
- [ ] Project builds successfully (BUILD SUCCESSFUL message)
- [ ] Programmer connected to computer
- [ ] Programmer detected in MPLAB X
- [ ] Power disconnected from target device
- [ ] ICSP connections verified (VPP, VDD, VSS, PGD, PGC)
- [ ] Anti-static precautions taken
- [ ] Configuration: XC8_PIC16F886 selected
- [ ] Optional: FLAP_POT enabled/disabled as desired
- [ ] Ready to program!
- [ ] Press F6 or "Make and Program Device Main Project"
- [ ] Wait for "Programming/Verify complete" message
- [ ] Disconnect programmer
- [ ] Reconnect power to device
- [ ] Test basic functionality (LEDs, buttons, RFID)
- [ ] Install in cat flap
- [ ] Monitor operation for 24-48 hours
- [ ] Success! 🐱

---

## Conclusion

Congratulations on deploying the alternative firmware! You've learned about PIC microcontrollers, embedded programming, and hardware interfacing. This project demonstrates the power of open-source hardware and software working together.

Remember:
- **Document your work** for future reference
- **Test thoroughly** before relying on the device
- **Share your experience** with the community
- **Contribute improvements** back to the project

Happy hacking, and may your cats enjoy their upgraded cat flap! 🐈

---

**Last Updated**: December 2025  
**Firmware Version**: As of commit f898cc3  
**Author**: CJFWeatherhead  
**Contributors**: Community contributions welcome!
