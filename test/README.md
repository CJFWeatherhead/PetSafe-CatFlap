# Test Directory

This directory contains unit tests for the PetSafe Cat Flap firmware.

## Structure

```
test/
├── test_cat.c          # Tests for cat.c (EEPROM tag storage)
├── test_rfid.c         # Tests for rfid.c (RFID reader)
├── test_serial.c       # Tests for serial.c (UART communication)
├── support/            # Test support files
│   ├── xc_mock.h       # Mock hardware registers
│   └── xc_mock.c       # Mock register implementations
└── README.md           # This file
```

## Running Tests

### Prerequisites

Install Ceedling (Ruby-based test framework):

```bash
sudo gem install ceedling
```

### Run All Tests

```bash
# From project root
ceedling test:all
```

### Run Individual Module Tests

```bash
ceedling test:cat      # Test cat module
ceedling test:rfid     # Test RFID module
ceedling test:serial   # Test serial module
```

### Generate Coverage Report

```bash
ceedling gcov:all
# Open build/artifacts/gcov/GcovCoverageResults.html
```

## Writing Tests

### Test File Naming

Test files must:
- Start with `test_` prefix
- Match source file name: `test_<module>.c`

### Example Test

```c
#include "unity.h"
#include "module.h"

void setUp(void) {
    // Run before each test
}

void tearDown(void) {
    // Run after each test
}

void test_function_name(void) {
    // Arrange
    int expected = 42;
    
    // Act
    int result = functionToTest();
    
    // Assert
    TEST_ASSERT_EQUAL_INT(expected, result);
}
```

## Test Support

### Hardware Mocks

The `support/` directory contains mock implementations of PIC16F886 hardware registers:

- `xc_mock.h`: Declares mock registers (PORTA, PORTB, etc.)
- `xc_mock.c`: Defines mock register variables

This allows testing hardware-dependent code on non-embedded platforms.

### Using Mocks in Tests

```c
#include "unity.h"
#include "xc_mock.h"

void test_hardware_function(void) {
    // Set up mock register
    PORTA = 0x00;
    
    // Call function that modifies hardware
    setLED(true);
    
    // Verify register was modified
    TEST_ASSERT_EQUAL_UINT8(0x01, PORTA);
}
```

## Current Test Coverage

### Modules with Tests

- ✅ **cat.c**: Data structure and interface tests
- ✅ **rfid.c**: Error codes and API contract tests
- ✅ **serial.c**: Buffer and configuration tests

### Modules Needing Tests

The following modules would benefit from additional tests:

- **peripherals.c**: Hardware I/O control
- **interrupts.c**: Interrupt handlers
- **user.c**: System initialization
- **main.c**: Application logic (integration tests)

## Testing Limitations

### Hardware-Dependent Code

Some functions require actual PIC16F886 hardware or simulator:

- EEPROM read/write operations
- RFID reader hardware interaction
- Interrupt service routines
- ADC conversions
- PWM generation

For these, we test:
1. **Data structures**: Verify correct size and layout
2. **API contracts**: Test interfaces and error codes
3. **Logic**: Test business logic separate from hardware

### Integration Testing

Full integration testing requires:
- MPLAB X Simulator
- Actual hardware with PIC programmer
- RFID test tags
- Serial monitoring

## Continuous Integration

Tests run automatically on:
- Every push to main/develop
- Every pull request
- Manual workflow dispatch

View CI status: https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions

## Contributing Tests

When adding new code:

1. **Write tests first** (TDD approach)
2. **Test edge cases** and error conditions
3. **Aim for coverage**: 80%+ for business logic
4. **Mock hardware**: Use xc_mock for register access
5. **Independent tests**: Each test should be self-contained

See [TESTING.md](../TESTING.md) for complete testing guide.

## Resources

- [Unity Documentation](https://github.com/ThrowTheSwitch/Unity)
- [Ceedling User Guide](https://github.com/ThrowTheSwitch/Ceedling)
- [TESTING.md](../TESTING.md) - Complete testing guide
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Contribution guidelines

## Support

Questions about testing? Open an issue or discussion on GitHub.
