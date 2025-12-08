# Testing Guide for PetSafe Cat Flap Firmware

This document provides comprehensive information about testing the PetSafe Cat Flap firmware, including unit tests, integration tests, static analysis, and CI/CD workflows.

## Table of Contents

1. [Overview](#overview)
2. [Test Infrastructure](#test-infrastructure)
3. [Running Tests](#running-tests)
4. [Writing Tests](#writing-tests)
5. [Static Analysis](#static-analysis)
6. [CI/CD Pipeline](#cicd-pipeline)
7. [Hardware Testing](#hardware-testing)
8. [Troubleshooting](#troubleshooting)

---

## Overview

### Testing Strategy

The testing strategy for this embedded firmware project includes:

1. **Unit Tests**: Test individual modules in isolation
2. **Static Analysis**: Automated code quality checks
3. **Build Verification**: Ensure code compiles with XC8 compiler
4. **Hardware Testing**: Manual verification on actual hardware

### Test Framework

We use **Ceedling**, which integrates:
- **Unity**: Unit testing framework for C
- **CMock**: Automatic mock generation
- **GCov**: Code coverage analysis

---

## Test Infrastructure

### Prerequisites

To run tests locally, install:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y gcc make ruby

# Install Ceedling
sudo gem install ceedling

# For static analysis
sudo apt-get install -y cppcheck
```

### Directory Structure

```
PetSafe-CatFlap/
├── test/
│   ├── test_cat.c          # Cat module tests
│   ├── test_rfid.c         # RFID module tests
│   ├── test_serial.c       # Serial communication tests
│   └── support/
│       ├── xc_mock.h       # Hardware register mocks
│       └── xc_mock.c       # Mock implementations
├── project.yml             # Ceedling configuration
└── .github/
    └── workflows/
        └── ci.yml          # CI/CD pipeline
```

### Configuration

The test environment is configured in `project.yml`:

- **Compiler**: GCC (for host testing)
- **Standards**: C99 with strict warnings
- **Coverage**: GCov HTML reports
- **Mocks**: CMock with callback support

---

## Running Tests

### Run All Tests

```bash
# From project root
ceedling test:all
```

This will:
1. Compile all test files
2. Run all unit tests
3. Generate test report
4. Display results

### Run Specific Tests

```bash
# Test a specific module
ceedling test:cat
ceedling test:rfid
ceedling test:serial

# Run with verbose output
ceedling test:all -v
```

### Generate Code Coverage

```bash
# Run tests with coverage
ceedling gcov:all

# View HTML coverage report
# Open: build/artifacts/gcov/GcovCoverageResults.html
```

### Clean Test Artifacts

```bash
ceedling clean
```

---

## Writing Tests

### Test File Naming

- Test files must start with `test_` prefix
- Example: `test_cat.c` tests the `cat.c` module

### Basic Test Structure

```c
#include "unity.h"
#include "module_to_test.h"

// Setup runs before each test
void setUp(void)
{
    // Initialize test fixtures
}

// Teardown runs after each test
void tearDown(void)
{
    // Clean up
}

// Test functions start with test_
void test_function_name(void)
{
    // Arrange: Set up test data
    int expected = 42;
    
    // Act: Call function under test
    int actual = functionToTest();
    
    // Assert: Verify results
    TEST_ASSERT_EQUAL_INT(expected, actual);
}
```

### Common Unity Assertions

```c
// Integer comparisons
TEST_ASSERT_EQUAL_INT(expected, actual);
TEST_ASSERT_EQUAL_UINT8(expected, actual);
TEST_ASSERT_EQUAL_UINT16(expected, actual);

// Boolean
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);

// Memory
TEST_ASSERT_EQUAL_MEMORY(expected, actual, size);

// Pointers
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);

// Floating point
TEST_ASSERT_EQUAL_FLOAT(expected, actual);

// Size
TEST_ASSERT_EQUAL_size_t(expected, actual);

// Comparison
TEST_ASSERT_LESS_THAN(threshold, actual);
TEST_ASSERT_GREATER_THAN(threshold, actual);
```

### Testing Hardware-Dependent Code

For code that accesses hardware registers:

```c
// Use mocks in test/support/xc_mock.h
#include "xc_mock.h"

void test_hardware_function(void)
{
    // Set up mock register state
    PORTA = 0x00;
    
    // Call function that modifies PORTA
    setLED(true);
    
    // Verify register was modified correctly
    TEST_ASSERT_EQUAL_UINT8(0x01, PORTA);
}
```

### Test Organization

Group related tests:

```c
// Test suite for initialization
void test_init_default_values(void) { }
void test_init_with_parameters(void) { }
void test_init_error_handling(void) { }

// Test suite for operations
void test_operation_success(void) { }
void test_operation_failure(void) { }
void test_operation_edge_cases(void) { }
```

---

## Static Analysis

### Using Cppcheck

Run static analysis on source code:

```bash
cppcheck --enable=all \
         --inconclusive \
         --std=c99 \
         --suppress=missingIncludeSystem \
         --suppress=unusedFunction \
         *.c
```

### Common Issues Detected

- **Memory leaks**: Unfreed memory
- **Buffer overflows**: Array bounds violations
- **Null pointer dereferences**: Unsafe pointer usage
- **Uninitialized variables**: Variables used before initialization
- **Type mismatches**: Incorrect type conversions

### Suppressing False Positives

Add inline suppressions for false positives:

```c
// cppcheck-suppress unusedFunction
static void internalHelper(void) {
    // This is called indirectly
}
```

---

## CI/CD Pipeline

### GitHub Actions Workflow

The CI pipeline (`.github/workflows/ci.yml`) runs automatically on:
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`
- Manual trigger via workflow dispatch

### Pipeline Stages

#### 1. Build with XC8 Compiler

```yaml
jobs:
  build:
    - Install XC8 compiler
    - Compile all source files
    - Upload build artifacts
```

**Purpose**: Verify firmware compiles for target PIC16F886

#### 2. Static Analysis

```yaml
jobs:
  static-analysis:
    - Install cppcheck
    - Run static analysis
    - Upload analysis report
```

**Purpose**: Find potential code quality issues

#### 3. Unit Tests

```yaml
jobs:
  test:
    - Install Ceedling
    - Run unit tests
    - Upload test results
```

**Purpose**: Verify code functionality

### Viewing CI Results

1. Go to repository on GitHub
2. Click "Actions" tab
3. Select a workflow run
4. View job logs and artifacts

### Local CI Simulation

Test the workflow locally before pushing:

```bash
# Install act (GitHub Actions local runner)
# https://github.com/nektos/act

# Run workflow locally
act push
```

---

## Hardware Testing

### Prerequisites

- PIC programmer (PICkit 3/4, ICD 3, or Snap)
- PetSafe Pet Porte PCB
- Serial adapter (optional, for debugging)
- MPLAB X IDE with XC8 compiler

### Testing Checklist

See [CONTRIBUTING.md](CONTRIBUTING.md) for complete hardware testing checklist.

**Quick checklist**:

- [ ] Firmware compiles successfully
- [ ] Firmware programs to device
- [ ] Device powers up correctly
- [ ] All modes work correctly
- [ ] RFID reading functions
- [ ] LEDs indicate proper status
- [ ] Buttons respond correctly
- [ ] Serial communication works (if used)

### Serial Debugging

Connect serial adapter to UART pins:

```bash
# Linux
screen /dev/ttyUSB0 38400

# macOS
screen /dev/tty.usbserial 38400

# Windows (PuTTY)
# Port: COM3, Baud: 38400, 8N1
```

Send commands:
- `S` - Request status
- `M0` - Set mode 0 (Normal)
- `M3` - Set mode 3 (Night)

### Safety Reminders

⚠️ **ALWAYS:**
- Disconnect power before programming
- Test thoroughly before deploying
- Provide alternative access for pets
- Never test on pets without fallback

---

## Troubleshooting

### Tests Fail to Build

**Problem**: Compilation errors in tests

**Solution**:
```bash
# Clean and rebuild
ceedling clean
ceedling test:all

# Check for missing includes
# Verify mock headers in test/support/
```

### Test Failures

**Problem**: Tests run but fail assertions

**Solution**:
1. Read the failure message carefully
2. Check expected vs. actual values
3. Verify test logic and assumptions
4. Run single test for debugging:
   ```bash
   ceedling test:module_name
   ```

### XC8 Compiler Not Found (CI)

**Problem**: CI fails to find XC8 compiler

**Solution**:
- Verify XC8 installation path in workflow
- Check download URL is still valid
- Ensure installer has execute permissions

### Ceedling Command Not Found

**Problem**: `ceedling` command not found

**Solution**:
```bash
# Reinstall Ceedling
sudo gem install ceedling

# Check Ruby version
ruby --version  # Should be 2.5 or later

# Add gem bin to PATH
export PATH="$PATH:$(ruby -e 'puts Gem.user_dir')/bin"
```

### Hardware Tests Pass but Device Fails

**Problem**: Unit tests pass but hardware doesn't work

**Solution**:
- Unit tests don't verify hardware interaction
- Use MPLAB X simulator for integration testing
- Test on actual hardware with programmer
- Use serial debugging to diagnose issues

### Coverage Report Not Generated

**Problem**: GCov reports missing

**Solution**:
```bash
# Ensure gcov plugin is enabled in project.yml
# Run coverage explicitly
ceedling gcov:all

# Check build/artifacts/gcov/ directory
ls -l build/artifacts/gcov/
```

---

## Best Practices

### Test-Driven Development (TDD)

1. **Red**: Write failing test first
2. **Green**: Write minimal code to pass
3. **Refactor**: Improve code while keeping tests green

### Test Independence

- Each test should be independent
- Use `setUp()` and `tearDown()` for isolation
- Don't rely on test execution order

### Meaningful Test Names

```c
// Good
void test_rfid_returns_error_on_bad_crc(void)

// Bad
void test_rfid_1(void)
```

### Test Coverage Goals

- **Critical functions**: 100% coverage
- **Business logic**: 80%+ coverage
- **Hardware abstraction**: Mock and test interfaces
- **Edge cases**: Always test boundary conditions

### Continuous Integration

- Run tests before every commit
- Fix failing tests immediately
- Keep main branch always buildable
- Review coverage reports regularly

---

## Resources

### Documentation

- [Unity Testing Framework](https://github.com/ThrowTheSwitch/Unity)
- [Ceedling Documentation](https://github.com/ThrowTheSwitch/Ceedling)
- [CMock User Guide](https://github.com/ThrowTheSwitch/CMock)

### PIC16F886 Resources

- [PIC16F886 Datasheet](https://www.microchip.com/en-us/product/PIC16F886)
- [XC8 Compiler User Guide](https://microchip.com/xc8)
- [MPLAB X IDE Documentation](https://www.microchip.com/mplab/mplab-x-ide)

### Project Documentation

- [README.md](README.md) - Project overview
- [CODE_ARCHITECTURE.md](CODE_ARCHITECTURE.md) - Code structure
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guide
- [DEPLOYMENT.md](DEPLOYMENT.md) - Hardware deployment

---

## Support

### Getting Help

- **Issues**: [GitHub Issues](https://github.com/CJFWeatherhead/PetSafe-CatFlap/issues)
- **Discussions**: [GitHub Discussions](https://github.com/CJFWeatherhead/PetSafe-CatFlap/discussions)

### Contributing Tests

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:
- Writing new tests
- Improving test coverage
- Reporting test failures
- Sharing testing results

---

**Last Updated**: December 2024
