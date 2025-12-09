# GitHub Copilot Instructions for PetSafe Cat Flap Firmware

## Project Authority and Context

This is the **PetSafe Cat Flap Alternative Firmware** project - a professional embedded systems firmware for the PIC16F886 microcontroller that controls RFID-authenticated pet access. This project maintains **rigorous engineering standards** with comprehensive documentation, strict linting, extensive unit testing, and robust CI/CD pipelines.

## Project Overview

### Core Purpose
Alternative firmware for the PetSafe Pet Porte 100-1023 PCB, providing enhanced control features for RFID-authenticated cat flap operation on a PIC16F886 8-bit microcontroller.

### Key Capabilities
- **RFID Tag Authentication**: 134.2 kHz EM4100 protocol with CRC verification
- **Multiple Operating Modes**: Normal, Vet (lockdown), Night (light-sensor), Learn, Clear, Open
- **Hardware Control**: Solenoid latches, LED indicators, buzzer feedback, button interface
- **Serial Communication**: 38400 baud UART for external monitoring and control
- **Persistent Storage**: EEPROM-based configuration and tag storage (up to 16 cats)
- **Light-Sensor Integration**: Automatic operation based on ambient light conditions

## Architecture Standards

### System Architecture
The firmware follows a **layered modular architecture** with strict separation of concerns:

```
Application Layer (main.c)
    ↓
Hardware Abstraction Layer (cat.c, rfid.c, serial.c, peripherials.c)
    ↓
Hardware Layer (PIC16F886 registers and peripherals)
```

### Module Organization

**Core Modules** (each must maintain single responsibility):
1. **main.c** - Application logic, mode management, main control loop
2. **cat.c/h** - EEPROM storage for RFID tags and configuration (128-byte config space, 16×8-byte cat slots)
3. **rfid.c/h** - 134.2 kHz RFID reading with EM4100 protocol, CRC validation
4. **serial.c/h** - UART communication with ring buffer (16-byte capacity), interrupt-driven
5. **peripherials.c/h** - Hardware I/O abstraction (ADC, timers, GPIO, solenoids)
6. **interrupts.c/h** - ISR handlers (Timer1 1ms tick, UART receive)
7. **user.c/h** - System initialization orchestration
8. **configuration_bits.c** - PIC16F886 configuration bits

### Critical Design Patterns

**Hardware Abstraction**: All hardware access MUST go through defined macros in peripherials.h:
```c
// CORRECT - Use defined macros
RED_LED = 1;
GREEN_LED = 0;
if (RED_BTN == 0) { /* button pressed */ }

// INCORRECT - Direct port manipulation
// PORTB = 0x10;  // Never do this - unclear and unmaintainable
```

**Timing**: Use non-blocking millisecond counter pattern (DO NOT use blocking delays):
```c
// CORRECT - Non-blocking timeout
ms_t start = millis();
while ((millis() - start) < TIMEOUT_MS) {
    // Can continue processing other tasks
    if (condition_met) break;
}

// INCORRECT - Blocking delay
// __delay_ms(5000);  // Blocks all processing
```

**Error Handling**: Return error codes, check all returns:
```c
uint8_t result = readRFID(id, 6, &crcComputed, &crcRead);
if (result != 0) {
    // Handle NO_CARRIER, NO_HEADER, BAD_START, BAD_CRC
}
```

## Embedded C Coding Standards

### Memory Constraints - CRITICAL

**PIC16F886 Resources** (strictly enforced):
- **Program Memory**: 8K words (14-bit instructions) - Target <90% usage
- **Data Memory (RAM)**: 368 bytes total - Target <80% usage
- **EEPROM**: 256 bytes (persistent storage only)
- **Stack**: Limited - avoid deep recursion (max 3-4 levels)

**Memory Best Practices**:
```c
// PREFER - Local variables (stack-allocated, automatically freed)
void function(void) {
    uint8_t local = 0;  // Efficient stack usage
}

// AVOID - Unnecessary global variables
// static uint8_t unnecessary_global = 0;  // Uses precious RAM

// PREFER - const data (stored in program memory, not RAM)
const uint8_t LOOKUP_TABLE[] = {1, 2, 3, 4, 5};

// AVOID - Large arrays in RAM
// uint8_t largeBuffer[200];  // Would use 54% of available RAM!

// PREFER - Appropriate data types
uint8_t counter;      // 0-255 (1 byte)
bool flag;            // true/false (1 byte)
uint16_t largeValue;  // 0-65535 (2 bytes)

// AVOID - Oversized types
// uint32_t counter;  // Wastes 3 bytes if value never exceeds 255
```

### Naming Conventions (strictly enforced)

**Functions**: camelCase
```c
void readSensor(void);
uint16_t getLightSensor(void);
bool lockGreenLatch(bool lock);
```

**Variables**: camelCase
```c
uint8_t opMode;
bool inLocked;
ms_t startTime;
```

**Constants/Macros**: UPPER_CASE
```c
#define MODE_NORMAL 0
#define OPEN_TIME 5000
#define SER_BUFFER 16
```

**Types**: PascalCase
```c
typedef struct {
    uint16_t crc;
    uint8_t id[6];
} Cat;
```

**File-scope (static)**: Prefix with static keyword
```c
static uint8_t privateVar = 0;
static void internalHelper(void);
```

### Code Formatting (non-negotiable)

**Indentation**: 4 spaces (NO TABS)
```c
void exampleFunction(uint8_t param) {
    if (condition) {
        doSomething();
    } else {
        doAlternative();
    }
    
    while (running) {
        process();
    }
}
```

**Braces**: Same-line for control structures
```c
// CORRECT
if (condition) {
    code();
}

for (int i = 0; i < 10; i++) {
    process(i);
}

// INCORRECT
// if (condition)
// {
//     code();
// }
```

**Spacing**: Space after keywords, around operators
```c
// CORRECT
if (a == b) {
    result = a + b;
}

// INCORRECT
// if(a==b){
//     result=a+b;
// }
```

### Function Documentation (required for all public functions)

```c
/**
 * Read RFID tag and validate CRC
 * 
 * Enables 134.2kHz PWM excitation, waits for synchronization header,
 * reads 10 bytes (6-byte ID + 2-byte reserved + 2-byte CRC), validates
 * CRC-CCITT checksum, and returns status.
 * 
 * @param id        Pointer to 6-byte buffer for tag ID
 * @param len       Length of ID buffer (must be 6)
 * @param crcComputed Pointer to store computed CRC
 * @param crcRead   Pointer to store CRC read from tag
 * @return 0 on success, or error code (NO_CARRIER, NO_HEADER, BAD_START, BAD_CRC)
 */
uint8_t readRFID(uint8_t* id, uint8_t len, uint16_t* crcComputed, uint16_t* crcRead);
```

### Comments Guidelines

**When to Comment**:
- Explain **WHY**, not **WHAT** (code shows what)
- Document complex algorithms or non-obvious logic
- Explain hardware timing requirements
- Document safety-critical sections
- Clarify unusual workarounds

**When NOT to Comment**:
```c
// BAD - States the obvious
counter++;  // Increment counter

// GOOD - Explains why
counter++;  // Maintain sync with 1ms timer tick for accurate delays
```

## Testing Philosophy (strictly enforced)

### Testing Strategy

This project maintains **professional-grade testing standards**:

1. **Unit Tests** - Test individual modules in isolation (Ceedling/Unity framework)
2. **Static Analysis** - Automated code quality checks (cppcheck)
3. **Build Verification** - Ensure compilation with XC8 compiler for target hardware
4. **Hardware Testing** - Manual verification on actual PIC16F886 hardware

### Unit Testing Requirements

**Test Framework**: Ceedling (Unity + CMock + GCov)

**Test File Structure**:
```
test/
├── test_cat.c          # Cat EEPROM module tests
├── test_rfid.c         # RFID reader tests  
├── test_serial.c       # Serial communication tests
└── support/
    ├── xc_mock.h       # Hardware register mocks
    └── xc_mock.c       # Mock implementations
```

**Writing Tests** (required for all new functionality):
```c
#include "unity.h"
#include "module_to_test.h"

// Setup runs before EACH test
void setUp(void) {
    // Initialize test fixtures, reset mocks
}

// Teardown runs after EACH test
void tearDown(void) {
    // Clean up resources
}

// Test naming: test_<function>_<scenario>
void test_readRFID_returns_error_on_bad_crc(void) {
    // ARRANGE - Set up test data
    uint8_t id[6];
    uint16_t crcComputed, crcRead;
    
    // ACT - Execute function under test
    uint8_t result = readRFID(id, 6, &crcComputed, &crcRead);
    
    // ASSERT - Verify expected behavior
    TEST_ASSERT_EQUAL_INT(BAD_CRC, result);
}
```

**Common Assertions**:
```c
TEST_ASSERT_EQUAL_INT(expected, actual);
TEST_ASSERT_EQUAL_UINT8(expected, actual);
TEST_ASSERT_EQUAL_UINT16(expected, actual);
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);
TEST_ASSERT_EQUAL_MEMORY(expected, actual, size);
```

**Running Tests**:
```bash
# Run all tests
ceedling test:all

# Run specific module tests
ceedling test:cat
ceedling test:rfid
ceedling test:serial

# Generate code coverage report
ceedling gcov:all

# Clean test artifacts
ceedling clean
```

**Test Coverage Goals**:
- **Critical safety functions**: 100% coverage (latch control, RFID validation)
- **Business logic**: 80%+ coverage (mode switching, cat storage)
- **Hardware abstraction**: Mock interfaces and test all paths
- **Edge cases**: ALWAYS test boundary conditions and error paths

### Static Analysis (mandatory before commits)

**Tool**: cppcheck with strict settings

**Running Static Analysis**:
```bash
# Use provided script
./lint.sh

# Or run directly
cppcheck --enable=all --inconclusive --std=c99 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    *.c
```

**Issues Detected** (must be addressed):
- Memory leaks
- Buffer overflows / array bounds violations
- Null pointer dereferences
- Uninitialized variables
- Type mismatches
- Dead code / unreachable code
- Unused variables

**Suppressing False Positives** (only when justified):
```c
// cppcheck-suppress unusedFunction
static void internalHelper(void) {
    // Called indirectly via function pointer
}
```

### CI/CD Pipeline (all checks must pass)

**GitHub Actions Workflow** (`.github/workflows/ci.yml`):

Automatically runs on every push and pull request:

1. **Build with XC8 Compiler**
   - Compiles all source files for PIC16F886 target
   - Verifies no compilation errors or warnings
   - Uploads build artifacts

2. **Static Code Analysis**
   - Runs cppcheck on all source files
   - Reports code quality issues
   - Does not fail on warnings (review required)

3. **Unit Tests**
   - Runs complete test suite with Ceedling
   - Must have 0 test failures
   - Uploads test results and coverage reports

**All jobs must pass before merging** - no exceptions.

## Linting and Code Quality Standards

### Pre-commit Checklist

Before committing ANY code changes:

1. **Build Successfully**:
   ```bash
   # In MPLAB X IDE: Clean and Build (Shift+F11)
   # Or via command line with XC8 compiler
   ```

2. **Run Static Analysis**:
   ```bash
   ./lint.sh
   # Address all legitimate issues, justify any suppressions
   ```

3. **Run Unit Tests**:
   ```bash
   ceedling test:all
   # All tests must pass
   ```

4. **Check Code Size**:
   - Program memory: <90% (leave headroom for future features)
   - RAM usage: <80% (avoid memory exhaustion)

5. **Review Changes**:
   ```bash
   git diff
   # Ensure only intended changes are included
   ```

### Code Quality Rules

**Mandatory Rules** (will be caught by static analysis or code review):

1. **No Magic Numbers**: Use named constants
   ```c
   // CORRECT
   #define MAX_CATS 16
   if (catCount < MAX_CATS) { /* ... */ }
   
   // INCORRECT
   // if (catCount < 16) { /* ... */ }  // What is 16?
   ```

2. **Initialize All Variables**:
   ```c
   // CORRECT
   uint8_t result = 0;
   bool initialized = false;
   
   // INCORRECT
   // uint8_t result;  // Undefined behavior
   ```

3. **Check All Return Values**:
   ```c
   // CORRECT
   uint8_t status = readRFID(id, 6, &crc1, &crc2);
   if (status != 0) {
       // Handle error
   }
   
   // INCORRECT
   // readRFID(id, 6, &crc1, &crc2);  // Ignoring potential errors
   ```

4. **Validate Inputs**:
   ```c
   uint8_t getCat(Cat* cat, uint8_t slot) {
       // CORRECT - Validate parameters
       if (cat == NULL || slot >= CAT_SLOTS) {
           return ERROR_INVALID_PARAM;
       }
       // Proceed with valid inputs
   }
   ```

5. **Avoid Buffer Overflows**:
   ```c
   // CORRECT
   if (index < ARRAY_SIZE) {
       array[index] = value;
   }
   
   // INCORRECT
   // array[index] = value;  // Unchecked bounds
   ```

6. **Use Safe String Operations**:
   ```c
   // CORRECT - Size-limited copy
   for (int i = 0; i < 6 && i < sizeof(dest); i++) {
       dest[i] = src[i];
   }
   
   // INCORRECT
   // strcpy(dest, src);  // No bounds checking
   ```

### Static vs Dynamic Memory

**Prefer Static Allocation** (predictable, no fragmentation):
```c
// CORRECT - Compile-time allocation
#define BUFFER_SIZE 16
uint8_t buffer[BUFFER_SIZE];

// AVOID - malloc/free not available and not recommended for embedded
// uint8_t* buffer = malloc(16);  // Not available on PIC16F886
```

## Safety and Reliability Standards

### Pet Safety (highest priority)

**Fail-Safe Principles**:
1. **Power Loss**: Device should fail to "unlocked" state (pets can exit)
2. **Error Conditions**: Default to safe mode (allow exit, lock entrance)
3. **Malfunction**: Provide manual override (mechanical backup)

**Testing Requirements**:
- Test ALL modes thoroughly before deployment
- Verify latches operate smoothly and completely
- Test with actual RFID tags
- Provide alternative pet access during testing
- Never test on pets without a backup access method

### Hardware Safety

**Critical Sections** (must be protected):
```c
// Solenoid activation - precise timing required
bool lockGreenLatch(bool lock) {
    // Enable drivers
    CL_GL_ENABLE = 1;
    // Set direction
    COMMON_LOCK = lock ? 1 : 0;
    GREEN_LOCK = lock ? 0 : 1;
    // Hold for 500ms
    __delay_ms(500);
    // Disable for safety
    CL_GL_ENABLE = 0;
    // Return to safe state
    COMMON_LOCK = 0;
    GREEN_LOCK = 0;
    return lock;
}
```

**Power Management**:
- Disable RFID excitation when not actively reading (saves ~100mA)
- Disable op-amp power when idle
- Minimize LED on-time where possible

## Hardware-Specific Requirements

### PIC16F886 Configuration

**Clock Configuration**:
- Crystal: 19.6 MHz High-Speed (HS mode)
- _XTAL_FREQ = 19660800UL (used by delay macros)
- Instruction cycle: Fosc/4 = 4.9 MHz

**Configuration Bits** (in configuration_bits.c):
```c
#pragma config FOSC = HS        // High-Speed crystal
#pragma config WDTE = OFF       // Watchdog timer OFF (no auto-reset)
#pragma config MCLRE = ON       // MCLR pin enabled
#pragma config LVP = OFF        // High-voltage programming only
```

### Pin Configuration (DO NOT MODIFY without careful consideration)

**PORTA**:
- RA0: Light sensor (ADC input AN0)
- RA2: RFID demodulated signal (ADC input AN2)
- RA5: Red solenoid control

**PORTB**:
- RB0: Door switch (input with pull-up)
- RB1: Solenoid common
- RB2: L293D enable 1/2 (CL_GL_ENABLE)
- RB3: LM324 op-amp power
- RB4: Red LED
- RB5: Green LED
- RB6: Red button (input with pull-up)
- RB7: Green button (input with pull-up)

**PORTC**:
- RC0: L293D enable 3/4 (RFID_RL_ENABLE)
- RC1: Buzzer control
- RC2: RFID excitation PWM (CCP1 output)
- RC3: L293D logic power
- RC4: Green solenoid control
- RC5: Serial header (unused)
- RC6: UART TX
- RC7: UART RX

### Timing Constraints (critical for RFID reading)

**Timer1 Configuration** (1ms interrupt):
```c
// Prescaler: 1:4
// Timer clock = Fosc/4 / 4 = 1.225 MHz
// For 1ms overflow: TMR1 = 0xFB37
#define TMR1_H_PRES 0xFB
#define TMR1_L_PRES 0x37
```

**RFID Timing**:
- Carrier frequency: 134.2 kHz (PWM on CCP1)
- Bit rate: ~400 bps (2.5ms per bit)
- Synchronization timeout: 100ms
- Complete read timeout: ~300ms

**UART Timing**:
- Baud rate: 38400 bps (configured via SPBRG register)
- Character time: ~260 μs per byte
- Serial timeout: 5ms per byte

## Serial Protocol Specification

### Command Format

**Commands** (sent TO device):

| Command | Format | Parameters | Description |
|---------|--------|------------|-------------|
| `S` | `S` | None | Request status |
| `M` | `M[mode]` | mode: 0-6 | Set operating mode |
| `C` | `C[R/S][index][value]` | R=Read, S=Set | Read/Set configuration |

**Responses** (sent FROM device):

| Response | Format | Description |
|----------|--------|-------------|
| Status | `AM[mode]L[light]P[pos]S[bits]\n` | Current device status |
| Config | `AC[index]V[value]\n` | Configuration value |
| Mode | `AM[mode]\n` | Mode confirmation |
| Error | `AE\n` | Error occurred |
| Cat | `E[6-byte-ID]\n` | Cat RFID detected |

**Example Transactions**:
```c
// Request status
→ 'S'
← "AM0L512P512S3\n"  // Mode 0, Light 512, Pos 512, Status 3

// Set night mode
→ 'M' '3'
← "AM3\n"  // Confirm mode 3

// Read light threshold configuration
→ 'C' 'R' 0x00
← "AC0V512\n"  // Config 0, Value 512

// Set light threshold
→ 'C' 'S' 0x00 0x02 0x00  // Value 512 (0x0200) little-endian
← "AC0V512\n"  // Confirm new value
```

## Contribution Workflow

### Before Starting Work

1. **Check Documentation**:
   - README.md - Project overview
   - CODE_ARCHITECTURE.md - Detailed architecture
   - CONTRIBUTING.md - Contribution guidelines
   - TESTING.md - Testing guide

2. **Understand Limitations**:
   - Not in active development (documentation-focused fork)
   - Only bug fixes and testing reports accepted
   - No new features or architectural changes

3. **Set Up Environment**:
   ```bash
   # Install MPLAB X IDE v5.x+
   # Install XC8 Compiler v2.00+
   # Install Ruby and Ceedling for tests
   sudo gem install ceedling
   # Install cppcheck for static analysis
   sudo apt-get install cppcheck
   ```

### Development Process

1. **Create Feature Branch**:
   ```bash
   git checkout -b fix/issue-description
   ```

2. **Make Changes** (follow all standards in this document)

3. **Test Locally**:
   ```bash
   # Build in MPLAB X
   # Run static analysis
   ./lint.sh
   # Run unit tests
   ceedling test:all
   ```

4. **Commit with Clear Messages**:
   ```bash
   git commit -m "Fix: Brief description of what was fixed"
   ```
   
   Good commit messages:
   - "Fix RFID timeout handling in low-light conditions"
   - "Add bounds checking to getCat() function"
   - "Update EEPROM write timing for reliability"
   
   Bad commit messages:
   - "Fixed stuff"
   - "Update"
   - "Changes"

5. **Push and Create PR**:
   - Fill out PR template completely
   - Include testing results (hardware revision, test results)
   - Link to related issue
   - Respond to code review comments

### Code Review Standards

**All PRs must**:
1. Pass all CI checks (build, static analysis, tests)
2. Include documentation updates if needed
3. Have clear, descriptive commit messages
4. Be tested on hardware if possible (or note limitations)
5. Follow all coding standards in this document
6. Not exceed memory constraints (program <90%, RAM <80%)

**Reviewers check for**:
- Code correctness and safety
- Standards compliance
- Test coverage adequacy
- Documentation completeness
- Potential security issues
- Memory usage impact

## Common Patterns and Anti-Patterns

### CORRECT Patterns (use these)

**Non-blocking Delays**:
```c
ms_t start = millis();
while ((millis() - start) < 5000) {
    // Can do other work while waiting
    handleSerial();
    if (condition) break;  // Can exit early
}
```

**Hardware Abstraction**:
```c
// Use meaningful names from peripherials.h
RED_LED = 1;        // Turn on red LED
GREEN_LED = 0;      // Turn off green LED
beep();             // Generate audible feedback
```

**Safe EEPROM Access**:
```c
Cat newCat;
uint8_t slot = saveCat(&newCat);
if (slot > 0) {
    // Success - tag saved in slot 1-16
} else {
    // Failure - EEPROM full or tag already exists
}
```

**Interrupt-Safe Communication**:
```c
// Ring buffer for UART RX (interrupt writes, main reads)
if (byteAvail()) {
    uint8_t byte;
    if (getByte(&byte) == 0) {
        // Process received byte
    }
}
```

### ANTI-PATTERNS (never use these)

**Blocking Delays** ❌:
```c
// WRONG - Stops ALL processing
__delay_ms(5000);  // Don't use this in production code
```

**Direct Port Manipulation** ❌:
```c
// WRONG - Unclear and unmaintainable
PORTB = 0x30;  // What does this do?
```

**Unchecked Returns** ❌:
```c
// WRONG - Ignoring potential errors
readRFID(id, 6, &crc1, &crc2);  // What if it failed?
```

**Magic Numbers** ❌:
```c
// WRONG - No context for what 512 means
if (sensorValue > 512) { /* ... */ }
```

**Uninitialized Variables** ❌:
```c
// WRONG - Undefined behavior
uint8_t result;  // Contains random value
if (result == 0) { /* ... */ }  // Unpredictable
```

**Unbounded Loops** ❌:
```c
// WRONG - Potential infinite loop, no timeout
while (!condition) {
    // What if condition never becomes true?
}
```

## Documentation Requirements

### When to Update Documentation

Update relevant documentation files when:

1. **Adding Features** → Update README.md, CODE_ARCHITECTURE.md
2. **Changing Architecture** → Update CODE_ARCHITECTURE.md
3. **Modifying Tests** → Update TESTING.md
4. **Changing Build Process** → Update CONTRIBUTING.md
5. **Security Changes** → Update SECURITY.md
6. **All Changes** → Update CHANGELOG.md

### Documentation Style

**Be Clear and Concise**:
- Use simple language
- Provide examples
- Include warnings for safety-critical operations
- Link to related resources

**Code Examples in Documentation**:
```markdown
### Example: Reading RFID Tag

```c
Cat detectedCat;
uint16_t crcComputed, crcRead;

if (readRFID(detectedCat.id, 6, &detectedCat.crc, &crcRead) == 0) {
    if (catExists(&detectedCat, &crcRead)) {
        // Valid cat - unlock door
        lockGreenLatch(false);
    }
}
```
```

## Security Considerations

### Authentication and Access Control

**Current Security**:
- RFID tag authentication (EM4100 protocol with CRC)
- No encryption (tags stored in plain text in EEPROM)
- No serial interface authentication
- Code protection optional (disabled by default for debugging)

**Known Limitations**:
- Physical access allows reprogramming
- RFID tags can potentially be cloned
- Serial interface has no authentication
- No audit logging of access events

### Secure Coding Practices

**Input Validation**:
```c
uint8_t processCommand(uint8_t mode) {
    // Validate input range
    if (mode > MODE_OPEN) {
        return ERROR_INVALID_MODE;
    }
    switchMode(mode);
    return SUCCESS;
}
```

**Bounds Checking**:
```c
void getCat(Cat* cat, uint8_t slot) {
    // Check slot range
    if (slot >= CAT_SLOTS) {
        cat->crc = 0;  // Mark as invalid
        return;
    }
    // Safe to access EEPROM
    readEEPROM(CAT_OFFSET + (slot * 8), cat, 8);
}
```

**Safe State on Error**:
```c
// On error, default to safe state (pets can exit)
if (criticalError) {
    lockRedLatch(false);   // Unlock exit
    lockGreenLatch(true);  // Lock entrance for safety
    opMode = MODE_NORMAL;
}
```

## Performance Optimization Guidelines

### Critical Timing Sections

**RFID Reading** (most time-sensitive):
- Bit reading must occur at precise intervals (~2.5ms)
- ADC sampling must be synchronized with signal
- Minimize interrupts during sync header detection

**Solenoid Control** (power-critical):
- Exactly 500ms activation time (too short = incomplete action, too long = overheating)
- Only one solenoid active at a time
- Verify return to safe state after activation

### Memory Optimization

**Use Program Memory for Constants**:
```c
// CORRECT - Stored in Flash, not RAM
const uint8_t MODE_NAMES[] = "Normal Vet Closed Night Learn Clear Open";

// INCORRECT - Wastes precious RAM
// static uint8_t MODE_NAMES[] = "Normal Vet...";
```

**Minimize Global Variables**:
```c
// Only use globals for truly shared state
static uint8_t opMode;        // Needed across functions
static bool outLocked;        // Needed across functions

// Prefer locals for temporary values
void processData(void) {
    uint8_t temp = 0;  // Stack-allocated, freed automatically
}
```

**Reuse Variables Where Safe**:
```c
void function(void) {
    uint8_t buffer[10];
    // Use buffer for RFID
    readRFID(buffer, 10, ...);
    // Reuse same buffer for serial (safe - not concurrent)
    formatSerial(buffer, 10);
}
```

## Testing on Hardware

### Safety Checklist

Before deploying to hardware:

- [ ] **Disconnect power** before programming
- [ ] **Test all modes** thoroughly in safe environment
- [ ] **Verify RFID reading** with actual tags
- [ ] **Check latch operation** (smooth, complete travel)
- [ ] **Test emergency manual override** (mechanical backup works)
- [ ] **Provide alternative access** for pets during testing
- [ ] **Monitor behavior** for unexpected mode changes
- [ ] **Document any modifications** made to hardware or software

### Debugging Techniques

**Serial Debugging**:
```c
// Use printf for debugging (goes to UART)
printf("Mode: %d, Light: %d\n", opMode, light);
printf("RFID Result: %d\n", result);
```

**LED Indicators** (quick visual feedback):
```c
// Flash pattern for debugging
for (int i = 0; i < 3; i++) {
    RED_LED = 1;
    __delay_ms(100);
    RED_LED = 0;
    __delay_ms(100);
}
```

**Oscilloscope Verification**:
- PWM frequency (RC2): Should be 134.2 kHz
- UART signals (RC6/RC7): 38400 baud
- Solenoid timing: 500ms pulses

## Project Status and Maintenance

### Current Status

- **Active Development**: ❌ Not in active development
- **Maintenance**: ⚠️ Limited maintenance (bug fixes only)
- **Testing**: ⚠️ Original testing on rev.X4 hardware only (this fork untested)
- **Documentation**: ✅ Comprehensive documentation maintained

### Contribution Scope

**ACCEPTED**:
- Bug fixes with clear reproduction steps
- Testing reports on different hardware revisions
- Documentation corrections and clarifications

**NOT ACCEPTED**:
- New features or enhancements
- Architectural changes or refactoring
- Support for different microcontrollers
- Active maintenance beyond bug fixes

### Support and Resources

**Documentation**:
- [README.md](../README.md) - Project overview and quick start
- [CODE_ARCHITECTURE.md](../CODE_ARCHITECTURE.md) - Detailed technical documentation
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Contribution guidelines
- [TESTING.md](../TESTING.md) - Complete testing guide
- [DEPLOYMENT.md](../DEPLOYMENT.md) - Hardware deployment guide
- [SECURITY.md](../SECURITY.md) - Security policy and considerations

**Getting Help**:
- GitHub Issues - Bug reports and discussions
- GitHub Discussions - Questions and community help
- Documentation - Comprehensive guides for all aspects

## Summary: Key Principles

When contributing to this project, remember:

1. **Pet Safety First** - Always consider pet welfare in design decisions
2. **Memory Matters** - PIC16F886 has only 368 bytes RAM, be frugal
3. **Test Everything** - Unit tests, static analysis, hardware testing all required
4. **Follow Standards** - Coding conventions, naming, formatting strictly enforced
5. **Document Clearly** - Code should be self-explanatory, comments explain why
6. **Non-Blocking Code** - Use millis() pattern, never block main loop
7. **Hardware Abstraction** - Always use defined macros, never direct port access
8. **Fail-Safe Design** - On error, default to safe state (pets can exit)
9. **Quality Gates** - All CI checks must pass, no exceptions
10. **Professional Standards** - This is production firmware for a safety-critical application

---

**This firmware controls physical access for living animals. Take this responsibility seriously. Test thoroughly. Provide backup access. Never compromise on safety.**

**Last Updated**: December 2024  
**Project**: PetSafe Cat Flap Alternative Firmware  
**Repository**: https://github.com/CJFWeatherhead/PetSafe-CatFlap
