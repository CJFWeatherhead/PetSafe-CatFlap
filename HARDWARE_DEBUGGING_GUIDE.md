# PetSafe Cat Flap Hardware Debugging Guide

**A comprehensive toolkit for debugging, tracing, and troubleshooting the PetSafe Pet Porte 100-1023 PCB**

> **⚠️ IMPORTANT**: This guide is designed to help debug different hardware revisions. The original firmware was developed for **revision X4**. Later revisions (X5, X6, etc.) may have different pinouts, component values, or circuit designs that affect LED, buzzer, and UART operation.

## Table of Contents

1. [Overview](#overview)
2. [PIC16F886 Pinout Reference](#pic16f886-pinout-reference)
3. [Complete Pin Mapping Table](#complete-pin-mapping-table)
4. [Hardware Circuit Analysis](#hardware-circuit-analysis)
5. [LED and Buzzer Circuits](#led-and-buzzer-circuits)
6. [UART Serial Communication](#uart-serial-communication)
7. [Hardware Revision Differences](#hardware-revision-differences)
8. [Pin Tracing Methodology](#pin-tracing-methodology)
9. [Debugging Tools and Techniques](#debugging-tools-and-techniques)
10. [Troubleshooting Common Issues](#troubleshooting-common-issues)
11. [Signal Quality and Testing](#signal-quality-and-testing)

---

## Overview

This guide provides comprehensive information for debugging and adapting the PetSafe Cat Flap firmware to different hardware revisions. Use this guide when:

- LEDs don't match expected behavior
- Buzzer doesn't work or has wrong polarity
- UART communication is unreliable
- Adapting firmware to a new hardware revision
- Tracing pins to verify correct connections
- Diagnosing hardware faults

### Safety First

⚡ **ALWAYS disconnect power** before:
- Probing circuits with multimeter
- Making hardware modifications
- Connecting/disconnecting programmers
- Soldering or desoldering components

🔒 **Avoid pet safety issues**:
- Test thoroughly before deployment
- Provide alternative pet access during testing
- Never test on pets without backup access

---

## PIC16F886 Pinout Reference

The PIC16F886 is available in 28-pin PDIP, SOIC, and SSOP packages. This firmware uses the following pinout:

```
                    PIC16F886 (28-pin DIP)
                    ┌────────────────┐
         MCLR/VPP  →│1            28│← RB7/ICSPDAT/PGD (Green Button)
  (Light Sensor) RA0│2            27│← RB6/ICSPCLK/PGC (Red Button)
              RA1/AN│3            26│← RB5 (Green LED)
  (RFID Signal) RA2│4            25│← RB4 (Red LED)
              RA3/AN│5            24│← RB3 (LM324 Power)
                RA4│6            23│← RB2 (CL_GL_ENABLE)
(Red Solenoid) RA5│7            22│← RB1 (COMMON_LOCK)
                GND│8            21│← RB0 (DOOR_SWITCH)
  (19.6MHz Crystal) │9            20│← VDD (+5V)
 (19.6MHz Crystal)  │10           19│← VSS (GND)
 (RFID_RL_ENABLE) RC0│11           18│← RC7/RX (UART Receive)
        (Buzzer) RC1│12           17│← RC6/TX (UART Transmit)
    (RFID PWM) RC2/CCP1│13           16│← RC5 (Serial Header)
   (L293 Logic) RC3│14           15│← RC4 (Green Solenoid)
                    └────────────────┘

ICSP Programming Pins:
  - Pin 1:  MCLR/VPP (Master Clear / Programming Voltage)
  - Pin 27: RB6/ICSPCLK/PGC (Programming Clock)
  - Pin 28: RB7/ICSPDAT/PGD (Programming Data)
  - Pin 20: VDD (+5V Power)
  - Pin 8, 19: GND (Ground)
```

### Pin Function Categories

| Category | Pins | Description |
|----------|------|-------------|
| **Power** | 8 (GND), 19 (VSS), 20 (VDD) | Ground and +5V supply |
| **Crystal** | 9-10 (OSC1/OSC2) | 19.6 MHz crystal oscillator |
| **Programming** | 1 (MCLR), 27 (PGC), 28 (PGD) | ICSP interface |
| **RFID** | 4 (RA2), 13 (RC2) | RFID signal input and PWM excitation |
| **Analog** | 2 (RA0), 3 (RA1), 5 (RA3) | ADC inputs (light sensor, position) |
| **Solenoids** | 7 (RA5), 15 (RC4), 22 (RB1) | Latch control outputs |
| **Motor Driver** | 23 (RB2), 11 (RC0), 14 (RC3), 24 (RB3) | L293D enables and power |
| **User Interface** | 25 (RB4), 26 (RB5), 12 (RC1) | LEDs and buzzer |
| **Buttons** | 27 (RB6), 28 (RB7), 21 (RB0) | User input buttons and door switch |
| **Serial** | 17 (RC6/TX), 18 (RC7/RX) | UART communication |

---

## Complete Pin Mapping Table

This table shows the complete firmware-to-hardware mapping:

| Pin | PIC Name | Firmware Name | Direction | Function | Pull-up | Notes |
|-----|----------|---------------|-----------|----------|---------|-------|
| **PORT A** |
| 1 | MCLR/RA5/VPP | - | Input | Programming/Reset | External | 10kΩ pull-up to VDD recommended |
| 2 | RA0/AN0 | - | Input (Analog) | Light Sensor | No | ADC Channel 0, 0-1023 range |
| 3 | RA1/AN1 | - | Input (Analog) | Flap Position (optional) | No | ADC Channel 1, may be unused |
| 4 | RA2/AN2 | RFID_STREAM | Input (Analog) | RFID Demodulated Signal | No | ADC Channel 2, critical timing |
| 5 | RA3/AN3 | - | Input (Analog) | Reserved | No | May be used in some revisions |
| 6 | RA4/T0CKI | - | Input | Reserved | No | Can be timer input |
| 7 | RA5 | RED_LOCK | Output | Red Solenoid Control | No | Active high, max 500ms |
| **PORT B** |
| 21 | RB0/INT | DOOR_SWITCH | Input | Door Position Sensor | Yes (WPUB) | Active low when door closed |
| 22 | RB1 | COMMON_LOCK | Output | Solenoid Common | No | Direction control for L293D |
| 23 | RB2 | CL_GL_ENABLE | Output | L293D Enable 1/2 | No | Enables channels for locks |
| 24 | RB3 | LM324_PWR | Output | Op-Amp Power Control | No | Powers LM324 for RFID |
| 25 | RB4 | RED_LED | Output | Red LED Indicator | No | **Check polarity on your board** |
| 26 | RB5 | GREEN_LED | Output | Green LED Indicator | No | **Check polarity on your board** |
| 27 | RB6/ICSPCLK/PGC | RED_BTN | Input | Red Button | Yes (WPUB) | Active low, also programming clock |
| 28 | RB7/ICSPDAT/PGD | GREEN_BTN | Input | Green Button | Yes (WPUB) | Active low, also programming data |
| **PORT C** |
| 11 | RC0 | RFID_RL_ENABLE | Output | L293D Enable 3/4 | No | Enables RFID excitation driver |
| 12 | RC1 | BUZZER | Output | Buzzer Control | No | **Check active high/low on your board** |
| 13 | RC2/CCP1 | RFID_EXCT | Output (PWM) | RFID Excitation | No | 134.2 kHz PWM, 50% duty |
| 14 | RC3 | L293_LOGIC | Output | L293D Logic Power | No | Controls power to motor driver |
| 15 | RC4 | GREEN_LOCK | Output | Green Solenoid Control | No | Active high, max 500ms |
| 16 | RC5 | SER_INT | Output | Serial Header I/O | No | Unused, available for debugging |
| 17 | RC6/TX/CK | - | Output | UART Transmit | No | 9600 baud (0.48% error) |
| 18 | RC7/RX/DT | - | Input | UART Receive | No | 9600 baud (0.48% error) |

### Configuration Register Settings

```c
// ANSEL (Analog Select) - Address: 0x188
// Configures pins as analog (1) or digital (0)
ANSEL = 0x0D;  // 0b00001101
// Bit 0 (AN0/RA0): 1 = Analog (Light Sensor)
// Bit 1 (AN1/RA1): 0 = Digital (or unused)
// Bit 2 (AN2/RA2): 1 = Analog (RFID Stream)
// Bit 3 (AN3/RA3): 1 = Analog (Reserved)
// Bits 4-7: 0 = Digital

// TRISA (Tri-State Port A) - Direction
TRISA = 0xDF;  // 0b11011111
// Bit 5 (RA5): 0 = Output (Red Lock)
// All others: 1 = Input

// TRISB (Tri-State Port B) - Direction
TRISB = 0xC1;  // 0b11000001
// Bit 0 (RB0): 1 = Input (Door Switch)
// Bits 1-5: 0 = Output (LEDs, enables, common)
// Bits 6-7: 1 = Input (Buttons)

// TRISC (Tri-State Port C) - Direction
TRISC = 0xC0;  // 0b11000000
// Bits 0-5: 0 = Output (enables, PWM, buzzer, solenoids)
// Bits 6-7: 1 = Input (UART - handled by peripheral)

// WPUB (Weak Pull-Up Port B)
WPUB = 0xC1;   // 0b11000001
// Bit 0 (RB0): 1 = Pull-up enabled (Door Switch)
// Bits 6-7: 1 = Pull-up enabled (Buttons)
// OPTION_REGbits.nRBPU = 0; // Enable weak pull-ups
```

---

## Hardware Circuit Analysis

### L293D Motor Driver Configuration

The L293D is a quadruple half-H driver used to control the solenoid latches.

```
L293D Pinout and Connections:
┌─────────────────────────────┐
│  1 Enable1,2    VCC1    16  │ ← +5V Logic
│  2 Input1       Input4  15  │
│  3 Output1      Output4 14  │
│  4 GND          GND     13  │
│  5 GND          GND     12  │
│  6 Output2      Output3 11  │
│  7 Input2       Input3  10  │
│  8 VCC2         Enable3,4 9 │ ← Motor supply voltage
└─────────────────────────────┘

Channels 1/2 (Green Latch):
  Enable1,2 (Pin 1) ← RB2 (CL_GL_ENABLE)
  Input1 (Pin 2)    ← RB1 (COMMON_LOCK)
  Output1 (Pin 3)   → Solenoid Common
  Input2 (Pin 7)    ← RC4 (GREEN_LOCK)
  Output2 (Pin 6)   → Green Solenoid

Channels 3/4 (Red Latch + RFID):
  Enable3,4 (Pin 9) ← RC0 (RFID_RL_ENABLE)
  Input3 (Pin 10)   ← RC2 (RFID_EXCT) via RC filter
  Output3 (Pin 11)  → RFID Coil Driver
  Input4 (Pin 15)   ← RB1 (COMMON_LOCK)
  Output4 (Pin 14)  → Red Solenoid

Power:
  VCC1 (Pin 16)     ← RC3 (L293_LOGIC) controlled +5V
  VCC2 (Pin 8)      ← Main power supply (battery/5V)
  GND (Pins 4,5,12,13) → Ground
```

**Key Points:**
- L293D is powered via RC3 (L293_LOGIC) to save power when idle
- Enable pins must be HIGH for outputs to activate
- Inputs control direction: HIGH on Input1/4, LOW on Input2/3 = lock
- Each solenoid should only be powered for **500ms maximum** to prevent overheating

### LM324 Op-Amp Circuit

The LM324 is a quad op-amp used to amplify and demodulate the RFID signal.

```
LM324 RFID Signal Processing:
                                    
RFID Antenna → Tuned Circuit → Op-Amp Stage 1 (Amplify)
    134.2kHz      (L/C)           ↓
                              Op-Amp Stage 2 (Demodulate/Filter)
                                  ↓
                              RA2 (RFID_STREAM) → ADC
                                  
Power: RB3 (LM324_PWR) → Controlled +5V supply
       Powered only during RFID read to save current
```

**Typical Configuration:**
- Gain Stage 1: ~40-60 dB
- Demodulation: Envelope detector with RC filter
- Output: 0-5V analog signal representing tag data
- Power consumption: ~3-5mA per channel when active

### RFID Excitation Circuit

```
RC2/CCP1 (PWM 134.2kHz, 50% duty)
    ↓
RC Filter (reduces harmonics)
    ↓
L293D Channel 3/4 Input (Current amplification)
    ↓
Tuned L/C Circuit (Antenna coil + tuning capacitor)
    ↓
Magnetic field at 134.2 kHz
```

**PWM Configuration:**
```c
// CCP1 PWM Mode for 134.2 kHz
// Fosc = 19.6 MHz, Fosc/4 = 4.9 MHz
// PWM Period = [(PR2) + 1] × 4 × Tosc × Prescaler
// Target: 134.2 kHz → Period = 7.45 μs
// PR2 = (4.9 MHz / 134.2 kHz) - 1 ≈ 36
// Duty Cycle = 50% → CCPR1L = 18
```

---

## LED and Buzzer Circuits

### LED Circuit Variations

**⚠️ CRITICAL**: LED polarity and drive circuits vary between hardware revisions!

#### Revision X4 (Original - Direct Drive)
```
RB4 (RED_LED) ──┬──[470Ω]──[LED>]──GND
                │
RB5 (GREEN_LED)─┴──[470Ω]──[LED>]──GND

Active: HIGH (1) = LED ON
        LOW (0)  = LED OFF
```

#### Possible Revision X5+ (Inverted or Different Configuration)

**Configuration A - Inverted Drive (Common Anode):**
```
+5V ──[470Ω]──[LED>]──RB4 (RED_LED)
+5V ──[470Ω]──[LED>]──RB5 (GREEN_LED)

Active: LOW (0)  = LED ON (sink current)
        HIGH (1) = LED OFF
```

**Configuration B - Transistor Driver:**
```
RB4 ──[10kΩ]──|──┬──[470Ω]──[LED>]──+5V
              │  │
             ┴   └─ NPN transistor (inverted logic)
              E   C
              │
             GND
             
Active: HIGH (1) = Transistor ON = LED ON
        LOW (0)  = Transistor OFF = LED OFF
```

**Configuration C - Shift Register/LED Driver IC:**
```
RB4 ──┐
      ├─→ [74HC595 or similar] ──→ Multiple LEDs
RB5 ──┘      Shift Register
      
Logic depends on shift register configuration
May require serial data protocol
```

### How to Identify Your LED Configuration

#### Method 1: Visual Inspection
1. **Power off the device**
2. Locate LEDs on PCB (usually near front panel)
3. Trace connections:
   - **Direct to MCU pin**: Simple resistor to LED
   - **To transistor**: Look for small 3-pin component (SOT-23 or TO-92)
   - **To IC**: Multiple LEDs may share driver IC

#### Method 2: Multimeter Continuity Test
1. **Power off and disconnect**
2. Set multimeter to diode test mode
3. Test between PIC pin and LED:
   - **Direct connection**: Continuity through resistor (~470Ω to ~1kΩ)
   - **No direct path**: Likely has transistor or driver IC

#### Method 3: Live Testing (Careful!)
1. **Power on device** (use caution)
2. Use voltmeter to measure pin voltage:
   - Read voltage at RB4 and RB5 in different modes
   - Compare to LED state (ON/OFF)
   - **If HIGH = LED ON**: Direct drive
   - **If LOW = LED ON**: Inverted drive
3. Document your findings

### Buzzer Circuit Variations

#### Revision X4 (Original - Active Buzzer)
```
RC1 (BUZZER) ──[100Ω]──[BUZZER]──GND
                         (Active)
Active: Toggle HIGH/LOW at ~1kHz = Beep
        Sustained HIGH = Continuous tone
```

#### Possible Revision X5+ Variations

**Passive Buzzer (Requires frequency):**
```
RC1 ──[100Ω]──[PIEZO]──GND
              (Passive)
              
Requires: PWM or toggling at audio frequency (1-5kHz)
Firmware toggle: Already implemented in beep() function
```

**Transistor-Driven Buzzer (Higher current):**
```
RC1 ──[10kΩ]──|──┬──[100Ω]──[BUZZER]──+5V
              │  │
             ┴   └─ NPN transistor
              E   C
              │
             GND
             
Active: HIGH = Buzzer ON (may be inverted)
```

### Testing and Adapting LED/Buzzer Code

If your hardware revision has different LED/buzzer behavior:

#### Option 1: Software Inversion (Quick Fix)

Edit `peripherals.h` to invert logic:

```c
// For inverted LED logic (common anode or transistor inverted)
#define RED_LED_ON()    (RED_LED = 0)
#define RED_LED_OFF()   (RED_LED = 1)
#define GREEN_LED_ON()  (GREEN_LED = 0)
#define GREEN_LED_OFF() (GREEN_LED = 1)

// For inverted buzzer
#define BUZZER_ON()     (BUZZER = 0)
#define BUZZER_OFF()    (BUZZER = 1)
```

Then replace direct assignments in code:
```c
// Old code:
RED_LED = 1;        // Turn on
RED_LED = 0;        // Turn off

// New code:
RED_LED_ON();       // Turn on (handles inversion)
RED_LED_OFF();      // Turn off (handles inversion)
```

#### Option 2: Test Mode for Hardware Detection

Add this diagnostic function to `main.c`:

```c
/**
 * Hardware diagnostic - cycles through all outputs
 * Use this to verify LED/buzzer polarity and operation
 */
void hardwareDiagnostic(void) {
    printf("Hardware Diagnostic Mode\r\n");
    printf("Watch LEDs and buzzer\r\n\r\n");
    
    // Test Red LED
    printf("RED_LED = HIGH (5 sec)\r\n");
    RED_LED = 1;
    __delay_ms(5000);
    RED_LED = 0;
    __delay_ms(1000);
    
    printf("RED_LED = LOW (5 sec)\r\n");
    RED_LED = 0;
    __delay_ms(5000);
    RED_LED = 1;
    __delay_ms(1000);
    
    // Test Green LED
    printf("GREEN_LED = HIGH (5 sec)\r\n");
    GREEN_LED = 1;
    __delay_ms(5000);
    GREEN_LED = 0;
    __delay_ms(1000);
    
    printf("GREEN_LED = LOW (5 sec)\r\n");
    GREEN_LED = 0;
    __delay_ms(5000);
    GREEN_LED = 1;
    __delay_ms(1000);
    
    // Test Buzzer
    printf("BUZZER = HIGH (2 sec)\r\n");
    BUZZER = 1;
    __delay_ms(2000);
    BUZZER = 0;
    __delay_ms(1000);
    
    printf("BUZZER = LOW (2 sec)\r\n");
    BUZZER = 0;
    __delay_ms(2000);
    BUZZER = 1;
    __delay_ms(1000);
    
    printf("Diagnostic complete\r\n");
}
```

Call this function at startup to identify correct polarity.

---

## UART Serial Communication

### Circuit Overview

The PIC16F886 UART (RC6/TX, RC7/RX) connects to an external serial interface.

#### Standard Configuration (Revision X4)
```
RC6 (TX) ────────────────────→ External RX
RC7 (RX) ←──────────────────── External TX
                            
Direct connection, 9600 baud
No level shifting (5V logic levels)
```

#### Revision X5+ with Series Resistors
```
RC6 (TX) ──[150Ω]──────────→ External RX
RC7 (RX) ←──[150Ω]────────── External TX
                            
150Ω series resistors added for protection or impedance matching
May affect signal quality
```

### Impact of 150Ω Series Resistors

The 150Ω series resistors can affect UART operation in several ways:

#### 1. **Voltage Drop (Usually Minimal)**
```
Effect: V_drop = I × R = 0.001A × 150Ω = 0.15V
Impact: Negligible for 5V logic (still >4.5V)
```

For typical UART current draw (1-2mA), voltage drop is minimal.

#### 2. **RC Time Constant (More Significant)**
```
With cable capacitance: τ = R × C
Example: 150Ω × 100pF = 15ns

At 9600 baud:
  Bit period = 104μs
  Rise time impact: 15ns / 104μs = 0.014% (negligible)
  
At 38400 baud (old baud rate):
  Bit period = 26μs  
  Rise time impact: 15ns / 26μs = 0.058% (still acceptable)
```

**Conclusion**: 150Ω resistors should NOT significantly affect 9600 or 38400 baud rates under normal conditions.

#### 3. **Impedance Matching**
```
UART output impedance: ~25-50Ω (PIC internal)
Cable impedance: ~100-120Ω (typical twisted pair)
Load impedance: ~1-10kΩ (receiver input)

With 150Ω resistors:
  Source impedance: 25Ω + 150Ω = 175Ω
  Better match to cable impedance
  Reduces reflections on long cables
```

**Purpose**: Likely added for:
- **ESD protection**: Limits current from static discharge
- **Impedance matching**: Improves signal integrity on longer cables
- **Current limiting**: Protects pins from short circuits

### UART Signal Quality Analysis

#### Without Resistors (Original X4)
```
Signal Quality:
  ✓ Clean square waves
  ✓ Fast rise/fall times (<1μs)
  ✗ Potential ringing on long cables
  ✗ More susceptible to ESD damage
  ✗ Higher current on shorts
```

#### With 150Ω Resistors (X5+)
```
Signal Quality:
  ✓ Reduced ringing and reflections
  ✓ Better ESD protection
  ✓ Lower current on faults
  ~ Slightly slower rise/fall times
  ~ May affect very long cables (>5m)
```

### Software Compensation Options

If you experience UART issues with the 150Ω resistors, try these software optimizations:

#### 1. Reduce Baud Rate (Already Done)
```c
// Current setting (optimal for reliability)
#define BAUD_RATE 9600  // 0.48% error, very robust

// If still having issues, can reduce further:
#define BAUD_RATE 4800  // Even more reliable, very slow
```

**Note**: The firmware already uses 9600 baud (reduced from 38400) specifically for improved reliability. This should handle the resistors without issue.

#### 2. Add Inter-Character Delay
```c
void putch(char byte)
{
    while(!TXIF) {
        // Wait for transmit buffer to be ready
    }
    TXREG = byte;
    
    // Optional: Add small delay between characters
    __delay_us(100);  // 100μs delay (still much less than bit time)
}
```

This gives the receiver more time to process each character.

#### 3. Increase Serial Timeout
```c
// In serial.c
#define SERIAL_TIMEOUT 10  // Increase from 5ms to 10ms
```

Allows more time for characters to arrive through resistive path.

#### 4. Enable Stronger Drive Strength (Hardware)

The PIC16F886 has limited drive strength options, but you can verify these settings:

```c
// Ensure TX pin is configured for maximum drive
// This is automatic for UART peripheral, but verify:
TRISCbits.TRISC6 = 0;  // TX as output (redundant with UART enable)
```

### Hardware Bypass Considerations

If software optimizations don't resolve UART issues, you may consider bypassing the resistors:

#### Option A: Direct Bypass (Requires Soldering)
```
Before:  RC6 ──[150Ω]──→ Header
After:   RC6 ───────────→ Header (resistor bridged)
```

**Pros:**
- Restores original signal quality
- Matches original firmware design

**Cons:**
- Removes ESD protection
- May increase ringing on long cables
- Requires PCB modification

#### Option B: External Buffer/Driver
```
RC6 ──[150Ω]──→ [74HC244] ──→ Header
                  Buffer IC
```

**Pros:**
- Maintains ESD protection (through resistors)
- Provides strong drive for long cables
- Isolation from PIC outputs

**Cons:**
- Requires additional components
- More complex modification

#### Option C: RS-232 Level Shifter
```
RC6 ──[150Ω]──→ [MAX232] ──→ RS-232 Levels (±12V)
                  Level Shifter
```

**Pros:**
- Professional serial interface
- Excellent noise immunity
- Long cable support (>10m)
- Built-in ESD protection

**Cons:**
- Requires MAX232 or similar IC
- Needs external capacitors
- More complex circuit

### Recommendation: Test First, Modify Later

**Step 1**: Test with current configuration (9600 baud, 150Ω resistors)
- The 9600 baud rate should handle the resistors fine
- Use short cable (<1m) initially
- Monitor `uartErrors` structure for error counts

**Step 2**: If errors occur, try software optimizations
- Increase timeout
- Add inter-character delay
- Reduce baud rate to 4800 if needed

**Step 3**: Only if software fails, consider hardware bypass
- Measure actual signal quality with oscilloscope first
- Verify resistors are actually causing the problem
- Consider external buffer before direct bypass

### UART Testing Procedure

#### Test 1: Basic Communication
```bash
# Connect serial terminal at 9600 baud
screen /dev/ttyUSB0 9600

# You should see startup banner:
========================================
PetSafe Cat Flap - Alternative Firmware
Serial Interface Ready
Baud Rate: 9600 bps
========================================

# Send status command: S
# Expected response: AM0L512P512S3
```

#### Test 2: Error Monitoring

Add to your code:
```c
// Print error statistics every 10 seconds
if ((millis() % 10000) < 100) {  // Every 10s
    printf("UART Errors: F=%d O=%d B=%d\r\n",
           uartErrors.framingErrors,
           uartErrors.overrunErrors,
           uartErrors.bufferOverflows);
}
```

Monitor for increasing errors:
- **Framing errors**: Baud rate mismatch or signal quality issue
- **Overrun errors**: Application not reading data fast enough  
- **Buffer overflows**: Too much data arriving

#### Test 3: Signal Quality (Oscilloscope)

If available, use oscilloscope to measure:

**Transmit (TX/RC6):**
- **Idle state**: Should be HIGH (~5V)
- **Start bit**: Clean transition to LOW (0V)
- **Data bits**: Clear HIGH/LOW levels
- **Rise time**: <10μs preferred, <50μs acceptable at 9600 baud
- **Fall time**: <10μs preferred, <50μs acceptable at 9600 baud
- **Ringing**: <10% overshoot acceptable

**Receive (RX/RC7):**
- Similar characteristics to TX
- Verify signal arrives cleanly after resistors

### Cable Recommendations

For reliable operation with 150Ω resistors:

**Short cables (<1m)**: Any cable works
**Medium cables (1-3m)**: Twisted pair or shielded recommended  
**Long cables (3-5m)**: Shielded twisted pair required
**Very long (>5m)**: Consider RS-232 level shifter

**Avoid:**
- Ribbon cable (high capacitance)
- Unshielded parallel wires (crosstalk)
- Running near power cables (noise coupling)

---

## Hardware Revision Differences

### Known Revisions

| Revision | Known Features | LED Circuit | Buzzer Circuit | UART Resistors | Notes |
|----------|---------------|-------------|----------------|----------------|-------|
| **X4** | Original firmware target | Direct drive | Active, direct | No resistors | Original design |
| **X5+** | Later production | Unknown (varies) | Unknown (varies) | 150Ω series (reported) | User reports differences |

### Identifying Your Hardware Revision

#### Method 1: PCB Marking
Look for revision marking on PCB:
- Usually silkscreen text: "Rev X4", "Rev X5", etc.
- May be near PIC or edge of board
- Sometimes on component side, sometimes solder side

#### Method 2: Component Comparison
Compare components to reference:
- Count LEDs (should be 2: red and green)
- Identify buzzer type (active vs passive)
- Check for series resistors on UART lines
- Note any additional ICs not in schematic

#### Method 3: Trace Analysis
Use multimeter continuity to trace connections:
- LED cathodes to ground or PIC pins
- Buzzer connections
- UART lines to header

### Creating Your Own Hardware Documentation

If you have a different revision, document it for the community:

1. **Take high-resolution photos**:
   - Top side (component side)
   - Bottom side (solder side)
   - Close-ups of: PIC area, LED area, UART area

2. **Create pin trace table**:
   ```
   | PIC Pin | Traces To | Component | Notes |
   |---------|-----------|-----------|-------|
   | RB4     | R10 → LED1 Anode | Red LED | 470Ω resistor |
   | RB5     | Q1 Base   | Transistor | NPN, drives green LED |
   ```

3. **Test and document LED polarity**:
   - Use diagnostic function (see section above)
   - Record: HIGH = ON/OFF, LOW = ON/OFF

4. **Share with community**:
   - Open GitHub issue with "Hardware Revision X5" (or your revision)
   - Include photos and documentation
   - Help others with same revision

---

## Pin Tracing Methodology

### Tools Needed

1. **Digital Multimeter (DMM)**:
   - Continuity mode (beep on short)
   - Resistance mode (measure resistor values)
   - Diode mode (test LEDs, diodes)

2. **Visual Aids**:
   - Magnifying glass or loupe (5x-10x)
   - Good lighting (LED work light)
   - Camera/phone for photos

3. **Documentation**:
   - Blank paper or printed pin diagram
   - Colored pens for marking traces
   - Spreadsheet or table for recording

### Step-by-Step Tracing Procedure

#### Step 1: Preparation
```
1. Disconnect all power (battery and external)
2. Wait 30 seconds for capacitors to discharge
3. Remove cat flap PCB from enclosure if possible
4. Clean PCB with isopropyl alcohol if dirty
5. Set up good lighting and magnification
```

#### Step 2: Identify PIC Pins
```
1. Locate PIC16F886 on PCB
2. Identify Pin 1 (usually marked with dot or notch)
3. Count pins: 1-28 going counter-clockwise (DIP view from top)
4. Mark Pin 1, Pin 14, Pin 28 on your diagram
```

#### Step 3: Trace Critical Pins

Start with the most important pins for your issue:

**For LED/Buzzer Issues:**
```
Priority pins: RB4 (25), RB5 (26), RC1 (12)

For each pin:
1. Touch multimeter probe to PIC pin
2. Visually follow trace on PCB
3. Identify: resistor, LED, transistor, or other component
4. Measure resistance between pin and component
5. Test continuity to ground or VCC
6. Document: "RB4 → 470Ω → LED+ → GND"
```

**For UART Issues:**
```
Priority pins: RC6 (17), RC7 (18)

For each pin:
1. Locate pin on PIC
2. Follow trace to header/connector
3. Check for series resistors
4. Measure resistor value (should be ~150Ω if present)
5. Test for any parallel components (capacitors, diodes)
6. Document path
```

#### Step 4: Component Identification

**Resistors:**
- Read color codes or measure with DMM
- Typical values: 100Ω-10kΩ for series, 1kΩ-100kΩ for pull-ups

**LEDs:**
- Use DMM diode mode: should show 1.8-2.2V forward drop
- Test polarity: diode mode shows which way conducts
- Note color: Red ~1.8V, Green ~2.1V

**Transistors:**
- Identify type: NPN (2N3904, BC547) or PNP (2N3906, BC557)
- Test with DMM: B-E junction ~0.6V, B-C junction ~0.6V
- Trace: PIC pin → Base, Collector → Load, Emitter → GND/VCC

**Buzzers:**
- Active: Has internal oscillator, 2 pins (+ and -)
- Passive: Piezo element, 2 pins (no polarity, or red/black wires)
- Test: Apply 3-5V directly, active buzzer sounds, passive is silent

#### Step 5: Create Schematic

Draw what you found:
```
RB4 ──[470Ω]──┬──[LED>]──GND
              │
              └── (Your finding here)
```

### Common PCB Tracing Tips

1. **Multilayer boards**: Some traces may be on internal layers
   - Look for vias (small holes) that connect layers
   - Use continuity mode to find hidden connections

2. **Ground and power planes**: 
   - Large copper areas may be hard to trace visually
   - Use continuity to identify ground and power connections
   - Ground usually connects to: GND pins (8, 19), mounting holes

3. **SMD components**:
   - Small resistors: read 3-digit code (e.g., 151 = 150Ω)
   - Capacitors: read value (104 = 0.1μF = 100nF)

4. **Trace under components**:
   - Some traces run under chips
   - May need to trace from both ends and infer connection

### Documentation Template

Create a table like this for your revision:

```markdown
## Hardware Revision X5 (or your revision)

### LED Circuit
| Pin | Component Path | Polarity | Notes |
|-----|----------------|----------|-------|
| RB4 | RB4 → R10 (470Ω) → LED1+ → GND | HIGH = ON | Direct drive |
| RB5 | RB5 → R11 (10kΩ) → Q1 Base, Q1 Collector → R12 (470Ω) → LED2+ → VCC, Q1 Emitter → GND | HIGH = ON | NPN transistor, inverted |

### Buzzer Circuit  
| Pin | Component Path | Type | Notes |
|-----|----------------|------|-------|
| RC1 | RC1 → R13 (100Ω) → BZ1+ → GND | Active | Requires toggle for beep |

### UART Circuit
| Pin | Component Path | Resistor Value | Notes |
|-----|----------------|----------------|-------|
| RC6 | RC6 → R14 (150Ω) → J1 Pin 2 | 150Ω | Series resistor for protection |
| RC7 | RC7 → R15 (150Ω) → J1 Pin 3 | 150Ω | Series resistor for protection |

### Photos
- [Photo 1: PCB top view](link)
- [Photo 2: LED area close-up](link)
- [Photo 3: UART area close-up](link)
```

---

## Debugging Tools and Techniques

### Essential Tools

#### 1. Digital Multimeter (DMM)
**Minimum features:**
- DC voltage: 0-20V range
- Resistance: 0-20kΩ range
- Continuity: Audible beep
- Diode test: Forward voltage measurement

**Usage:**
```
Voltage measurement:
  - Power supply: Should be 5.00V ±5% (4.75-5.25V)
  - PIC VDD (Pin 20): Should equal power supply
  - Logic HIGH: Should be >4.5V
  - Logic LOW: Should be <0.5V

Continuity:
  - Trace connections between components
  - Verify ground connections
  - Check for shorts

Resistance:
  - Measure resistor values
  - Check LED forward voltage (diode mode)
  - Verify no shorts between power and ground (>1kΩ)
```

#### 2. Logic Analyzer
**Recommended: Cheap USB logic analyzer (~$10-20)**

**Capabilities:**
- Capture digital signals (up to 8-24 channels)
- Protocol decoding (UART, SPI, I2C)
- Timing analysis
- Signal comparison

**Usage for PetSafe debugging:**
```
Channel assignments:
  CH0: RC6 (TX)     - UART transmit
  CH1: RC7 (RX)     - UART receive
  CH2: RB4 (RED_LED) - Red LED state
  CH3: RB5 (GREEN_LED) - Green LED state
  CH4: RC1 (BUZZER)  - Buzzer control
  CH5: RC2 (RFID_EXCT) - RFID excitation PWM

Trigger: Rising edge on TX (captures serial transmission)
Sample rate: 1 MHz minimum (10 MHz preferred)
```

**Software:**
- Sigrok PulseView (free, cross-platform)
- Saleae Logic (commercial, excellent UI)
- DSView (free, Sigrok-based)

#### 3. Oscilloscope
**Useful but not required for basic debugging**

**Minimum specs:**
- Bandwidth: 10 MHz (50 MHz preferred)
- Sample rate: 100 MS/s minimum
- Channels: 2 (4 preferred)

**What to measure:**
```
RFID PWM (RC2):
  - Frequency: Should be 134.2 kHz
  - Duty cycle: Should be 50%
  - Amplitude: 0-5V square wave

UART signals:
  - Baud rate: Measure bit time (should be 104 μs for 9600 baud)
  - Rise/fall time: Should be <10 μs
  - Logic levels: HIGH ~5V, LOW ~0V

Power supply:
  - Ripple: Should be <100 mV
  - Noise: Should be <50 mV
  - Transients: Check during solenoid activation
```

#### 4. Serial Terminal
**Free software options:**
- PuTTY (Windows)
- screen (Linux/Mac)
- minicom (Linux)
- CoolTerm (cross-platform)
- Arduino Serial Monitor

**Configuration:**
```
Port: /dev/ttyUSB0 (Linux) or COM3 (Windows)
Baud rate: 9600
Data bits: 8
Parity: None
Stop bits: 1
Flow control: None
Line ending: LF (\n) or CR+LF (\r\n)
```

### Debugging Techniques

#### Technique 1: Serial Debug Output

Add verbose debugging to code:

```c
// In main.c
#define DEBUG_VERBOSE 1  // Set to 1 for debug output

#if DEBUG_VERBOSE
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) 
#endif

// Usage:
DEBUG_PRINT("Switching to mode %d\r\n", opMode);
DEBUG_PRINT("Light sensor: %d, Threshold: %d\r\n", light, lightThd);
DEBUG_PRINT("RFID read result: %d\r\n", result);
```

#### Technique 2: LED Blink Patterns

Use LEDs for visual debugging without serial:

```c
// Error codes via LED blinks
void blinkError(uint8_t errorCode) {
    for (uint8_t i = 0; i < errorCode; i++) {
        RED_LED = 1;
        __delay_ms(200);
        RED_LED = 0;
        __delay_ms(200);
    }
    __delay_ms(1000);  // Pause between sequences
}

// Usage:
if (rfidError) {
    blinkError(3);  // 3 blinks = RFID error
}
```

#### Technique 3: State Machine Logging

Log state transitions:

```c
void switchMode(uint8_t mode) {
    uint8_t oldMode = opMode;
    
    // ... mode switching code ...
    
    printf("Mode: %d → %d (", oldMode, opMode);
    switch(opMode) {
        case MODE_NORMAL: printf("NORMAL"); break;
        case MODE_VET: printf("VET"); break;
        case MODE_NIGHT: printf("NIGHT"); break;
        // ... etc ...
    }
    printf(")\r\n");
}
```

#### Technique 4: Watchpoint Debugging

Monitor specific variables:

```c
// In main loop
static uint8_t lastOpMode = 0xFF;
static bool lastInLocked = false;

if (opMode != lastOpMode) {
    printf("OpMode changed: %d\r\n", opMode);
    lastOpMode = opMode;
}

if (inLocked != lastInLocked) {
    printf("InLocked changed: %d\r\n", inLocked);
    lastInLocked = inLocked;
}
```

#### Technique 5: Performance Profiling

Measure execution time:

```c
void profileFunction(void) {
    ms_t start = millis();
    
    // Function to profile
    uint8_t result = readRFID(id, 6, &crc1, &crc2);
    
    ms_t elapsed = millis() - start;
    printf("readRFID took %lu ms\r\n", (unsigned long)elapsed);
}
```

### Advanced Debugging: ICSP Debugging

The PIC16F886 supports in-circuit debugging via ICSP:

**Requirements:**
- PICkit 3/4 or ICD 3 (MPLAB Snap doesn't support debugging)
- MPLAB X IDE (not just IPE)
- Firmware compiled in debug mode

**Capabilities:**
- Set breakpoints
- Step through code
- Watch variables in real-time
- View register values

**Limitations:**
- Timing-critical code (RFID) may behave differently
- Some resources used by debugger
- Can't debug while device is in use (pet access blocked)

---

## Troubleshooting Common Issues

### Issue 1: LEDs Always Off or Always On

**Symptoms:**
- LEDs don't respond to mode changes
- LEDs stuck in one state

**Diagnosis:**
1. **Check power supply**: Measure VDD at Pin 20 (should be 5V)
2. **Check LED polarity**: Test with multimeter diode mode
3. **Test PIC output**: Measure voltage at RB4/RB5 pins
   - Should toggle between 0V and 5V
4. **Check LED circuit**: Trace from PIC pin to LED

**Solutions:**
- **If PIC output correct but LED wrong**: Polarity issue, use software inversion
- **If PIC output stuck**: Code issue or pin configuration problem
- **If no PIC output**: Check TRISB configuration, LED may be shorted

### Issue 2: Buzzer Silent or Wrong Tone

**Symptoms:**
- Buzzer doesn't beep
- Buzzer has continuous tone
- Wrong frequency

**Diagnosis:**
1. **Identify buzzer type**: Active or passive?
   - Apply 5V directly: Active buzzes, passive is silent
2. **Check PIC output**: Measure RC1 pin during beep
   - Should toggle if active buzzer
   - Should PWM if passive buzzer  
3. **Measure voltage**: 0-5V range during beep?

**Solutions:**
- **Active buzzer, no beep**: Check polarity, voltage supply
- **Passive buzzer, no beep**: Requires PWM frequency (current code should work)
- **Wrong frequency**: Adjust beep() function toggle rate

### Issue 3: UART Not Working

**Symptoms:**
- No startup banner
- Garbage characters
- No response to commands

**Diagnosis:**
1. **Check baud rate**: Terminal set to 9600?
2. **Check wiring**: TX to RX, RX to TX (crossed)?
3. **Check voltage levels**: Measure RC6 and RC7 idle state (should be ~5V)
4. **Look for resistors**: 150Ω series resistors present?
5. **Monitor errors**: Check `uartErrors` counters

**Solutions:**
| Problem | Solution |
|---------|----------|
| Wrong baud rate | Set terminal to 9600 baud |
| Reversed wiring | Swap TX and RX connections |
| Low voltage | Check for shorts or loading |
| High error count | Reduce baud rate, check cable quality |
| Garbage characters | Verify crystal frequency (19.6 MHz) |

### Issue 4: RFID Not Reading

**Symptoms:**
- No cat detection
- Intermittent reads
- Wrong IDs detected

**Diagnosis:**
1. **Check PWM**: Measure RC2 frequency (should be 134.2 kHz)
2. **Check excitation power**: LM324_PWR and L293_LOGIC enabled?
3. **Check RFID signal**: Measure RA2 voltage (should vary 0-5V near tag)
4. **Test with known tag**: Use cat's actual collar tag

**Solutions:**
- **No PWM**: Check CCP1 configuration
- **Low signal on RA2**: Check LM324 op-amp power, L293D connections
- **Intermittent**: May be noise, add capacitor to RA2 (0.1μF to GND)

### Issue 5: Solenoids Not Actuating

**Symptoms:**
- Latches don't move
- Clicking sound but no movement
- Wrong direction

**Diagnosis:**
1. **Check power**: L293_LOGIC enabled? (should be HIGH during activation)
2. **Check enable pins**: CL_GL_ENABLE or RFID_RL_ENABLE HIGH?
3. **Check direction**: COMMON_LOCK and GREEN/RED_LOCK states
4. **Measure voltage**: Solenoid should get 5V for 500ms

**Solutions:**
- **No power**: Check L293_LOGIC pin, may be shorted or code issue
- **No movement**: Check solenoid connections, may be open circuit
- **Wrong direction**: Swap COMMON_LOCK polarity in code
- **Weak movement**: Check power supply (solenoids draw ~500mA)

---

## Signal Quality and Testing

### Oscilloscope Signal Analysis

If you have access to an oscilloscope, verify these signals:

#### RFID PWM (RC2)
```
Expected waveform:
  ┌─┐ ┌─┐ ┌─┐
  │ │ │ │ │ │
──┘ └─┘ └─┘ └──

Frequency: 134.2 kHz ± 1%
Period: 7.45 μs
Duty cycle: 50% ± 5%
Amplitude: 0-5V
Rise time: <1 μs
Fall time: <1 μs
```

**How to measure:**
1. Connect probe to RC2 (Pin 13)
2. Set scope to 2 μs/div horizontal
3. Set 2V/div vertical
4. Trigger on rising edge
5. Measure frequency with cursor

#### UART TX (RC6)
```
Expected waveform (character 'A' = 0x41):
Idle  Start  Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7 Stop
 ─────┐    ┌────┐    ┌────┐    ┌────┐         ┌────┐    ┌────
      └────┘    └────┘    └────┘    └─────────┘    └────┘

Bit time: 104 μs (for 9600 baud)
Logic HIGH: ~5V
Logic LOW: ~0V
Rise/fall: <10 μs
```

**How to measure:**
1. Connect probe to RC6 (Pin 17)
2. Send single character via printf
3. Trigger on falling edge (start bit)
4. Measure bit time with cursor (should be 104 μs)
5. Verify 8 data bits + start + stop

#### Light Sensor (RA0)
```
Expected: 0-5V DC, varies with light
Typical values:
  Bright light: 0-100 (ADC units)
  Room light: 200-500
  Darkness: 600-1023
```

**How to measure:**
1. Connect probe to RA0 (Pin 2)
2. DC coupling
3. Vary light on sensor
4. Voltage should change smoothly

### Logic Analyzer Protocol Decode

Using a logic analyzer with UART decode:

**Configuration:**
```
Protocol: UART (async serial)
Channels: CH0 = TX (RC6), CH1 = RX (RC7)
Baud rate: 9600
Data bits: 8
Parity: None
Stop bits: 1
Bit order: LSB first
Logic level: 5V CMOS
```

**Expected startup sequence:**
```
TX: "\r\n"
TX: "========================================\r\n"
TX: "PetSafe Cat Flap - Alternative Firmware\r\n"
TX: "Serial Interface Ready\r\n"
TX: "Baud Rate: 9600 bps\r\n"
TX: "========================================\r\n"
TX: "\r\n"
```

**Expected command/response:**
```
RX: 'S' (0x53)
TX: "AM0L512P512S3\n" (status response)
```

### Power Supply Quality

Verify power supply meets specifications:

**DC Voltage:**
- Nominal: 5.0V
- Tolerance: ±5% (4.75V - 5.25V)
- Ripple: <100 mV peak-to-peak
- Noise: <50 mV RMS

**Current Capacity:**
- Idle: ~50-100 mA
- RFID reading: ~150-200 mA
- Solenoid activation: ~500-600 mA (peak)
- Minimum supply: 1A rated

**Test procedure:**
1. Measure voltage at VDD (Pin 20) with DMM
2. Measure ripple with oscilloscope (AC coupling)
3. Trigger solenoid and observe voltage drop
   - Should not drop below 4.5V
   - If drops more: insufficient power supply

---

## Conclusion

This guide provides comprehensive information for debugging and adapting the PetSafe Cat Flap firmware to different hardware revisions. Key takeaways:

### For LED/Buzzer Issues:
1. **Trace circuits** to identify polarity and drive method
2. **Use diagnostic function** to test hardware behavior
3. **Apply software inversion** if needed
4. **Document your findings** for community

### For UART Issues with 150Ω Resistors:
1. **Test first with 9600 baud** - should work fine
2. **Monitor error counters** for actual issues
3. **Try software optimizations** before hardware changes
4. **Use oscilloscope** to verify signal quality
5. **Only bypass resistors** if proven necessary

### For Hardware Revision Differences:
1. **Identify your revision** from PCB markings
2. **Trace critical pins** using methodology in this guide
3. **Document and photograph** your findings
4. **Share with community** via GitHub issues

### General Debugging:
1. **Start simple**: Power supply, pin voltages, basic connectivity
2. **Use available tools**: Multimeter, serial terminal, LED indicators
3. **Progress systematically**: Don't skip steps
4. **Document everything**: Create your own notes and schematics

### Safety Reminders:
- ⚡ Always disconnect power before probing
- 🐈 Never test on pets without backup access
- 📋 Keep good documentation of changes
- 🔧 Double-check connections before powering on

---

## Appendix: Quick Reference

### Pin Quick Reference
```
Critical pins for debugging:
  RB4 (25): Red LED
  RB5 (26): Green LED  
  RC1 (12): Buzzer
  RC6 (17): UART TX
  RC7 (18): UART RX
  RC2 (13): RFID PWM (134.2 kHz)
  RA2 (4):  RFID Signal Input
  RA0 (2):  Light Sensor
```

### Voltage Quick Reference
```
Normal operation:
  VDD: 5.0V ±5%
  Logic HIGH: >4.5V
  Logic LOW: <0.5V
  RFID signal (RA2): 0-5V varying
  Light sensor (RA0): 0-5V, varies with light
```

### Baud Rate Calculator
```
Formula: Actual Baud = _XTAL_FREQ / (16 × (SPBRG + 1))

For 19.6 MHz crystal:
  SPBRG = 127 → 9646 baud (0.48% error) ✓ Current
  SPBRG = 255 → 4784 baud (0.33% error) ✓ Even more reliable
  SPBRG = 63  → 19345 baud (0.73% error) ○ Acceptable
  SPBRG = 31  → 39062 baud (2.91% error) ✗ Unreliable
```

### Common Component Values
```
LEDs: 470Ω series resistor (typical)
Buttons: 10kΩ pull-up (internal weak pull-ups used)
UART: 150Ω series resistor (on some revisions)
Crystal: 19.6 MHz, 18-22pF load capacitors
Power: 0.1μF bypass capacitors on VDD
```

---

**Document Version**: 1.0  
**Last Updated**: December 2024  
**Maintainer**: Community (documentation fork)  
**Original Hardware**: Revision X4  
**Contributions Welcome**: GitHub Issues and Pull Requests

---

**Remember**: This firmware controls physical access for living animals. Debug thoroughly, test carefully, and always provide alternative pet access during development and testing.
