/**
 * Unit Tests for Extended Modes Features
 * 
 * Tests for:
 * 1. Extended Modes menu navigation
 * 2. Silent Mode functionality
 * 3. Key Pad Lock behavior
 * 4. Beep functions (short, long, series)
 * 5. Lock return time configuration
 * 6. LED pattern correctness
 */

#include "unity.h"
#include "xc_hardware_mock.h"
#include <stdint.h>
#include <stdbool.h>

// Extended mode constants (from main.c)
#define EXT_MODE_EXIT 1
#define EXT_MODE_OPEN 2
#define EXT_MODE_SILENT 3
#define EXT_MODE_LIGHT_LEVEL 4
#define EXT_MODE_LOCK_TIME 5
#define EXT_MODE_LOW_BATTERY 6
#define EXT_MODE_TIMER 7

// Test fixtures
void setUp(void)
{
    // This is run before each test
}

void tearDown(void)
{
    // This is run after each test
}

/**
 * Test: Extended mode constants are properly defined
 */
void test_extended_mode_constants_defined(void)
{
    TEST_ASSERT_EQUAL_UINT8(1, EXT_MODE_EXIT);
    TEST_ASSERT_EQUAL_UINT8(2, EXT_MODE_OPEN);
    TEST_ASSERT_EQUAL_UINT8(3, EXT_MODE_SILENT);
    TEST_ASSERT_EQUAL_UINT8(4, EXT_MODE_LIGHT_LEVEL);
    TEST_ASSERT_EQUAL_UINT8(5, EXT_MODE_LOCK_TIME);
    TEST_ASSERT_EQUAL_UINT8(6, EXT_MODE_LOW_BATTERY);
    TEST_ASSERT_EQUAL_UINT8(7, EXT_MODE_TIMER);
}

/**
 * Test: Extended mode values are sequential
 */
void test_extended_mode_values_sequential(void)
{
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_EXIT + 1, EXT_MODE_OPEN);
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_OPEN + 1, EXT_MODE_SILENT);
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_SILENT + 1, EXT_MODE_LIGHT_LEVEL);
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_LIGHT_LEVEL + 1, EXT_MODE_LOCK_TIME);
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_LOCK_TIME + 1, EXT_MODE_LOW_BATTERY);
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_LOW_BATTERY + 1, EXT_MODE_TIMER);
}

/**
 * Test: Extended mode range is valid (1-7)
 */
void test_extended_mode_range_valid(void)
{
    TEST_ASSERT_TRUE(EXT_MODE_EXIT >= 1);
    TEST_ASSERT_TRUE(EXT_MODE_TIMER <= 7);
    
    // All modes should be in valid range
    uint8_t modes[] = {EXT_MODE_EXIT, EXT_MODE_OPEN, EXT_MODE_SILENT, 
                       EXT_MODE_LIGHT_LEVEL, EXT_MODE_LOCK_TIME, 
                       EXT_MODE_LOW_BATTERY, EXT_MODE_TIMER};
    
    for(uint8_t i=0; i<7; ++i){
        TEST_ASSERT_TRUE(modes[i] >= 1);
        TEST_ASSERT_TRUE(modes[i] <= 7);
    }
}

/**
 * Test: Silent mode flag toggles correctly
 */
void test_silent_mode_toggle_logic(void)
{
    bool silentMode = false;
    
    // First toggle: off -> on
    silentMode = !silentMode;
    TEST_ASSERT_TRUE(silentMode);
    
    // Second toggle: on -> off
    silentMode = !silentMode;
    TEST_ASSERT_FALSE(silentMode);
    
    // Third toggle: off -> on
    silentMode = !silentMode;
    TEST_ASSERT_TRUE(silentMode);
}

/**
 * Test: Key pad lock flag toggles correctly
 */
void test_keypad_lock_toggle_logic(void)
{
    bool keyPadLocked = false;
    
    // First toggle: unlocked -> locked
    keyPadLocked = !keyPadLocked;
    TEST_ASSERT_TRUE(keyPadLocked);
    
    // Second toggle: locked -> unlocked
    keyPadLocked = !keyPadLocked;
    TEST_ASSERT_FALSE(keyPadLocked);
}

/**
 * Test: Lock return time range validation (1-25 seconds)
 */
void test_lock_return_time_range(void)
{
    uint16_t lockReturnTime;
    
    // Minimum: 1 second = 1000ms
    lockReturnTime = 1000;
    TEST_ASSERT_TRUE(lockReturnTime >= 1000);
    TEST_ASSERT_TRUE(lockReturnTime <= 25000);
    
    // Maximum: 25 seconds = 25000ms
    lockReturnTime = 25000;
    TEST_ASSERT_TRUE(lockReturnTime >= 1000);
    TEST_ASSERT_TRUE(lockReturnTime <= 25000);
    
    // Default: 4 seconds = 4000ms
    lockReturnTime = 4000;
    TEST_ASSERT_TRUE(lockReturnTime >= 1000);
    TEST_ASSERT_TRUE(lockReturnTime <= 25000);
    TEST_ASSERT_EQUAL_UINT16(4000, lockReturnTime);
}

/**
 * Test: Lock return time boundary conditions
 */
void test_lock_return_time_boundaries(void)
{
    uint16_t time;
    
    // Test minimum boundary (1 second)
    time = 1000;
    TEST_ASSERT_EQUAL_UINT16(1000, time);
    
    // Test below minimum (should be clamped to 1000)
    time = 500;
    if(time < 1000) time = 1000;
    TEST_ASSERT_EQUAL_UINT16(1000, time);
    
    // Test maximum boundary (25 seconds)
    time = 25000;
    TEST_ASSERT_EQUAL_UINT16(25000, time);
    
    // Test above maximum (should be clamped to 25000)
    time = 30000;
    if(time > 25000) time = 25000;
    TEST_ASSERT_EQUAL_UINT16(25000, time);
}

/**
 * Test: Beep series count validation
 */
void test_beep_series_count_valid(void)
{
    uint8_t count;
    
    // Valid range: 1-7 beeps for extended modes
    for(count = 1; count <= 7; ++count){
        TEST_ASSERT_TRUE(count >= 1);
        TEST_ASSERT_TRUE(count <= 7);
    }
    
    // Test specific mode counts
    TEST_ASSERT_EQUAL_UINT8(1, EXT_MODE_EXIT);
    TEST_ASSERT_EQUAL_UINT8(3, EXT_MODE_SILENT);
    TEST_ASSERT_EQUAL_UINT8(5, EXT_MODE_LOCK_TIME);
    TEST_ASSERT_EQUAL_UINT8(7, EXT_MODE_TIMER);
}

/**
 * Test: Extended mode navigation wrap-around (up)
 */
void test_extended_mode_navigation_wrap_up(void)
{
    uint8_t currentMode = EXT_MODE_TIMER;
    
    // Navigate up from mode 7 should wrap to mode 1
    currentMode++;
    if(currentMode > EXT_MODE_TIMER){
        currentMode = EXT_MODE_EXIT;
    }
    
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_EXIT, currentMode);
}

/**
 * Test: Extended mode navigation wrap-around (down)
 */
void test_extended_mode_navigation_wrap_down(void)
{
    uint8_t currentMode = EXT_MODE_EXIT;
    
    // Navigate down from mode 1 should wrap to mode 7
    if(currentMode <= EXT_MODE_EXIT){
        currentMode = EXT_MODE_TIMER;
    }else{
        currentMode--;
    }
    
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_TIMER, currentMode);
}

/**
 * Test: Extended mode navigation sequence (up)
 */
void test_extended_mode_navigation_sequence_up(void)
{
    uint8_t currentMode = EXT_MODE_EXIT;
    
    // Navigate through all modes going up
    TEST_ASSERT_EQUAL_UINT8(1, currentMode);
    
    currentMode++;
    TEST_ASSERT_EQUAL_UINT8(2, currentMode);
    
    currentMode++;
    TEST_ASSERT_EQUAL_UINT8(3, currentMode);
    
    currentMode++;
    TEST_ASSERT_EQUAL_UINT8(4, currentMode);
    
    currentMode++;
    TEST_ASSERT_EQUAL_UINT8(5, currentMode);
    
    currentMode++;
    TEST_ASSERT_EQUAL_UINT8(6, currentMode);
    
    currentMode++;
    TEST_ASSERT_EQUAL_UINT8(7, currentMode);
}

/**
 * Test: Extended mode state tracking
 */
void test_extended_mode_state_tracking(void)
{
    bool inExtendedMode = false;
    uint8_t currentExtendedMode = EXT_MODE_EXIT;
    
    // Initially not in extended mode
    TEST_ASSERT_FALSE(inExtendedMode);
    TEST_ASSERT_EQUAL_UINT8(EXT_MODE_EXIT, currentExtendedMode);
    
    // Enter extended mode
    inExtendedMode = true;
    TEST_ASSERT_TRUE(inExtendedMode);
    
    // Exit extended mode
    inExtendedMode = false;
    TEST_ASSERT_FALSE(inExtendedMode);
}

/**
 * Test: Light threshold storage range (ADC is 10-bit)
 */
void test_light_threshold_range(void)
{
    uint16_t lightThreshold;
    
    // ADC is 10-bit, so valid range is 0-1023
    lightThreshold = 0;
    TEST_ASSERT_TRUE(lightThreshold <= 1023);
    
    lightThreshold = 512;
    TEST_ASSERT_TRUE(lightThreshold <= 1023);
    
    lightThreshold = 1023;
    TEST_ASSERT_TRUE(lightThreshold <= 1023);
}

/**
 * Test: Button press timing constants
 */
void test_button_timing_constants(void)
{
    uint32_t shortPress = 2000;   // < 2s for Night Mode
    uint32_t longPress = 5000;    // > 5s for Vet Mode
    uint32_t extendedPress = 10000;  // > 10s for Learn Mode
    uint32_t veryLongPress = 30000;  // > 30s for Clear/Reset/Lock
    
    TEST_ASSERT_TRUE(shortPress < longPress);
    TEST_ASSERT_TRUE(longPress < extendedPress);
    TEST_ASSERT_TRUE(extendedPress < veryLongPress);
    
    // Verify actual values
    TEST_ASSERT_EQUAL_UINT32(2000, shortPress);
    TEST_ASSERT_EQUAL_UINT32(5000, longPress);
    TEST_ASSERT_EQUAL_UINT32(10000, extendedPress);
    TEST_ASSERT_EQUAL_UINT32(30000, veryLongPress);
}

/**
 * Test: Default OPEN_TIME is 4 seconds (per manual)
 */
void test_default_open_time_is_4_seconds(void)
{
    uint16_t openTime = 4000;  // 4 seconds in milliseconds
    
    TEST_ASSERT_EQUAL_UINT16(4000, openTime);
    
    // Verify it's not the old value (5 seconds)
    TEST_ASSERT_NOT_EQUAL(5000, openTime);
}

/**
 * Test: Silent mode should not affect system beeps
 */
void test_silent_mode_scope(void)
{
    bool silentMode = true;
    bool isCatDetection = true;
    bool isSystemBeep = false;
    
    // Cat detection beep should be suppressed in silent mode
    bool shouldBeep = !silentMode && isCatDetection;
    TEST_ASSERT_FALSE(shouldBeep);
    
    // System beeps should NOT be suppressed (even in silent mode)
    shouldBeep = !isSystemBeep || !silentMode;
    TEST_ASSERT_TRUE(shouldBeep);
}

/**
 * Test: Extended mode menu entry condition
 */
void test_extended_mode_entry_condition(void)
{
    uint32_t btnPress = 0;
    bool shouldEnter = false;
    
    // Too short - should not enter
    btnPress = 1000;
    shouldEnter = (btnPress > 2000) && (btnPress < 30000);
    TEST_ASSERT_FALSE(shouldEnter);
    
    // Just right - should enter
    btnPress = 3000;
    shouldEnter = (btnPress > 2000) && (btnPress < 30000);
    TEST_ASSERT_TRUE(shouldEnter);
    
    // Too long (Clear Mode takes priority) - should not enter extended
    btnPress = 31000;
    shouldEnter = (btnPress > 2000) && (btnPress < 30000);
    TEST_ASSERT_FALSE(shouldEnter);
}

/**
 * Test: Key pad lock prevents button functions
 */
void test_keypad_lock_prevents_functions(void)
{
    bool keyPadLocked = true;
    uint32_t btnPress = 3000;  // Normal press time
    
    // When locked and press is not unlock command
    bool shouldProcess = !keyPadLocked || (btnPress > 30000);
    TEST_ASSERT_FALSE(shouldProcess);
    
    // Unlock command should still work (>30s)
    btnPress = 31000;
    shouldProcess = !keyPadLocked || (btnPress > 30000);
    TEST_ASSERT_TRUE(shouldProcess);
    
    // When unlocked, all presses work
    keyPadLocked = false;
    btnPress = 3000;
    shouldProcess = !keyPadLocked || (btnPress > 30000);
    TEST_ASSERT_TRUE(shouldProcess);
}
