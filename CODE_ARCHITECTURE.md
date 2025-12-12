# Code Architecture and Documentation

This document provides a comprehensive overview of the PetSafe Cat Flap firmware architecture, code organization, and implementation details.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Code Structure](#code-structure)
3. [Module Documentation](#module-documentation)
4. [Data Flow](#data-flow)
5. [Hardware Abstraction](#hardware-abstraction)
6. [Memory Usage](#memory-usage)
7. [Timing and Interrupts](#timing-and-interrupts)
8. [Communication Protocols](#communication-protocols)

---

## Project Overview

### Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         main.c                               │
│                    (Main Application)                        │
│  ┌────────────────────────────────────────────────────────┐ │
│  │  Main Loop:                                            │ │
│  │  - Mode management                                     │ │
│  │  - Button handling                                     │ │
│  │  - RFID reading                                        │ │
│  │  - Serial communication                                │ │
│  │  - Sensor monitoring                                   │ │
│  └────────────────────────────────────────────────────────┘ │
└──────────────────┬──────────────────────────────────────────┘
                   │
    ┌──────────────┼──────────────┬──────────────┬──────────┐
    │              │              │              │          │
┌───▼────┐    ┌───▼────┐    ┌───▼────┐    ┌───▼────┐ ┌──▼───┐
│ cat.c  │    │ rfid.c │    │serial.c│    │periph- │ │inter-│
│        │    │        │    │        │    │erials.c│ │rupts.│
│ EEPROM │    │  RFID  │    │  UART  │    │  I/O   │ │  c   │
│Storage │    │ Reader │    │  Comm  │    │ Control│ │      │
└────────┘    └────────┘    └────────┘    └────────┘ └──────┘
     │             │             │              │          │
     └─────────────┴─────────────┴──────────────┴──────────┘
                              │
                    ┌─────────▼──────────┐
                    │  configuration_    │
                    │     bits.c         │
                    │  (PIC Config)      │
                    └────────────────────┘
```

### System Components

1. **Main Application** (`main.c`): Core application logic
2. **Cat Management** (`cat.c/h`): EEPROM storage for RFID tags
3. **RFID Reader** (`rfid.c/h`): 134.2 kHz RFID tag reading
4. **Serial Communication** (`serial.c/h`): UART interface
5. **Peripherals** (`peripherials.c/h`): Hardware I/O control
6. **Interrupts** (`interrupts.c/h`): Timer and UART interrupts
7. **User Initialization** (`user.c/h`): System initialization
8. **Configuration** (`configuration_bits.c`): PIC configuration bits

---

## Code Structure

### File Organization

```
PetSafe-CatFlap/
├── main.c                    # Main application logic
├── cat.c / cat.h             # Cat RFID tag storage (EEPROM)
├── rfid.c / rfid.h           # RFID reader implementation
├── serial.c / serial.h       # UART serial communication
├── peripherials.c / .h       # Hardware I/O control
├── interrupts.c / .h         # Interrupt service routines
├── user.c / user.h           # System initialization
├── configuration_bits.c      # PIC configuration
├── Makefile                  # Build configuration
└── nbproject/                # MPLAB X project files
```

### Compilation Units

Each module is designed to be independently compilable and testable:

- **Header files** (`.h`): Interface definitions, macros, type definitions
- **Source files** (`.c`): Implementation of functions
- **Configuration**: Hardware-specific settings

---

## Module Documentation

### 1. Main Application (`main.c`)

**Purpose**: Central control logic for the cat flap firmware.

#### Key Components

##### Operating Modes

```c
#define MODE_NORMAL 0   // Standard RFID operation
#define MODE_VET 1      // Keep cat inside
#define MODE_CLOSED 2   // Both latches locked
#define MODE_NIGHT 3    // Light-sensor based locking
#define MODE_LEARN 4    // Learn new RFID tags
#define MODE_CLEAR 5    // Clear all stored tags
#define MODE_OPEN 6     // Free access (no locks)
```

**Mode Descriptions**:

- **Normal Mode**: Entrance locked, exit open. RFID unlocks entrance temporarily.
- **Vet Mode**: Both entrance and exit locked. Cat cannot leave.
- **Closed Mode**: Both locked. No access.
- **Night Mode**: Automatically locks based on light sensor reading.
- **Learn Mode**: Accepts new RFID tags for 30 seconds.
- **Clear Mode**: Erases all stored RFID tags from EEPROM.
- **Open Mode**: Both latches open. Free access.

##### State Variables

```c
static uint8_t opMode = MODE_NORMAL;    // Current operating mode
static bool outLocked = false;          // Exit latch state
static bool inLocked = false;           // Entrance latch state
static uint16_t light = 0;              // Light sensor value
static uint16_t lightThd = 0;           // Light threshold
```

##### Functions

**`void switchMode(uint8_t mode)`**
- Changes operating mode and configures latches accordingly
- Updates LED indicators
- Parameters: `mode` - New mode (0-6)

**`uint8_t handleButtons(ms_t *time)`**
- Handles button press detection and timing
- Supports single button and both-button combinations
- Returns: Button press type (GREEN_PRESS, RED_PRESS, BOTH_PRESS)
- Parameters: `time` - Pointer to store press duration

**`void learnCat(void)`**
- Enters learn mode for 30 seconds
- Reads RFID tags and saves to EEPROM
- Blinks green LED during learning
- Beeps on successful save

**`void handleSerial(void)`**
- Processes incoming serial commands
- Supports status queries, mode changes, configuration
- Command format: Single character followed by parameters

**`void printStatus(void)`**
- Sends current status via serial
- Format: `AM[mode]L[light]P[position]S[status]\n`

**`void printCat(Cat* c)`**
- Sends cat RFID ID via serial when detected
- Format: `E[6-byte-ID]\n`

**`uint16_t buildStatusBits(void)`**
- Builds bit pattern representing device status
- Bit 0: Entrance locked
- Bit 1: Exit locked
- Bits 2-3: Reserved (formerly used for flap position)

##### Main Loop Flow

```c
void main(void) {
    InitApp();                    // Initialize hardware
    loadConfiguration();          // Load settings from EEPROM
    switchMode(MODE_NORMAL);      // Start in normal mode
    
    while(1) {
        updateSensors();          // Read light sensor, flap position
        
        switch(opMode) {
            case MODE_NORMAL:
                // Read RFID, unlock if valid
                break;
            case MODE_VET:
                // Keep locked, blink red LED
                break;
            case MODE_NIGHT:
                // Auto-lock based on light
                break;
            // ... other modes
        }
        
        handleButtons();          // Process button input
        handleSerial();           // Process serial commands
    }
}
```

**Key Timing Values**:
- `OPEN_TIME`: 5000ms - Duration to keep door open after RFID match
- `LIGHT_READ_PERIOD`: 5000ms - Interval between light sensor reads

---

### 2. Cat Management (`cat.c/h`)

**Purpose**: Manages RFID tag storage in EEPROM.

#### EEPROM Memory Layout

```
Address Range    | Content
─────────────────┼────────────────────────────
0x00 - 0x7F      | Configuration parameters (128 bytes)
  0x00-0x01      |   Light threshold (uint16_t)
  0x02-0x7F      |   Reserved for future config
─────────────────┼────────────────────────────
0x80 - 0xFF      | Cat RFID storage (128 bytes)
                 |   16 slots × 8 bytes each
```

#### Data Structures

```c
typedef struct {
    uint16_t crc;   // RFID tag CRC (2 bytes)
    uint8_t id[6];  // RFID tag ID (6 bytes)
} Cat;              // Total: 8 bytes per cat
```

#### Functions

**`uint16_t getConfiguration(uint8_t cfg)`**
- Reads a 16-bit configuration value from EEPROM
- Parameters: `cfg` - Configuration index
- Returns: Configuration value
- Example: `lightThd = getConfiguration(LIGHT_CFG);`

**`void setConfiguration(uint8_t cfg, uint16_t value)`**
- Writes a 16-bit configuration value to EEPROM
- Parameters: 
  - `cfg` - Configuration index
  - `value` - Value to write
- Example: `setConfiguration(LIGHT_CFG, 512);`

**`void getCat(Cat* cat, uint8_t slot)`**
- Retrieves cat data from specific EEPROM slot
- Parameters:
  - `cat` - Pointer to Cat structure
  - `slot` - Slot number (0-15)
- If slot is invalid or empty, sets cat->crc to 0

**`uint8_t saveCat(Cat* cat)`**
- Saves cat RFID data to first available EEPROM slot
- Parameters: `cat` - Pointer to Cat structure with CRC and ID
- Returns: Slot number (1-16) if saved, 0 if no space or already exists
- Behavior:
  - Finds first empty slot (CRC = 0)
  - If tag already exists, returns its slot number
  - If EEPROM full, returns 0

**`bool catExists(Cat* cat, uint16_t* otherCrc)`**
- Checks if cat RFID tag exists in EEPROM
- Parameters:
  - `cat` - Pointer to Cat structure (CRC checked)
  - `otherCrc` - Secondary CRC to verify
- Returns: `true` if found and CRC matches, `false` otherwise
- Side effect: Loads cat ID into structure if found

**`void clearCats(void)`**
- Erases all stored cat RFID tags
- Only clears CRC bytes (sets to 0x00)
- ID bytes remain but are ignored (CRC=0 means empty slot)
- Beeps 5 times to confirm operation

#### Usage Example

```c
// Learn a new cat
Cat newCat;
uint16_t crcRead, crcComputed;
if (readRFID(newCat.id, 6, &newCat.crc, &crcRead) == 0) {
    uint8_t slot = saveCat(&newCat);
    if (slot > 0) {
        beep();  // Success
    }
}

// Check if cat is authorized
Cat detectedCat;
uint16_t crcRead;
if (readRFID(detectedCat.id, 6, &detectedCat.crc, &crcRead) == 0) {
    if (catExists(&detectedCat, &crcRead)) {
        // Open door
        lockGreenLatch(false);
    }
}
```

---

### 3. RFID Reader (`rfid.c/h`)

**Purpose**: Implements 134.2 kHz RFID tag reading using EM4100 protocol.

#### RFID Communication Overview

The RFID system uses:
- **Frequency**: 134.2 kHz carrier
- **Modulation**: Amplitude Shift Keying (ASK)
- **Protocol**: EM4100 compatible
- **Data Rate**: ~400 bits/second

#### Key Components

**PWM Generation** (for 134.2 kHz carrier):
```c
void setRFIDPWM(bool on)
```
- Configures CCP1 module for PWM output on RC2
- 50% duty cycle at 134.2 kHz
- Powers L293 driver and LM324 op-amp
- Enables ADC for signal reading

**Demodulation and Bit Reading**:
```c
bool readBit()
```
- Reads demodulated RFID signal via ADC
- Samples during specific time windows
- Uses Manchester encoding detection
- Threshold at 512 (mid-range of 10-bit ADC)

**Synchronization**:
```c
uint8_t syncRFID(void)
```
- Waits for RFID tag header (9 consecutive '1' bits)
- Timeout: 100ms
- Returns: 0 on success, NO_HEADER on timeout

**Data Reading**:
```c
uint8_t readRFIDByte(uint8_t* d)
```
- Reads 8 data bits plus start bit
- Accumulates bits LSB-first
- Returns: 0 on success, error code otherwise

**CRC Calculation**:
```c
uint16_t crc(uint8_t *p, uint8_t len)
```
- Implements CRC-CCITT (0x1021 polynomial)
- Bit-reversed output
- Verifies data integrity

#### Main Function

```c
uint8_t readRFID(uint8_t* id, uint8_t len, uint16_t* crcComputed, uint16_t* crcRead)
```

**Process**:
1. Enable RFID PWM excitation
2. Wait for tag synchronization (header detection)
3. Read 10 bytes of data:
   - 6 bytes: Tag ID
   - 2 bytes: Reserved/data
   - 2 bytes: CRC
4. Compute CRC and compare with received CRC
5. Disable RFID PWM
6. Return status code

**Return Values**:
- `0`: Success
- `NO_CARRIER`: No signal detected
- `NO_HEADER`: Sync header not found
- `BAD_START`: Invalid start bit
- `BAD_CRC`: CRC mismatch

#### Error Codes

```c
#define NO_CARRIER 1
#define NO_HEADER 2
#define BAD_START 3
#define BAD_CRC 4
```

---

### 4. Serial Communication (`serial.c/h`)

**Purpose**: UART serial interface for external control and monitoring.

#### UART Configuration

```c
#define BAUD_RATE 9600
#define DIVIDER ((int)(_XTAL_FREQ/(16UL * BAUD_RATE) -1))
```

**Settings**:
- **Baud Rate**: 9600 bps (reduced from 38400 for improved reliability with crystal skew)
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: None
- **Error Handling**: Automatic detection and recovery from framing/overrun errors

#### Ring Buffer Implementation

```c
#define SER_BUFFER 16

struct RingBuffer {
    uint8_t rIndex;     // Read index
    uint8_t uIndex;     // Write index (updated by ISR)
    uint8_t buffer[SER_BUFFER];
};
```

**Buffer Management**:
- Circular buffer for received data
- Written by interrupt service routine
- Read by main application
- Capacity: 16 bytes
- Overflow protection with error counting

#### Error Tracking

```c
struct UartErrors {
    uint8_t framingErrors;   // Count of framing errors
    uint8_t overrunErrors;   // Count of overrun errors
    uint8_t bufferOverflows; // Count of ring buffer overflows
};
```

**Error Handling**:
- **Framing Errors**: Detected when FERR flag is set, byte discarded and CREN toggled
- **Overrun Errors**: Detected when OERR flag is set, CREN reset to clear
- **Buffer Overflows**: Tracked when ring buffer is full, oldest data protected
- All errors automatically counted for diagnostics

#### Functions

**`void initSerial(void)`**
- Configures UART hardware
- Sets baud rate divider (9600 bps)
- Enables continuous reception
- Enables RX interrupt
- Initializes ring buffer
- Resets error counters
- Prints startup banner with firmware information

**`void putch(char byte)`**
- Transmits single character
- Blocks until TXIF flag set
- Used by `printf()` for formatted output

**`void putShort(uint16_t v)`**
- Sends 16-bit value as two bytes
- Little-endian format (LSB first)

**`uint8_t getByte(uint8_t* v)`**
- Reads single byte from ring buffer
- Timeout: 5ms
- Returns: 0 on success, 1 on timeout
- Advances read pointer

**`uint8_t getShort(uint16_t* v)`**
- Reads 16-bit value as two bytes
- Little-endian format
- Timeout: 5ms (per byte)
- Returns: 0 on success, 1 on timeout

**`bool byteAvail(void)`**
- Checks if data available in buffer
- Returns: `true` if data present
- Non-blocking

#### Serial Protocol

**Commands** (sent TO device):

| Command | Format | Description |
|---------|--------|-------------|
| `S` | `S` | Request status |
| `C` | `C[R/S][index][value]` | Read/Set configuration |
| `M` | `M[mode]` | Set operating mode |

**Responses** (sent FROM device):

| Response | Format | Description |
|----------|--------|-------------|
| Status | `AM[mode]L[light]P[pos]S[bits]\n` | Current status |
| Config | `AC[index]V[value]\n` | Configuration value |
| Mode | `AM[mode]\n` | Mode confirmation |
| Error | `AE\n` | Error occurred |
| Cat | `E[6-byte-ID]\n` | Cat detected |

#### Usage Example

```c
// Request status
putch('S');
// Response: AM0L512P512S3\n

// Set night mode
putch('M');
putch(3);
// Response: AM3\n

// Read light threshold config
putch('C');
putch('R');  // Read
putch(LIGHT_CFG);
// Response: AC0V512\n
```

---

### 5. Peripherals Control (`peripherials.c/h`)

**Purpose**: Hardware I/O abstraction and control.

#### I/O Configuration

**Pin Definitions** (from `peripherials.h`):

```c
// PORTA
#define RFID_STREAM      PORTAbits.RA2  // RFID demodulated input
#define RED_LOCK         PORTAbits.RA5  // Red solenoid control

// PORTB
#define DOOR_SWITCH      PORTBbits.RB0  // Door sensor (pull-up)
#define COMMON_LOCK      PORTBbits.RB1  // Solenoid common
#define CL_GL_ENABLE     PORTBbits.RB2  // L293D enable 1/2
#define LM324_PWR        PORTBbits.RB3  // Op-amp power
#define RED_LED          PORTBbits.RB4  // Red LED
#define GREEN_LED        PORTBbits.RB5  // Green LED
#define RED_BTN          PORTBbits.RB6  // Red button (pull-up)
#define GREEN_BTN        PORTBbits.RB7  // Green button (pull-up)

// PORTC
#define RFID_RL_ENABLE   PORTCbits.RC0  // L293D enable 3/4
#define BUZZER           PORTCbits.RC1  // Buzzer control
#define RFID_EXCT        PORTCbits.RC2  // RFID excitation (PWM)
#define L293_LOGIC       PORTCbits.RC3  // L293D logic power
#define GREEN_LOCK       PORTCbits.RC4  // Green solenoid control
#define SER_INT          PORTCbits.RC5  // Serial header (unused)
// RC6/RC7: UART TX/RX
```

#### Functions

**`void initPeripherials(void)`**
- Configures all I/O pins (input/output direction)
- Sets up analog inputs (light sensor, flap position)
- Enables weak pull-ups on buttons
- Configures Timer1 for millisecond counter
- Enables interrupts

**Analog Configuration**:
```c
ANSEL = 0xD;   // RA0, RA2, RA3 analog
```

**`uint16_t getLightSensor(void)`**
- Reads light sensor on RA0 (AN0)
- Returns: 10-bit ADC value (0-1023)
- Higher value = darker
- Process:
  1. Select ADC channel 0
  2. Wait for acquisition (1ms)
  3. Start conversion
  4. Wait for completion
  5. Return 10-bit result

**`void beep(void)`**
- Generates audible beep
- Toggles buzzer at ~1 kHz for ~200ms
- Used for feedback (tag learned, mode change, etc.)

**`bool lockGreenLatch(bool lock)`**
- Controls entrance latch (green)
- Parameters: `lock` - true to lock, false to unlock
- Process:
  1. Enable L293D channels
  2. Set solenoid direction
  3. Power for 500ms
  4. Disable power
  5. Set outputs to safe state
- Returns: Lock state

**`bool lockRedLatch(bool lock)`**
- Controls exit latch (red)
- Similar process to green latch
- Also disables RFID excitation during operation

#### Timer1 Configuration

```c
// Prescaler: 1:4
// Fosc = 19.6 MHz
// Timer clock = Fosc/4 / 4 = 1.225 MHz
// Overflow every 1ms: load value = 0xFB37
#define TMR1_H_PRES 0xFB
#define TMR1_L_PRES 0x37
```

---

### 6. Interrupts (`interrupts.c/h`)

**Purpose**: Handles time-critical events.

#### Interrupt Service Routine

```c
void __interrupt() isr(void)
```

**Handles Two Interrupts**:

1. **Timer1 Overflow** (every 1ms):
   - Reloads timer preset values
   - Increments millisecond counter
   - Clears TMR1IF flag

2. **UART Receive**:
   - Reads byte from RCREG
   - Stores in ring buffer
   - Advances write index
   - Wraps at buffer end
   - Clears RCIF flag

#### Millisecond Counter

```c
typedef uint32_t ms_t;
static volatile ms_t millisValue = 0;

ms_t millis(void) {
    return millisValue;
}
```

**Usage**:
- Non-blocking delays
- Timeout detection
- Periodic task scheduling
- Button press timing

**Example**:
```c
ms_t start = millis();
while ((millis() - start) < 5000) {
    // Wait up to 5 seconds
}
```

---

### 7. User Initialization (`user.c/h`)

**Purpose**: High-level system initialization.

```c
void InitApp(void) {
    initPeripherials();   // Hardware I/O
    initSerial();         // UART communication
}
```

Simple wrapper that orchestrates initialization sequence.

---

### 8. Configuration Bits (`configuration_bits.c`)

**Purpose**: PIC16F886 hardware configuration.

```c
// CONFIG1
#pragma config FOSC = HS        // High-Speed crystal
#pragma config WDTE = OFF       // Watchdog timer OFF
#pragma config PWRTE = OFF      // Power-up timer OFF
#pragma config MCLRE = ON       // MCLR pin enabled
#pragma config CP = OFF         // Code protection OFF
#pragma config CPD = OFF        // Data protection OFF
#pragma config BOREN = OFF      // Brown-out reset OFF
#pragma config IESO = OFF       // Int/Ext switchover OFF
#pragma config FCMEN = ON       // Fail-safe clock monitor ON
#pragma config LVP = OFF        // Low-voltage programming OFF

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out at 4.0V
#pragma config WRT = OFF        // Flash write protection OFF
#pragma config DEBUG = OFF      // Debugger disabled
```

**Key Settings**:
- **Crystal**: 19.6 MHz High-Speed
- **Watchdog**: Disabled (no auto-reset)
- **Code Protection**: Disabled (allows read-back)
- **Programming**: High-voltage only (LVP=OFF)

---

## Data Flow

### RFID Authentication Flow

```
┌─────────────────────────────────────────────────┐
│ 1. Cat approaches with RFID collar              │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 2. main.c calls readRFID()                      │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 3. rfid.c enables 134.2kHz PWM excitation       │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 4. rfid.c waits for sync header (9×'1')         │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 5. rfid.c reads 10 bytes (ID + CRC)             │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 6. rfid.c computes and verifies CRC             │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 7. main.c calls catExists() with CRC+ID         │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│ 8. cat.c searches EEPROM slots (16 max)         │
└────────────────┬────────────────────────────────┘
                 │
        ┌────────┴────────┐
        │                 │
┌───────▼──────┐   ┌──────▼────────┐
│   Match!     │   │  No Match     │
└───────┬──────┘   └──────┬────────┘
        │                 │
┌───────▼──────────────────▼────────┐
│ 9. main.c controls latches        │
│    Match: unlock, beep, open 5s   │
│    No match: stay locked           │
└────────────────────────────────────┘
```

### Serial Command Flow

```
External Device → UART RX (RC7) → Interrupt → Ring Buffer
                                      ↓
            main.c handleSerial() ← getByte()
                    ↓
            Command Parser
            ├─ 'S' → printStatus()
            ├─ 'C' → getConfiguration() / setConfiguration()
            └─ 'M' → switchMode()
                    ↓
            Response via putch()
                    ↓
            UART TX (RC6) → External Device
```

---

## Hardware Abstraction

### Layer Architecture

```
┌─────────────────────────────────────────────┐
│         Application Layer (main.c)          │
│  - Business logic                           │
│  - Mode management                          │
│  - Decision making                          │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│      Hardware Abstraction (*.c/h)           │
│  - cat.c       → EEPROM operations          │
│  - rfid.c      → RFID protocol              │
│  - serial.c    → UART communication         │
│  - peripherials.c → I/O, ADC, timers        │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│         Hardware Layer (PIC16F886)          │
│  - Registers (PORTA, PORTB, PORTC, etc.)    │
│  - Peripherals (ADC, UART, Timer, PWM)      │
│  - Interrupts                               │
└─────────────────────────────────────────────┘
```

### Benefits

- **Portability**: Easy to port to different microcontroller
- **Testing**: Can mock hardware layer for unit tests
- **Maintenance**: Changes isolated to specific modules
- **Clarity**: Clear separation of concerns

---

## Memory Usage

### Program Memory (Flash)

- **Total Available**: 8K words (14-bit instructions)
- **Typical Usage**: ~3-5K words (depends on optimizations)
- **Storage**: Executable code only

### Data Memory (RAM)

- **Total Available**: 368 bytes
- **Usage Breakdown**:
  - Global variables: ~50 bytes
  - Stack: ~50 bytes
  - Serial buffer: 16 bytes
  - Local variables: ~50 bytes
  - Remaining: ~200 bytes

**RAM Optimization Tips**:
- Use local variables (auto-allocated on stack)
- Avoid large arrays in RAM
- Use `const` for read-only data (stored in Flash)
- Reuse variables when possible

### EEPROM Memory

- **Total Available**: 256 bytes
- **Layout**:
  - 0x00-0x7F (128 bytes): Configuration
  - 0x80-0xFF (128 bytes): Cat storage (16 slots × 8 bytes)

---

## Timing and Interrupts

### Interrupt Priority

1. **Timer1**: 1ms tick (highest priority in code)
2. **UART RX**: Incoming serial data

Both share single ISR, checked in order.

### Critical Timing

- **RFID bit reading**: ~2.5ms per bit (400 bps)
- **Solenoid activation**: 500ms hold time
- **Beep duration**: ~200ms
- **RFID timeout**: 100ms for sync
- **Serial timeout**: 5ms per byte

### Timing Functions

```c
ms_t start = millis();
while ((millis() - start) < timeout) {
    // Wait or poll
}
```

**Advantages**:
- Non-blocking
- Precise (1ms resolution)
- Doesn't stop interrupts
- Can monitor multiple timeouts

---

## Communication Protocols

### RFID Protocol (EM4100)

**Physical Layer**:
- Carrier: 134.2 kHz
- Modulation: Amplitude Shift Keying (ASK)
- Bit rate: ~400 bps

**Data Format**:
```
Header: 9 consecutive '1' bits
Data:   10 bytes
  - Byte 0-5: Tag ID (6 bytes)
  - Byte 6-7: Reserved
  - Byte 8-9: CRC-CCITT (little-endian, bit-reversed)
```

**Encoding**: Manchester
- '0' = high-to-low transition
- '1' = low-to-high transition

### Serial Protocol

**Command Structure**:
```
Command: 1 byte
Parameters: variable (0-3 bytes)
```

**Response Structure**:
```
Prefix: 'A' (acknowledge) or 'E' (error)
Type: 1 byte command type
Data: variable
Terminator: '\n'
```

**Example Transactions**:
```
→ S                  # Request status
← AM0L512P512S3\n    # Mode 0, Light 512, Pos 512, Status 3

→ M03                # Set mode 3 (night)
← AM3\n              # Confirm mode 3

→ CR00               # Read config 0
← AC0V512\n          # Config 0, Value 512
```

---

## Code Conventions

### Naming

- **Functions**: `camelCase` → `readRFID()`, `getLightSensor()`
- **Variables**: `camelCase` → `opMode`, `lightThd`
- **Macros**: `UPPER_CASE` → `MODE_NORMAL`, `OPEN_TIME`
- **Types**: `PascalCase` → `Cat`, `RingBuffer`

### File Headers

Standard Microchip header in most files. Key info:
- File name
- Author (mdonze)
- Creation date
- Purpose/description

### Comments

- Function headers: Doxygen-style
- Inline: Explain "why", not "what"
- Complex logic: Step-by-step explanation

---

## Development Guidelines

### Adding New Features

1. **Identify module**: Which file should contain the feature?
2. **Define interface**: Add function prototypes to header
3. **Implement**: Add function to source file
4. **Test**: Verify on hardware
5. **Document**: Update this document

### Modifying Hardware

1. **Check pin availability**: See `peripherials.h`
2. **Update macros**: Define new pin names
3. **Update initialization**: Configure in `initPeripherials()`
4. **Update build flags**: Add to Makefile if needed

### Debugging Tips

1. **Serial output**: Use `printf()` for debugging
2. **LED indicators**: Quick visual feedback
3. **MPLAB X simulator**: Step through code
4. **Oscilloscope**: Check PWM, timing
5. **Logic analyzer**: Verify RFID signals

---

## Build System

### Makefile

MPLAB X generates Makefile in `nbproject/` directory.

**Key files**:
- `Makefile-XC8_PIC16F886.mk`: Build rules
- `Makefile-variables.mk`: Project variables
- `configurations.xml`: Project configuration

### Compiler Options

**XC8 Compiler**:
- Optimization: Free mode (O1)
- Warnings: All enabled
- Target: PIC16F886

### Build Macros

**`_XTAL_FREQ`**: 19600000 (19.6 MHz)
- Defines system clock frequency
- Used by delay macros
  - Adds RA1 as analog input
  - Includes flap position functions
  - Adds status bits for flap state

---

## Performance Considerations

### Optimization Areas

1. **RFID Reading**: Most time-critical
   - Tight timing loops
   - Minimal interrupts during read
   - ADC sampling synchronized

2. **Solenoid Control**: Power management
   - 500ms activation time
   - Returns to safe state
   - Prevents simultaneous activation

3. **Serial Communication**: Buffered
   - Ring buffer prevents data loss
   - Interrupt-driven receive
   - Non-blocking transmit

### Power Consumption

**Active mode**:
- MCU: ~5mA @ 20MHz
- RFID excitation: ~100mA during read
- Solenoids: ~500mA during activation
- LEDs: ~20mA each when on

**Power saving opportunities**:
- Disable RFID excitation when not reading
- Turn off op-amp power when idle
- Use sleep mode (not currently implemented)

---

## Troubleshooting Guide

### Common Issues

**RFID not reading**:
- Check PWM frequency (134.2 kHz)
- Verify op-amp power (LM324_PWR)
- Check L293D enable signals
- Verify ADC threshold (512)

**Solenoids not working**:
- Check L293D power (L293_LOGIC)
- Verify enable signals
- Check direction pins
- Ensure 500ms activation time

**Serial not responding**:
- Verify baud rate (9600)
- Check TX/RX connections (crossed?)
- Ensure interrupts enabled
- Check ring buffer overflow
- Monitor UART error counters (framing, overrun, buffer overflow)

**Random resets**:
- Check power supply stability
- Verify brown-out settings
- Check for stack overflow
- Monitor RAM usage

---

## Future Enhancements

### Potential Features

1. **Network connectivity**: ESP8266/ESP32 module via UART
2. **Real-time clock**: Track entry/exit times
3. **Weight sensor**: Verify correct pet
4. **Camera integration**: Photo capture on entry
5. **Battery backup**: Maintain operation during power loss
6. **Sleep mode**: Reduce power consumption
7. **Extended logging**: Store more events in EEPROM
8. **Multi-frequency RFID**: Support different tag types

### Code Improvements

1. **Unit tests**: Automated testing framework
2. **State machine**: Formalize mode transitions
3. **Error recovery**: More robust error handling
4. **Bootloader**: Over-the-air firmware updates
5. **Configuration UI**: Mobile app for settings

---

## References

### Datasheets

- [PIC16F886 Datasheet](https://www.microchip.com/wwwproducts/en/PIC16F886)
- [XC8 Compiler Guide](https://www.microchip.com/xc8)
- [L293D Motor Driver](http://www.ti.com/product/L293D)
- [LM324 Op-Amp](http://www.ti.com/product/LM324)
- [EM4100 RFID Protocol](https://www.emmicroelectronic.com/)

### Standards

- CRC-CCITT: Polynomial 0x1021
- Manchester Encoding: IEEE 802.3
- UART: RS-232 standard

---

**Document Version**: 1.0  
**Last Updated**: December 2025  
**Author**: CJFWeatherhead  
**Contributors**: Community contributions welcome!
