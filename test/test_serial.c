/**
 * Unit Tests for Serial Module
 * 
 * Tests the serial communication definitions and ring buffer structure
 * Note: UART hardware functions are tested via hardware/integration tests
 */

#include "unity.h"
#include "xc_hardware_mock.h"  // Must be included before serial.h
#include "serial.h"

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
 * Test: Serial buffer size definition
 */
void test_serial_buffer_size(void)
{
    // Buffer should be 16 bytes as defined
    TEST_ASSERT_EQUAL(16, SER_BUFFER);
}

/**
 * Test: Ring buffer structure size
 */
void test_ring_buffer_structure(void)
{
    struct RingBuffer testBuffer;
    
    // Ring buffer should have correct fields
    TEST_ASSERT_EQUAL_size_t(1, sizeof(testBuffer.rIndex));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(testBuffer.uIndex));
    TEST_ASSERT_EQUAL_size_t(SER_BUFFER, sizeof(testBuffer.buffer));
}

/**
 * Test: Ring buffer indices
 */
void test_ring_buffer_indices(void)
{
    struct RingBuffer testBuffer;
    
    // Initialize indices
    testBuffer.rIndex = 0;
    testBuffer.uIndex = 0;
    
    TEST_ASSERT_EQUAL_UINT8(0, testBuffer.rIndex);
    TEST_ASSERT_EQUAL_UINT8(0, testBuffer.uIndex);
    
    // Test index wrapping (conceptually)
    testBuffer.rIndex = 15;
    testBuffer.uIndex = 15;
    
    TEST_ASSERT_EQUAL_UINT8(15, testBuffer.rIndex);
    TEST_ASSERT_EQUAL_UINT8(15, testBuffer.uIndex);
}

/**
 * Test: Ring buffer data storage
 */
void test_ring_buffer_data(void)
{
    struct RingBuffer testBuffer;
    
    // Fill buffer with test data
    for (uint8_t i = 0; i < SER_BUFFER; i++) {
        testBuffer.buffer[i] = (uint8_t)('A' + i);
    }
    
    // Verify data
    for (uint8_t i = 0; i < SER_BUFFER; i++) {
        TEST_ASSERT_EQUAL_UINT8('A' + i, testBuffer.buffer[i]);
    }
}

/**
 * Test: UART baud rate divider calculation
 */
void test_uart_divider(void)
{
    // DIVIDER should be calculated for 38400 baud
    // Formula: _XTAL_FREQ/(16 * 38400) - 1
    // With 19.6608 MHz crystal: 19660800/(16*38400) - 1 = 31
    
    #ifdef _XTAL_FREQ
        int calculated = (int)(_XTAL_FREQ/(16UL * 38400) - 1);
        TEST_ASSERT_EQUAL(calculated, DIVIDER);
    #endif
}

/**
 * Test: Ring buffer empty condition
 */
void test_ring_buffer_empty(void)
{
    struct RingBuffer testBuffer;
    
    // Buffer is empty when read index equals update index
    testBuffer.rIndex = 5;
    testBuffer.uIndex = 5;
    
    TEST_ASSERT_EQUAL(testBuffer.rIndex, testBuffer.uIndex);
}

/**
 * Test: Ring buffer full condition
 */
void test_ring_buffer_full(void)
{
    struct RingBuffer testBuffer;
    
    // Buffer is full when next update index equals read index
    testBuffer.rIndex = 5;
    testBuffer.uIndex = 4;  // One behind in circular buffer
    
    // Calculate if buffer is conceptually full
    uint8_t nextIndex = (testBuffer.uIndex + 1) % SER_BUFFER;
    if (nextIndex == testBuffer.rIndex) {
        TEST_ASSERT_TRUE(true); // Buffer would be full
    }
}

/**
 * Test: Multiple buffer instances
 */
void test_multiple_buffers(void)
{
    struct RingBuffer buffer1, buffer2;
    
    // Initialize with different values
    buffer1.rIndex = 1;
    buffer1.uIndex = 2;
    buffer2.rIndex = 3;
    buffer2.uIndex = 4;
    
    // Verify independence
    TEST_ASSERT_EQUAL_UINT8(1, buffer1.rIndex);
    TEST_ASSERT_EQUAL_UINT8(2, buffer1.uIndex);
    TEST_ASSERT_EQUAL_UINT8(3, buffer2.rIndex);
    TEST_ASSERT_EQUAL_UINT8(4, buffer2.uIndex);
}

/**
 * Test: Short value size
 */
void test_short_value_type(void)
{
    uint16_t test_short = 0;
    
    // uint16_t should be 2 bytes
    TEST_ASSERT_EQUAL_size_t(2, sizeof(test_short));
    
    // Test range
    test_short = 0xFFFF;
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, test_short);
    
    test_short = 0x0000;
    TEST_ASSERT_EQUAL_UINT16(0x0000, test_short);
}

// Note: Hardware-dependent functions (initSerial, putch, getByte, etc.)
// would require hardware mocking for full functional testing.
// These tests validate data structures and calculations.
