# Serial Communication Migration Guide

## Overview

This guide helps you migrate from the previous serial communication implementation (38400 baud) to the new, more reliable implementation (9600 baud).

## What Changed?

### Baud Rate (BREAKING CHANGE)

**Old Configuration:**
- Baud rate: 38400 bps
- Actual rate: 39516 bps (2.91% error)
- Status: Unreliable due to crystal skew

**New Configuration:**
- Baud rate: 9600 bps
- Actual rate: 9646 bps (0.48% error)
- Status: Highly reliable, well within UART tolerance

### Why the Change?

The PIC16F886's 19.6 MHz crystal doesn't divide evenly for 38400 baud, resulting in:
- Actual baud rate of 39516 bps (2.91% error)
- Frequent communication errors
- Unreliable data transmission
- Reports indicated fluctuations as high as 39750 bps

The new 9600 baud rate:
- Only 0.48% error (9646 bps actual)
- Well within UART specifications (<2% error)
- Significantly more reliable
- Better tolerance for crystal variations

### Enhanced Error Handling

**New Features:**
- Automatic detection and recovery from framing errors (FERR)
- Automatic detection and recovery from overrun errors (OERR)
- Ring buffer overflow protection
- Error statistics tracking for diagnostics

**Error Statistics:**
```c
extern volatile struct UartErrors uartErrors;

// Access error counters:
uint8_t framingErrors = uartErrors.framingErrors;
uint8_t overrunErrors = uartErrors.overrunErrors;
uint8_t bufferOverflows = uartErrors.bufferOverflows;
```

### Startup Banner

On initialization, the firmware now prints a banner:
```
========================================
PetSafe Cat Flap - Alternative Firmware
Serial Interface Ready
Baud Rate: 9600 bps
========================================
```

This confirms:
- Serial port is initialized
- Firmware is running
- Correct baud rate is configured

## Migration Steps

### 1. Update Serial Terminal Configuration

**If using a serial terminal (PuTTY, screen, minicom, etc.):**

Old configuration:
```bash
screen /dev/ttyUSB0 38400
```

New configuration:
```bash
screen /dev/ttyUSB0 9600
```

**PuTTY users:**
1. Open Session settings
2. Change "Speed" from 38400 to 9600
3. Save the session

**Arduino Serial Monitor:**
1. Change baud rate dropdown from 38400 to 9600

### 2. Update Software Applications

**Python example:**
```python
# Old code:
ser = serial.Serial('/dev/ttyUSB0', 38400, timeout=1)

# New code:
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
```

**Node.js example:**
```javascript
// Old code:
const port = new SerialPort('/dev/ttyUSB0', { baudRate: 38400 });

// New code:
const port = new SerialPort('/dev/ttyUSB0', { baudRate: 9600 });
```

**C/C++ example:**
```c
// Old code:
cfsetispeed(&tty, B38400);
cfsetospeed(&tty, B38400);

// New code:
cfsetispeed(&tty, B9600);
cfsetospeed(&tty, B9600);
```

### 3. Update Documentation and Scripts

Update any:
- Configuration files
- Shell scripts
- Documentation
- Connection instructions
- Monitoring tools

Search for "38400" and replace with "9600".

## Protocol Compatibility

### Commands (UNCHANGED)

All serial commands remain the same:

- `S` - Request status
- `Cxx` - Read/write configuration
- `Mx` - Set operating mode

### Response Format (UNCHANGED)

Response formats are unchanged:

- Status: `AM[mode]L[light]P[pos]S[bits]\n`
- Config: `AC[index]V[value]\n`
- Mode: `AM[mode]\n`
- Cat detected: `E[6-byte-ID]\n`

### Timing Considerations

**Improved Reliability:**
- Less likely to encounter communication errors
- More tolerance for cable length and noise
- Better stability with varying crystal temperatures

**Slightly Slower:**
- Data transfer is ~4x slower than before
- Status updates: ~15ms instead of ~4ms
- For typical usage (occasional commands), this is negligible
- For continuous streaming, consider if 9600 baud meets your needs

## Testing Your Setup

### 1. Connect and Verify Banner

After connecting, you should see:
```
========================================
PetSafe Cat Flap - Alternative Firmware
Serial Interface Ready
Baud Rate: 9600 bps
========================================
```

If you don't see this, check:
- Baud rate is set to 9600
- TX/RX wires are not crossed
- Serial cable is working
- Firmware has been updated

### 2. Test Status Command

Send: `S`

Expected response: `AM[mode]L[light]P[pos]S[bits]\n`

Example: `AM0L512P512S3\n`

### 3. Verify Error Handling

The new implementation handles errors automatically:
- Framing errors are detected and cleared
- Overrun errors trigger automatic recovery
- Buffer overflows are tracked (check `uartErrors.bufferOverflows`)

## Troubleshooting

### Serial Terminal Shows Garbage

**Possible causes:**
- Baud rate mismatch (most common)
- Wrong serial port
- Hardware connection issue

**Solution:**
1. Verify baud rate is set to 9600
2. Try disconnecting and reconnecting
3. Check for startup banner
4. Verify TX/RX connections

### No Response to Commands

**Possible causes:**
- Not waiting for startup banner
- Commands sent during initialization
- Cable issue

**Solution:**
1. Wait 100ms after connection for initialization
2. Wait to see startup banner before sending commands
3. Send `S` command and wait for response
4. Check cable connections

### Intermittent Communication

**Possible causes:**
- Noisy environment
- Long cable runs
- Poor ground connection

**Solution:**
1. Use shielded cable
2. Keep cable length <3 meters for best results
3. Ensure proper ground connection
4. Check error counters to identify issue type

### How to Check Error Statistics

Error counters are exposed in the `uartErrors` global structure:

```c
// In your monitoring code:
printf("Framing errors: %d\n", uartErrors.framingErrors);
printf("Overrun errors: %d\n", uartErrors.overrunErrors);
printf("Buffer overflows: %d\n", uartErrors.bufferOverflows);
```

If errors are increasing:
- Framing errors: Check baud rate, cable quality
- Overrun errors: Application not reading data fast enough
- Buffer overflows: Too much data arriving, increase read frequency

## Benefits of the New Implementation

### 1. Reliability
- 6x reduction in baud rate error (2.91% → 0.48%)
- Automatic error recovery
- More stable with temperature variations

### 2. Robustness
- Handles framing errors gracefully
- Recovers from overrun conditions
- Protects against buffer overflow

### 3. Diagnostics
- Error statistics for monitoring
- Startup banner confirms operation
- Better debugging capabilities

### 4. Future-Proof
- Room for additional error handling
- Extensible error tracking
- Foundation for enhanced features

## Rollback Procedure (If Needed)

If you need to rollback to 38400 baud:

1. Edit `serial.h`:
   ```c
   #define BAUD_RATE 38400
   ```

2. Rebuild firmware
3. Reflash device
4. Update your terminal/software to 38400

**Note:** This is NOT recommended due to the higher error rate.

## Support

If you encounter issues:

1. Verify baud rate is set to 9600
2. Check the startup banner appears
3. Test with `S` command
4. Review error statistics
5. Check hardware connections
6. Open an issue on GitHub with:
   - Your serial configuration
   - Terminal output (including banner)
   - Error messages
   - Hardware revision

## Summary

**Action Required:** Update all serial terminals and software to 9600 baud

**Benefits:**
- 6x better accuracy (0.48% vs 2.91% error)
- Automatic error handling
- Better reliability
- Diagnostic capabilities

**No Changes Required:**
- Command protocol (same commands)
- Response format (same responses)
- Hardware connections (same pins)

The migration is straightforward - just update your baud rate setting to 9600!
