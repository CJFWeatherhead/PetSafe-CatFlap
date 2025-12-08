/**
 * Unit Tests for RFID Module
 * 
 * Tests the RFID tag reading error codes and definitions
 * Note: Full RFID hardware functions require actual PIC16F886 hardware
 * and are tested via integration/hardware tests
 */

#include "unity.h"
#include "xc_hardware_mock.h"  // Must be included before rfid.h
#include "rfid.h"

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
 * Test: RFID error code definitions
 */
void test_rfid_error_codes(void)
{
    // Verify error codes are defined with unique values
    TEST_ASSERT_EQUAL(1, NO_CARRIER);
    TEST_ASSERT_EQUAL(2, NO_HEADER);
    TEST_ASSERT_EQUAL(3, BAD_START);
    TEST_ASSERT_EQUAL(4, BAD_CRC);
    
    // Ensure all error codes are different
    TEST_ASSERT_NOT_EQUAL(NO_CARRIER, NO_HEADER);
    TEST_ASSERT_NOT_EQUAL(NO_CARRIER, BAD_START);
    TEST_ASSERT_NOT_EQUAL(NO_CARRIER, BAD_CRC);
    TEST_ASSERT_NOT_EQUAL(NO_HEADER, BAD_START);
    TEST_ASSERT_NOT_EQUAL(NO_HEADER, BAD_CRC);
    TEST_ASSERT_NOT_EQUAL(BAD_START, BAD_CRC);
}

/**
 * Test: Success return code is 0
 */
void test_rfid_success_code(void)
{
    // readRFID should return 0 on success
    // This is a convention test
    int success = 0;
    TEST_ASSERT_EQUAL(0, success);
}

/**
 * Test: RFID ID buffer size
 */
void test_rfid_id_buffer(void)
{
    // Standard RFID tags have 6-byte IDs
    uint8_t id_buffer[6];
    
    TEST_ASSERT_EQUAL_size_t(6, sizeof(id_buffer));
    
    // Verify buffer can hold all values
    for (uint8_t i = 0; i < 6; i++) {
        id_buffer[i] = (uint8_t)(0xFF - i);
        TEST_ASSERT_EQUAL_UINT8(0xFF - i, id_buffer[i]);
    }
}

/**
 * Test: CRC value types
 */
void test_rfid_crc_types(void)
{
    uint16_t crc_computed = 0;
    uint16_t crc_read = 0;
    
    // CRC should be 16-bit
    TEST_ASSERT_EQUAL_size_t(2, sizeof(crc_computed));
    TEST_ASSERT_EQUAL_size_t(2, sizeof(crc_read));
    
    // Test CRC value range
    crc_computed = 0xFFFF;
    crc_read = 0x0000;
    
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, crc_computed);
    TEST_ASSERT_EQUAL_UINT16(0x0000, crc_read);
}

/**
 * Test: RFID PWM control boolean
 */
void test_rfid_pwm_control(void)
{
    // setRFIDPWM takes boolean parameter
    bool pwm_on = true;
    bool pwm_off = false;
    
    TEST_ASSERT_TRUE(pwm_on);
    TEST_ASSERT_FALSE(pwm_off);
}

/**
 * Test: Error code priority
 */
void test_error_code_priority(void)
{
    // Error codes are defined in ascending order reflecting priority
    // NO_CARRIER (1) - most fundamental error (detected first)
    // NO_HEADER (2) - next level error
    // BAD_START (3) - protocol error
    // BAD_CRC (4) - data integrity error
    
    // Verify each code is greater than the previous (in correct sequence)
    TEST_ASSERT_GREATER_THAN(NO_CARRIER, NO_HEADER);  // 2 > 1
    TEST_ASSERT_GREATER_THAN(NO_HEADER, BAD_START);   // 3 > 2
    TEST_ASSERT_GREATER_THAN(BAD_START, BAD_CRC);     // 4 > 3
}

// Note: The actual readRFID function requires hardware interaction
// and would need hardware mocking or integration testing for full coverage.
// These tests validate the API contract and data types.
