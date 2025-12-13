# PIC16F886 Pinout Quick Reference

**Quick reference card for PetSafe Cat Flap PCB debugging and development**

## Visual Pinout Diagram

```
                        PIC16F886 (28-Pin DIP/SOIC)
                        Looking at chip from top
                        Notch or dot marks Pin 1
                        
                              ╔═══╗
                         MCLR ║1 ●28║ RB7/ICSPDAT    (Green Button, ICSP Data)
          (Light Sensor) RA0  ║2  27║ RB6/ICSPCLK    (Red Button, ICSP Clock)
       (Flap Position*) RA1   ║3  26║ RB5            (Green LED)
        (RFID Stream)   RA2   ║4  25║ RB4            (Red LED)
                        RA3   ║5  24║ RB3            (LM324 Power)
                        RA4   ║6  23║ RB2            (CL_GL_ENABLE)
       (Red Solenoid)   RA5   ║7  22║ RB1            (Common Lock)
                        GND   ║8  21║ RB0            (Door Switch)
         (19.6MHz Xtal) OSC1  ║9  20║ VDD            (+5V Power)
         (19.6MHz Xtal) OSC2  ║10 19║ VSS            (Ground)
    (RFID_RL_ENABLE)    RC0   ║11 18║ RC7/RX         (UART Receive)
           (Buzzer)     RC1   ║12 17║ RC6/TX         (UART Transmit)
        (RFID PWM)      RC2   ║13 16║ RC5            (Serial Header I/O)
       (L293 Logic)     RC3   ║14 15║ RC4            (Green Solenoid)
                              ╚═══╝
                              
     * RA1 is optional, may be unused or used for flap position sensor
```

## Pin Groupings by Function

### Power Pins (3 pins)
```
Pin 20: VDD  ────────────→ +5V supply (must be 4.75-5.25V)
Pin 8:  GND  ────────────→ Ground
Pin 19: VSS  ────────────→ Ground (same as GND)
```

### Programming Pins (ICSP) (5 pins)
```
Pin 1:  MCLR/VPP  ───────→ Master Clear / Programming Voltage
Pin 27: RB6/PGC   ───────→ Programming Clock (also Red Button)
Pin 28: RB7/PGD   ───────→ Programming Data (also Green Button)
Pin 20: VDD       ───────→ +5V Power
Pin 8:  GND       ───────→ Ground
```

**ICSP Header Standard Layout:**
```
   ┌───────────┐
   │ 1  MCLR   │──── Pin 1 (MCLR)
   │ 2  VDD    │──── Pin 20 (+5V)
   │ 3  GND    │──── Pin 8, 19 (GND)
   │ 4  PGD    │──── Pin 28 (RB7)
   │ 5  PGC    │──── Pin 27 (RB6)
   │ 6  NC     │──── Not Connected (or AUX)
   └───────────┘
```

### Clock/Oscillator (2 pins)
```
Pin 9:  OSC1/CLKIN  ─────→ 19.6 MHz Crystal input
Pin 10: OSC2/CLKOUT ─────→ 19.6 MHz Crystal output
```

**Crystal Circuit:**
```
         ┌───────────┐
Pin 9  ──┤    XTAL   ├── Pin 10
    │    │ 19.6 MHz  │    │
  [22pF] └───────────┘  [22pF]
    │                    │
   GND                  GND
```

### Analog Input Pins (4 pins)
```
Pin 2 (RA0/AN0): Light Sensor    (ADC Channel 0, 0-1023, higher = darker)
Pin 3 (RA1/AN1): Flap Position*  (ADC Channel 1, optional, may be unused)
Pin 4 (RA2/AN2): RFID Signal     (ADC Channel 2, 0-5V demodulated RFID)
Pin 5 (RA3/AN3): Reserved        (ADC Channel 3, available for expansion)

* Flap position sensor may not be populated on all hardware revisions
```

### Digital I/O - User Interface (5 pins)
```
Pin 25 (RB4):     Red LED        Output, Active HIGH* (check your revision)
Pin 26 (RB5):     Green LED      Output, Active HIGH* (check your revision)
Pin 12 (RC1):     Buzzer         Output, Toggle for beep
Pin 27 (RB6):     Red Button     Input, Active LOW, weak pull-up enabled
Pin 28 (RB7):     Green Button   Input, Active LOW, weak pull-up enabled

* LED polarity may vary by hardware revision (see HARDWARE_DEBUGGING_GUIDE.md)
```

### Solenoid Control (3 pins)
```
Pin 7 (RA5):      Red Solenoid   Output, Active HIGH, max 500ms
Pin 15 (RC4):     Green Solenoid Output, Active HIGH, max 500ms
Pin 22 (RB1):     Common Lock    Output, Direction control for L293D
```

### Motor Driver Control (4 pins)
```
Pin 23 (RB2):     CL_GL_ENABLE    Output, Enable L293D channels 1/2
Pin 11 (RC0):     RFID_RL_ENABLE  Output, Enable L293D channels 3/4
Pin 14 (RC3):     L293_LOGIC      Output, Power control for L293D
Pin 24 (RB3):     LM324_PWR       Output, Power control for LM324 op-amp
```

### RFID System (2 pins)
```
Pin 13 (RC2/CCP1): RFID Excitation  Output, 134.2 kHz PWM, 50% duty cycle
Pin 4 (RA2/AN2):   RFID Signal      Input, Demodulated signal, ADC Channel 2
```

### UART Serial (2 pins)
```
Pin 17 (RC6/TX): UART Transmit  Output, 9600 baud, may have 150Ω resistor
Pin 18 (RC7/RX): UART Receive   Input, 9600 baud, may have 150Ω resistor
```

### Miscellaneous (3 pins)
```
Pin 21 (RB0):     Door Switch     Input, Active LOW, weak pull-up enabled
Pin 16 (RC5):     Serial Header   Output, Unused, available for debugging
Pin 6 (RA4):      Reserved        Available for expansion
```

---

## Port Configurations

### PORT A (8 pins: RA0-RA7, but only RA0-RA5 available)
```
Bit  Pin  Name   Direction  Function                Pull-up  Analog
─────────────────────────────────────────────────────────────────────
0    2    RA0    Input      Light Sensor            No       Yes (AN0)
1    3    RA1    Input      Flap Position*          No       Yes (AN1)
2    4    RA2    Input      RFID Signal             No       Yes (AN2)
3    5    RA3    Input      Reserved                No       Yes (AN3)
4    6    RA4    Input      Reserved                No       No
5    7    RA5    Output     Red Solenoid            No       No
6    -    -      N/A        Not available on PIC16F886
7    -    -      N/A        Not available on PIC16F886

Configuration Registers:
  TRISA  = 0xDF  (0b11011111) - Bit 5 output, all others input
  ANSEL  = 0x0D  (0b00001101) - AN0, AN2, AN3 analog; others digital
```

### PORT B (8 pins: RB0-RB7)
```
Bit  Pin  Name   Direction  Function                Pull-up  Special
─────────────────────────────────────────────────────────────────────
0    21   RB0    Input      Door Switch             Yes      INT
1    22   RB1    Output     Common Lock             No       -
2    23   RB2    Output     CL_GL_ENABLE            No       -
3    24   RB3    Output     LM324 Power             No       -
4    25   RB4    Output     Red LED                 No       -
5    26   RB5    Output     Green LED               No       -
6    27   RB6    Input      Red Button              Yes      ICSP CLK
7    28   RB7    Input      Green Button            Yes      ICSP DAT

Configuration Registers:
  TRISB  = 0xC1  (0b11000001) - Bits 0,6,7 input; others output
  WPUB   = 0xC1  (0b11000001) - Pull-ups on RB0, RB6, RB7
  OPTION_REGbits.nRBPU = 0    - Enable weak pull-ups
```

### PORT C (8 pins: RC0-RC7)
```
Bit  Pin  Name   Direction  Function                Pull-up  Special
─────────────────────────────────────────────────────────────────────
0    11   RC0    Output     RFID_RL_ENABLE          No       -
1    12   RC1    Output     Buzzer                  No       -
2    13   RC2    Output     RFID Excitation         No       CCP1/PWM
3    14   RC3    Output     L293 Logic Power        No       -
4    15   RC4    Output     Green Solenoid          No       -
5    16   RC5    Output     Serial Header (unused)  No       -
6    17   RC6    Output     UART TX                 No       USART
7    18   RC7    Input      UART RX                 No       USART

Configuration Registers:
  TRISC  = 0xC0  (0b11000000) - Bits 6,7 controlled by UART; others output
```

---

## Electrical Characteristics

### Absolute Maximum Ratings
```
Parameter                   Min     Max     Unit
────────────────────────────────────────────────
VDD (Power Supply)          -0.3    +7.5    V
Input voltage (any pin)     -0.3    VDD+0.3 V
Output current (per pin)    -       25      mA
Output current (PORT)       -       200     mA
Operating temperature       -40     +85     °C
Storage temperature         -65     +150    °C
```

### Recommended Operating Conditions
```
Parameter                   Min     Typ     Max     Unit
────────────────────────────────────────────────────────
VDD (Power Supply)          4.5     5.0     5.5     V
VDD (for this design)       4.75    5.0     5.25    V  (±5%)
Operating frequency         0       -       20      MHz
Operating temperature       0       25      70      °C  (typical)
```

### DC Characteristics (VDD = 5.0V, 25°C)
```
Parameter                        Typ     Unit    Conditions
────────────────────────────────────────────────────────────
Input HIGH voltage (VIH)         4.5     V       Min for logic 1
Input LOW voltage (VIL)          0.5     V       Max for logic 0
Output HIGH voltage (VOH)        4.7     V       IOH = -3mA
Output LOW voltage (VOL)         0.3     V       IOL = 8.5mA
Input leakage current            ±1      μA      VSS ≤ Vin ≤ VDD
Weak pull-up current             50-400  μA      Typical 250 μA
```

### Current Consumption (VDD = 5.0V, 19.6 MHz)
```
Condition                        Typical     Unit
────────────────────────────────────────────────
CPU active, no peripherals       5           mA
CPU + UART active                8           mA
CPU + ADC active                 7           mA
CPU + PWM active                 6           mA
Sleep mode                       1           μA
Watchdog timer active            18          μA
```

### Additional Load Current (External Components)
```
Component                        Current     Notes
────────────────────────────────────────────────────────
Red LED                          10-20 mA    With 470Ω resistor
Green LED                        10-20 mA    With 470Ω resistor
Buzzer                           5-15 mA     Active buzzer
LM324 op-amp                     3-5 mA      Per channel (4 total)
L293D motor driver               10-20 mA    Logic supply (no load)
Solenoids (each)                 400-600 mA  Peak during activation
RFID coil                        80-120 mA   During reading
Total idle (typical)             50-100 mA   LEDs off, no RFID
Total with RFID read             150-200 mA  LEDs off
Peak with solenoid               500-700 mA  During latch actuation
```

**Power Supply Requirement**: Minimum 1A rated supply recommended

---

## Pin Function Details

### Input Pins

#### Analog Inputs (ADC)
```
RA0 (Pin 2): Light Sensor
  - ADC Channel: AN0
  - Range: 0-1023 (10-bit)
  - Value: Higher = Darker
  - Sample time: 20 μs minimum acquisition
  - Used for: Night mode auto-locking

RA2 (Pin 4): RFID Signal
  - ADC Channel: AN2
  - Range: 0-1023 (10-bit)
  - Signal: Demodulated 134.2 kHz RFID carrier
  - Threshold: 512 (mid-range) for bit detection
  - Timing: Critical - sampled at specific intervals
```

#### Digital Inputs (Buttons and Sensors)
```
RB0 (Pin 21): Door Switch
  - Active: LOW (0V) when door closed
  - Idle: HIGH (~5V) via weak pull-up
  - Pull-up: Internal WPUB enabled (250 μA typical)
  - Debounce: Software debouncing recommended

RB6 (Pin 27): Red Button
  - Active: LOW when pressed
  - Idle: HIGH via weak pull-up
  - Pull-up: Internal WPUB enabled
  - Dual function: ICSP programming clock

RB7 (Pin 28): Green Button
  - Active: LOW when pressed
  - Idle: HIGH via weak pull-up
  - Pull-up: Internal WPUB enabled
  - Dual function: ICSP programming data

RC7 (Pin 18): UART Receive
  - Logic: 5V CMOS levels
  - Idle: HIGH (~5V)
  - Baud rate: 9600 bps
  - Series resistor: May have 150Ω (check revision)
```

### Output Pins

#### PWM Output
```
RC2 (Pin 13): RFID Excitation (CCP1)
  - Frequency: 134.2 kHz
  - Duty cycle: 50%
  - Amplitude: 0-5V square wave
  - Mode: CCP1 module in PWM mode
  - Connected to: L293D input via RC filter
```

#### Digital Outputs (LEDs and Buzzer)
```
RB4 (Pin 25): Red LED
  - Logic: HIGH = ON* (may vary by revision)
  - Current: 10-20 mA (via series resistor)
  - Typical resistor: 470Ω
  - Polarity: Check your hardware revision!

RB5 (Pin 26): Green LED
  - Logic: HIGH = ON* (may vary by revision)
  - Current: 10-20 mA (via series resistor)
  - Typical resistor: 470Ω
  - Polarity: Check your hardware revision!

RC1 (Pin 12): Buzzer
  - Type: Active buzzer (typical)
  - Drive: Toggle at ~1 kHz for beep
  - Current: 5-15 mA
  - Series resistor: 100Ω typical
```

#### Power Control Outputs
```
RB3 (Pin 24): LM324 Power
  - Function: Powers op-amp for RFID
  - Logic: HIGH = ON, LOW = OFF
  - Current: 3-5 mA per channel
  - Duty: Only enabled during RFID read

RC3 (Pin 14): L293D Logic Power
  - Function: Powers L293D logic section
  - Logic: HIGH = ON, LOW = OFF
  - Current: 10-20 mA
  - Duty: Enabled during solenoid actuation

RB2 (Pin 23): CL_GL_ENABLE
  - Function: Enable L293D channels 1/2
  - Controls: Green latch solenoid
  - Logic: HIGH = Enabled
  - Duration: 500ms max

RC0 (Pin 11): RFID_RL_ENABLE
  - Function: Enable L293D channels 3/4
  - Controls: Red latch + RFID driver
  - Logic: HIGH = Enabled
  - Duration: Varies (500ms for solenoid)
```

#### Solenoid Control Outputs
```
RA5 (Pin 7): Red Solenoid
  - Function: Exit latch control
  - Logic: HIGH = Lock, LOW = Unlock*
  - Current: Via L293D H-bridge
  - Duration: 500ms maximum
  - Safety: Returns to safe state after

RC4 (Pin 15): Green Solenoid
  - Function: Entrance latch control
  - Logic: HIGH = Lock, LOW = Unlock*
  - Current: Via L293D H-bridge
  - Duration: 500ms maximum
  - Safety: Returns to safe state after

RB1 (Pin 22): Common Lock
  - Function: Direction control for L293D
  - Logic: Combined with solenoid pins
  - Current: Via L293D H-bridge
  - Note: Controls solenoid polarity

* Direction depends on L293D wiring
```

#### UART Output
```
RC6 (Pin 17): UART Transmit
  - Logic: 5V CMOS levels
  - Idle: HIGH (~5V)
  - Baud rate: 9600 bps
  - Series resistor: May have 150Ω (check revision)
  - Format: 8N1 (8 data, no parity, 1 stop)
```

---

## Common Voltage Measurements

Use these as reference when debugging with a multimeter:

### Normal Operation (Device Idle, LEDs Off)
```
Pin     Expected Voltage    Notes
────────────────────────────────────────────────────────
1       4.5-5.5V            MCLR pulled high
2       0-5V (varies)       Light sensor output
4       ~2.5V               RFID idle (no tag)
7       LOW (0V)            Solenoid idle
8       0V                  Ground reference
11      LOW (0V)            RFID driver disabled
12      LOW (0V)            Buzzer off
13      0V or 2.5V          RFID PWM off or idle
14      LOW (0V)            L293D power off
15      HIGH (~5V)          Solenoid safe state
17      HIGH (~5V)          UART TX idle
18      HIGH (~5V)          UART RX idle
20      5.0V ±5%            Main power supply
21      HIGH (~5V)          Door switch pull-up
22      HIGH (~5V)          Common lock safe state
23      LOW (0V)            Enable off
24      LOW (0V)            Op-amp power off
25      LOW (0V)            Red LED off
26      LOW (0V)            Green LED off
27      HIGH (~5V)          Button pull-up
28      HIGH (~5V)          Button pull-up
```

### During RFID Read
```
Pin     Expected Voltage    Notes
────────────────────────────────────────────────────────
4       0-5V rapidly        RFID signal demodulated
11      HIGH (5V)           RFID driver enabled
13      0-5V @ 134.2kHz     PWM excitation active
14      HIGH (5V)           L293D powered
24      HIGH (5V)           Op-amp powered
```

### During Solenoid Activation (Green Latch)
```
Pin     Expected Voltage    Notes
────────────────────────────────────────────────────────
14      HIGH (5V)           L293D powered
15      HIGH or LOW         Direction control
22      HIGH or LOW         Direction control (opposite of pin 15)
23      HIGH (5V)           Enable channels 1/2
```

---

## Troubleshooting with Pinout

### Problem: No Power
```
Check:
  Pin 20 (VDD): Should be 5.0V ±5%
  Pin 8, 19 (GND): Should be 0V
  Pin 1 (MCLR): Should be >4.5V (pulled high)
  
If Pin 20 is 0V: Power supply issue
If Pin 1 is 0V: MCLR pulled low (device in reset)
```

### Problem: Can't Program
```
Check:
  Pin 1 (MCLR): Should accept programming voltage (12-13V)
  Pin 27 (PGC): Should see clock pulses during programming
  Pin 28 (PGD): Should see data during programming
  Pin 20 (VDD): Should be 5.0V
  Pin 8 (GND): Should be 0V and connected to programmer ground
  
Common issues:
  - MCLR not pulled high (add 10kΩ pull-up)
  - Wrong voltage on VDD (must be 4.5-5.5V)
  - Ground not connected between programmer and target
  - Config bits set to disable LVP (use high-voltage programming)
```

### Problem: LEDs Not Working
```
Check:
  Pin 25 (RB4): Measure voltage during LED operation
    - Should toggle between 0V and 5V
    - If stuck at one level: software or configuration issue
  Pin 26 (RB5): Same as above for green LED
  
Measure LED directly:
  - Use multimeter diode mode
  - Should show ~1.8-2.2V forward drop
  - Check polarity (anode to cathode)
  
If pin toggles but LED doesn't:
  - Check series resistor (should be 100-1000Ω)
  - Check LED orientation
  - Check for broken trace
```

### Problem: UART Not Working
```
Check:
  Pin 17 (TX): Should idle HIGH (~5V)
    - During transmission: Should show pulses
    - If always LOW: possible short to ground
    - If always HIGH: may be disconnected or not transmitting
  Pin 18 (RX): Should idle HIGH (~5V)
    - If LOW: possible short or wrong wiring
    - If floating: not connected
  
Check for series resistors:
  - Measure resistance between Pin 17 and header
  - Measure resistance between Pin 18 and header
  - Should be <10Ω if no resistor, ~150Ω if resistor present
  
Verify crystal:
  - Pin 9, 10: Should see ~2-3V DC (oscillating)
  - If 0V or 5V static: crystal not oscillating
  - Check crystal connections and capacitors
```

### Problem: RFID Not Reading
```
Check:
  Pin 13 (RC2): Should show 134.2 kHz PWM when reading
    - Use oscilloscope or frequency counter
    - If no PWM: CCP1 module not configured
    - If wrong frequency: PR2 register incorrect
  Pin 4 (RA2): Should show varying voltage (0-5V) near tag
    - If constant: check op-amp power (Pin 24)
    - If noisy: may need filtering capacitor
  Pin 24 (RB3): Should be HIGH during RFID read
    - Powers LM324 op-amp
    - If LOW: op-amp not powered
  Pin 11 (RC0): Should be HIGH during RFID read
    - Enables RFID driver
    - If LOW: driver not enabled
```

---

## Quick Pin Test Procedure

**10-Minute Basic Hardware Test**

You'll need: Multimeter, 5V power supply, current meter (optional)

```
Step 1: Power Check (2 minutes)
  [ ] Apply 5V to Pin 20 (VDD)
  [ ] Connect ground to Pin 8, 19 (GND)
  [ ] Measure Pin 20: Should be 5.0V ±0.25V
  [ ] Measure Pin 1 (MCLR): Should be >4.5V
  [ ] Current draw: Should be 5-10 mA (no firmware loaded)

Step 2: Crystal Check (1 minute)
  [ ] Measure Pin 9 (OSC1): Should be ~2-3V DC (if oscillating)
  [ ] Measure Pin 10 (OSC2): Should be ~2-3V DC (if oscillating)
  [ ] If 0V or 5V: Crystal not working (check connections)

Step 3: Input Check (2 minutes)
  [ ] Measure Pin 27 (RB6): Should be ~5V (pull-up)
  [ ] Measure Pin 28 (RB7): Should be ~5V (pull-up)
  [ ] Measure Pin 21 (RB0): Should be ~5V (pull-up)
  [ ] Short each to ground: Voltage should drop to ~0V

Step 4: Analog Input Check (2 minutes)
  [ ] Measure Pin 2 (RA0): Should be 0-5V (light dependent)
  [ ] Measure Pin 4 (RA2): Should be ~2-3V (idle)
  [ ] Vary light on sensor: Pin 2 should change

Step 5: Output Check (3 minutes) - Requires firmware
  [ ] Measure Pin 25 (RB4): Should toggle during operation
  [ ] Measure Pin 26 (RB5): Should toggle during operation
  [ ] Measure Pin 17 (TX): Should be HIGH idle, pulses during TX
  [ ] Measure Pin 18 (RX): Should be HIGH idle

Pass all checks? Hardware is likely OK!
Fail any check? See detailed troubleshooting in main guide.
```

---

## Related Documents

- **[HARDWARE_DEBUGGING_GUIDE.md](HARDWARE_DEBUGGING_GUIDE.md)** - Complete debugging guide with circuits and troubleshooting
- **[CODE_ARCHITECTURE.md](CODE_ARCHITECTURE.md)** - Software architecture and pin usage in code
- **[DEPLOYMENT.md](DEPLOYMENT.md)** - Programming and deployment guide
- **[README.md](README.md)** - Project overview and getting started

## External References

### Datasheets
- **[PIC16F886 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/41291F.pdf)** - Official Microchip datasheet (336 pages)
- **[L293D Motor Driver](https://www.ti.com/lit/ds/symlink/l293.pdf)** - Texas Instruments datasheet
- **[LM324 Op-Amp](https://www.ti.com/lit/ds/symlink/lm324.pdf)** - Texas Instruments datasheet
- **[PICkit 3 User Guide](https://ww1.microchip.com/downloads/en/DeviceDoc/51795B.pdf)** - ICSP programming guide

### Programming Tools
- **[MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)** - Development environment
- **[XC8 Compiler](https://www.microchip.com/mplab/compilers)** - C compiler for PIC microcontrollers
- **[MPLAB IPE](https://www.microchip.com/mplab/mplab-integrated-programming-environment)** - Production programming tool

### Community Resources
- **[PetSafe Cat Flap GitHub](https://github.com/CJFWeatherhead/PetSafe-CatFlap)** - Main repository
- **[GitHub Issues](https://github.com/CJFWeatherhead/PetSafe-CatFlap/issues)** - Bug reports and hardware revisions
- **[GitHub Discussions](https://github.com/CJFWeatherhead/PetSafe-CatFlap/discussions)** - Community help

---

**Document Version**: 1.0  
**Last Updated**: December 2024  
**Device**: PIC16F886 (28-pin)  
**Application**: PetSafe Pet Porte 100-1023  
**Crystal**: 19.6 MHz  
**Voltage**: 5.0V ±5%

---

**💡 Tip**: Print this document and keep it at your workbench for quick reference during debugging!

**⚠️ Safety**: Always disconnect power before probing circuits or making modifications.
