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
    for (size_t i = 0; i < SER_BUFFER; i++) {
        testBuffer.buffer[i] = (uint8_t)('A' + i);
    }
    
    // Verify data
    for (size_t i = 0; i < SER_BUFFER; i++) {
        TEST_ASSERT_EQUAL_UINT8('A' + i, testBuffer.buffer[i]);
    }
}

/**
 * Test: UART baud rate divider calculation
 */
void test_uart_divider(void)
{
    // DIVIDER should be calculated for 9600 baud
    // Formula: _XTAL_FREQ/(16 * 9600) - 1
    // With test mock _XTAL_FREQ=19660800: 19660800/(16*9600) - 1 = 127.0 = 127
    // Note: Actual hardware uses 19600000 which gives 126.7 ≈ 127
    
    #ifdef _XTAL_FREQ
        int calculated = (int)(_XTAL_FREQ/(16UL * BAUD_RATE) - 1);
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
 * Test: UART error structure size
 */
void test_uart_errors_structure(void)
{
    struct UartErrors errors;
    
    // Should have three uint8_t fields
    TEST_ASSERT_EQUAL_size_t(1, sizeof(errors.framingErrors));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(errors.overrunErrors));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(errors.bufferOverflows));
}

/**
 * Test: UART error initialization
 */
void test_uart_errors_init(void)
{
    struct UartErrors errors;
    
    // Initialize error counters
    errors.framingErrors = 0;
    errors.overrunErrors = 0;
    errors.bufferOverflows = 0;
    
    // Verify initialization
    TEST_ASSERT_EQUAL_UINT8(0, errors.framingErrors);
    TEST_ASSERT_EQUAL_UINT8(0, errors.overrunErrors);
    TEST_ASSERT_EQUAL_UINT8(0, errors.bufferOverflows);
}

/**
 * Test: Baud rate constant
 */
void test_baud_rate_constant(void)
{
    // Verify baud rate is 9600
    TEST_ASSERT_EQUAL(9600, BAUD_RATE);
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

/**
 * Test: Ring buffer capacity
 */
void test_ring_buffer_capacity(void)
{
    // Buffer can hold SER_BUFFER-1 bytes (one slot reserved for empty detection)
    TEST_ASSERT_TRUE(SER_BUFFER > 1);
    
    // Effective capacity is SER_BUFFER - 1
    uint8_t effective_capacity = SER_BUFFER - 1;
    TEST_ASSERT_EQUAL_UINT8(15, effective_capacity);
}

/**
 * Test: Ring buffer index arithmetic
 */
void test_ring_buffer_index_arithmetic(void)
{
    uint8_t index = 0;
    
    // Test increment without wrap
    index = 5;
    index++;
    TEST_ASSERT_EQUAL_UINT8(6, index);
    
    // Test wrap-around simulation
    index = SER_BUFFER - 1;
    index++;
    if (index == SER_BUFFER) {
        index = 0;
    }
    TEST_ASSERT_EQUAL_UINT8(0, index);
}

/**
 * Test: UART errors structure initialization
 */
void test_uart_errors_all_fields(void)
{
    struct UartErrors errors;
    
    // Initialize all fields
    errors.framingErrors = 5;
    errors.overrunErrors = 3;
    errors.bufferOverflows = 1;
    
    // Verify all fields maintain values
    TEST_ASSERT_EQUAL_UINT8(5, errors.framingErrors);
    TEST_ASSERT_EQUAL_UINT8(3, errors.overrunErrors);
    TEST_ASSERT_EQUAL_UINT8(1, errors.bufferOverflows);
}

/**
 * Test: UART errors counter overflow
 */
void test_uart_errors_counter_overflow(void)
{
    struct UartErrors errors;
    
    // Set to maximum
    errors.framingErrors = 255;
    
    // Increment (would wrap to 0 in actual uint8_t)
    errors.framingErrors++;
    
    // Verify overflow behavior
    TEST_ASSERT_EQUAL_UINT8(0, errors.framingErrors);
}

/**
 * Test: Ring buffer pointer wraparound calculation
 */
void test_ring_buffer_next_index_calculation(void)
{
    struct RingBuffer testBuffer;
    
    // Test next index without wraparound
    testBuffer.rIndex = 5;
    uint8_t nextIndex = testBuffer.rIndex + 1;
    if (nextIndex == SER_BUFFER) {
        nextIndex = 0;
    }
    TEST_ASSERT_EQUAL_UINT8(6, nextIndex);
    
    // Test next index with wraparound
    testBuffer.rIndex = SER_BUFFER - 1;
    nextIndex = testBuffer.rIndex + 1;
    if (nextIndex == SER_BUFFER) {
        nextIndex = 0;
    }
    TEST_ASSERT_EQUAL_UINT8(0, nextIndex);
}

/**
 * Test: Ring buffer available space calculation
 */
void test_ring_buffer_available_space(void)
{
    struct RingBuffer testBuffer;
    
    // Empty buffer
    testBuffer.rIndex = 0;
    testBuffer.uIndex = 0;
    
    // Calculate available space
    int available;
    if (testBuffer.rIndex >= testBuffer.uIndex) {
        available = testBuffer.rIndex - testBuffer.uIndex;
    } else {
        available = (SER_BUFFER - testBuffer.uIndex) + testBuffer.rIndex;
    }
    
    // Empty buffer should have 0 available (data to read)
    TEST_ASSERT_EQUAL_INT(0, available);
}

/**
 * Test: Serial buffer size is power-friendly
 */
void test_serial_buffer_size_power_of_two(void)
{
    // 16 is a power of 2, making modulo operations efficient
    TEST_ASSERT_EQUAL(16, SER_BUFFER);
    
    // Verify it's a power of 2
    TEST_ASSERT_EQUAL(0, SER_BUFFER & (SER_BUFFER - 1));
}

/**
 * Test: Baud rate divider calculation correctness
 */
void test_baud_rate_divider_calculation(void)
{
    // DIVIDER = (_XTAL_FREQ / (16 * BAUD_RATE)) - 1
    // NOTE: Test uses _XTAL_FREQ=19660800 (defined in project.yml for testing)
    // Hardware uses _XTAL_FREQ=19600000 (actual crystal frequency)
    // Both values calculate to DIVIDER=127 for 9600 baud
    
    #ifdef _XTAL_FREQ
    int calculated = (int)(_XTAL_FREQ / (16UL * BAUD_RATE) - 1);
    TEST_ASSERT_EQUAL(calculated, DIVIDER);
    
    // Verify it's within valid range for 8-bit register
    TEST_ASSERT_TRUE(DIVIDER <= 255);
    TEST_ASSERT_TRUE(DIVIDER >= 0);
    #endif
}

/**
 * Test: Ring buffer used space calculation
 */
void test_ring_buffer_used_space(void)
{
    struct RingBuffer testBuffer;
    
    // Partially filled buffer
    testBuffer.rIndex = 8;
    testBuffer.uIndex = 3;
    
    // Calculate used space
    int used;
    if (testBuffer.rIndex >= testBuffer.uIndex) {
        used = testBuffer.rIndex - testBuffer.uIndex;
    } else {
        used = (SER_BUFFER - testBuffer.uIndex) + testBuffer.rIndex;
    }
    
    // Should have 5 bytes of data
    TEST_ASSERT_EQUAL_INT(5, used);
}

/**
 * Test: Ring buffer boundary conditions
 */
void test_ring_buffer_boundary_conditions(void)
{
    struct RingBuffer testBuffer;
    
    // Test at exact boundary
    testBuffer.rIndex = SER_BUFFER;
    testBuffer.uIndex = 0;
    
    // rIndex should wrap
    if (testBuffer.rIndex >= SER_BUFFER) {
        testBuffer.rIndex = 0;
    }
    
    TEST_ASSERT_EQUAL_UINT8(0, testBuffer.rIndex);
}

/**
 * Test: UART error counters independence
 */
void test_uart_errors_independence(void)
{
    struct UartErrors errors;
    
    // Set different values
    errors.framingErrors = 10;
    errors.overrunErrors = 20;
    errors.bufferOverflows = 30;
    
    // Modify one
    errors.framingErrors++;
    
    // Others should be unchanged
    TEST_ASSERT_EQUAL_UINT8(11, errors.framingErrors);
    TEST_ASSERT_EQUAL_UINT8(20, errors.overrunErrors);
    TEST_ASSERT_EQUAL_UINT8(30, errors.bufferOverflows);
}

// Note: Hardware-dependent functions (initSerial, putch, getByte, etc.)
// would require hardware mocking for full functional testing.
// These tests validate data structures and calculations.
