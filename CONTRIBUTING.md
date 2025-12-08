# Contributing to PetSafe Cat Flap Firmware

Thank you for your interest in this project!

## Important: Project Scope and Maintenance

**This is a documentation-focused fork** of the original project. Please understand:

- **Not in active development**: No new features are planned
- **Limited maintenance**: No long-term maintenance commitment
- **No responsibility**: The repository owner does not take responsibility for this codebase
- **Original testing only**: Code was tested by the original developer on **rev.X4 hardware only**
- **Fork status**: This fork has **not been tested** on any hardware

## Acceptable Contributions

We welcome **limited** contributions in these areas:

1. **Bug Fixes**: Code corrections for identified issues
2. **Testing Reports**: Share your testing results on different hardware revisions
3. **Documentation**: Corrections and clarifications to existing documentation

## Not Accepting

- New features or enhancements
- Architectural changes
- Support for different microcontrollers
- Major refactoring

If you want to add significant features, please consider forking the project.

## Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates.

**When submitting a bug report, include**:

- **Clear title and description**
- **Steps to reproduce** the issue
- **Hardware configuration**:
  - PCB revision (X4, X5, etc.)
  - PIC programmer model
  - Any modifications made
- **Software versions**:
  - MPLAB X IDE version
  - XC8 compiler version
  - Firmware commit hash
- **Expected behavior** vs. actual behavior
- **Error messages** or unusual behavior
- **Photos or screenshots** if relevant
- **Serial output** if available

## Sharing Testing Reports

Testing reports are highly valuable! Please open an issue with:

- **Hardware revision**: Specify your PCB version
- **Test results**: What worked, what didn't
- **Modifications needed**: Any changes required for your hardware
- **Photos**: Hardware setup if helpful

## Submitting Bug Fixes

If you have a fix for a bug:

1. **Fork** the repository
2. **Create a branch** for your fix (`git checkout -b fix/issue-description`)
3. **Make minimal changes** - only fix the specific bug
4. **Test on hardware** if possible
5. **Document your testing** in the pull request
6. **Commit with clear message** describing the fix
7. **Push to your fork** (`git push origin fix/issue-description`)
8. **Open a Pull Request** with:
   - Description of the bug
   - How your fix addresses it
   - Testing performed (hardware revision, test results)
   - Any side effects or considerations

## Development Setup

### Prerequisites

1. **Install development tools**:
   - MPLAB X IDE v5.x or later
   - XC8 Compiler v2.00 or later
   - Git for version control

2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/PetSafe-CatFlap.git
   cd PetSafe-CatFlap
   git remote add upstream https://github.com/CJFWeatherhead/PetSafe-CatFlap.git
   ```

3. **Open in MPLAB X**:
   - File → Open Project
   - Select the cloned directory
   - Build to verify setup (F11)

### Hardware Setup for Testing

If you have hardware for testing:

1. **PIC programmer** (PICkit 3, PICkit 4, ICD 3, or compatible)
2. **Test board** or actual PetSafe Pet Porte PCB
3. **Serial adapter** for debugging (optional but recommended)
4. **Multimeter** for voltage verification

### Development Workflow

1. **Keep your fork synchronized**:
   ```bash
   git fetch upstream
   git checkout main
   git merge upstream/main
   ```

2. **Create feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make changes and test**

4. **Commit with meaningful messages**:
   ```bash
   git add .
   git commit -m "Add feature: brief description"
   ```

5. **Push and create PR**:
   ```bash
   git push origin feature/your-feature-name
   ```

## Coding Standards

### C Code Style

Follow these conventions for consistency:

#### File Organization

```c
/* File header comment */
/* 
 * File:   filename.c
 * Author: your-name
 * Description: Brief description
 * Created: Date
 */

/* System includes */
#include <xc.h>
#include <stdint.h>

/* Project includes */
#include "header.h"

/* Defines */
#define CONSTANT_NAME 123

/* Type definitions */
typedef struct { ... } MyStruct;

/* Static/private variables */
static uint8_t privateVar = 0;

/* Function prototypes (if needed) */
static void privateFunction(void);

/* Function implementations */
void publicFunction(void) {
    // Implementation
}
```

#### Naming Conventions

- **Functions**: camelCase → `readSensor()`, `initPeripherals()`
- **Variables**: camelCase → `sensorValue`, `isEnabled`
- **Constants/Macros**: UPPER_CASE → `MAX_BUFFER_SIZE`, `LED_PIN`
- **Types**: PascalCase → `SensorData`, `ConfigSettings`
- **Private/static**: Prefix with `static` keyword

#### Code Formatting

```c
// Use spaces, not tabs (4 spaces per indent)
void exampleFunction(uint8_t param) {
    if (condition) {
        // Code here
    } else {
        // Alternative code
    }
    
    // Braces on same line for if/while/for
    while (running) {
        doSomething();
    }
    
    // Space after keywords
    for (int i = 0; i < 10; i++) {
        process(i);
    }
}
```

#### Comments

```c
/**
 * Brief function description
 * @param input Description of parameter
 * @return Description of return value
 */
uint8_t calculateValue(uint8_t input) {
    // Implementation comments where needed
    uint8_t result = 0;
    
    // Explain complex logic
    if (complexCondition) {
        // Why this approach was chosen
        result = complexCalculation();
    }
    
    return result;
}

// Use single-line comments for brief notes
// Multi-line comments for explanations
```

### Hardware Abstraction

When accessing hardware, use meaningful names:

```c
// Good - uses defined macros
RED_LED = 1;
GREEN_LED = 0;

// Avoid direct port access without macros
// PORTB = 0x10;  // Bad - unclear what this does
```

### Memory Considerations

The PIC16F886 has limited resources:

- **RAM**: 368 bytes - use sparingly
- **Flash**: 8K words - optimize code size
- **EEPROM**: 256 bytes - for persistent data only

**Best practices**:

```c
// Prefer local variables
void function(void) {
    uint8_t local = 0;  // On stack
}

// Use static for file-scope variables
static uint8_t fileScope = 0;

// Mark constants to store in program memory
const uint8_t lookup[] = {1, 2, 3};

// Use appropriate data types
uint8_t counter;     // 0-255
int8_t offset;       // -128 to 127
uint16_t largeVal;   // 0-65535
bool flag;           // true/false
```

### Error Handling

```c
// Return error codes
#define SUCCESS 0
#define ERROR_INVALID_PARAM 1
#define ERROR_TIMEOUT 2

uint8_t performOperation(void) {
    if (invalidCondition) {
        return ERROR_INVALID_PARAM;
    }
    
    // Operation
    
    return SUCCESS;
}

// Check return values
uint8_t result = performOperation();
if (result != SUCCESS) {
    // Handle error
}
```

## Submitting Changes

### Commit Messages

Write clear, descriptive commit messages:

```
Short summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain what changed and why, not how (code shows how).

- Bullet points are okay
- Use present tense: "Add feature" not "Added feature"
- Reference issues: "Fixes #123"
```

**Examples**:

- ✅ Good: `Add night mode auto-lock feature`
- ✅ Good: `Fix RFID reading timeout issue (#45)`
- ✅ Good: `Update documentation for flap position sensor`
- ❌ Bad: `Fixed stuff`
- ❌ Bad: `Update`
- ❌ Bad: `Changed main.c`

### Pull Request Process

1. **Update documentation** if you change functionality
2. **Add comments** to complex code sections
3. **Test on hardware** if possible
4. **Update CHANGELOG.md** with your changes
5. **Fill out PR template** completely
6. **Respond to review comments** promptly

### PR Template

When opening a PR, include:

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactoring

## Testing
- [ ] Tested on hardware
- [ ] Tested in simulator
- [ ] All modes verified
- [ ] Serial communication tested

## Hardware Configuration
- Programmer: PICkit 3 / PICkit 4 / ICD 3 / Other
- PCB Revision: X4 / Other
- Modifications: None / Describe

## Checklist
- [ ] Code follows project style guidelines
- [ ] Comments added for complex sections
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] Tested thoroughly

## Related Issues
Fixes #(issue number)
```

## Testing

### Automated Testing

All contributions should pass automated tests:

1. **Unit Tests**: Run `ceedling test:all`
   ```bash
   # Install Ceedling
   sudo gem install ceedling
   
   # Run tests
   ceedling test:all
   ```

2. **Static Analysis**: Code is automatically checked with cppcheck
   ```bash
   cppcheck --enable=all --std=c99 *.c
   ```

3. **CI/CD Pipeline**: All pull requests trigger automated:
   - Build with XC8 compiler
   - Static code analysis
   - Unit test execution

See **[TESTING.md](TESTING.md)** for complete testing guide.

### Required Testing

Before submitting changes:

1. **Build verification**:
   ```
   Clean and Build Project (Shift+F11)
   Verify: BUILD SUCCESSFUL
   ```

2. **Unit tests**: Run and verify all tests pass
   ```bash
   ceedling test:all
   # Ensure: ALL TESTS PASSED
   ```

3. **Code size check**:
   - Ensure program memory usage < 90% (leave room for future features)
   - Check RAM usage < 80%

4. **Functional testing** (if hardware available):
   - [ ] All modes work correctly
   - [ ] RFID reading functions
   - [ ] LEDs indicate proper status
   - [ ] Buttons respond correctly
   - [ ] Latches operate smoothly
   - [ ] Serial communication works (if applicable)

### Testing Checklist

```markdown
## Basic Functionality
- [ ] Firmware compiles without errors
- [ ] Firmware programs successfully
- [ ] Device powers up correctly
- [ ] LEDs respond to mode changes

## RFID Testing
- [ ] Tag reading works
- [ ] Learn mode accepts new tags
- [ ] Stored tags are recognized
- [ ] Clear mode removes tags

## Mode Testing
- [ ] Normal mode operates correctly
- [ ] Vet mode locks properly
- [ ] Night mode responds to light
- [ ] Learn mode accepts tags
- [ ] Clear mode erases tags
- [ ] Open mode allows free access

## Button Testing
- [ ] Red button short press
- [ ] Red button long press
- [ ] Green button long press
- [ ] Both buttons long press

## Optional Features
- [ ] Flap position sensing (if FLAP_POT enabled)
- [ ] Serial commands respond correctly
- [ ] Configuration parameters save/load
```

### Simulation Testing

If hardware unavailable:

1. **Use MPLAB X Simulator**:
   - Set breakpoints to verify logic
   - Step through code
   - Monitor register values
   - Verify timing calculations

2. **Static analysis**:
   - Review code for logical errors
   - Check for buffer overflows
   - Verify pointer usage
   - Check for uninitialized variables

## Documentation

### Documentation Standards

When contributing, update relevant documentation:

1. **Code comments**: Explain complex logic
2. **README.md**: Update if adding features
3. **DEPLOYMENT.md**: Update if changing hardware/software requirements
4. **CHANGELOG.md**: Document all changes
5. **API documentation**: Update if changing interfaces

### Documentation Style

- Use clear, simple language
- Assume reader is unfamiliar with project
- Provide examples where helpful
- Include warnings for safety-critical operations
- Link to relevant resources

### Adding Examples

When documenting features:

```markdown
## Feature Name

Brief description of what it does.

### Usage

```c
// Example code
initFeature();
useFeature();
```

### Parameters

- `param1`: Description
- `param2`: Description

### Returns

Description of return value

### Example

Practical example showing typical usage
```
```

## Community

### Getting Help

- **Documentation**: Check [DEPLOYMENT.md](DEPLOYMENT.md) first
- **Existing Issues**: Search before opening new ones
- **Discussions**: Use GitHub Discussions for questions
- **Contact**: Open an issue for project-specific questions

### Providing Help

- Answer questions in issues/discussions
- Review pull requests
- Improve documentation
- Test on different hardware configurations
- Share your experience and tips

### Recognition

Contributors will be recognized in:
- Project README
- Release notes
- Commit history

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (The Unlicense). See [LICENSE](LICENSE) for details.

## Questions?

If you have questions about contributing:

1. Check this guide thoroughly
2. Search existing issues and discussions
3. Open a new discussion or issue if still unclear

Thank you for contributing to making cat flaps smarter! 🐱

---

**Remember**: Test on hardware when possible, and never risk your pet's safety!
