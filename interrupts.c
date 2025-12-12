#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "interrupts.h"
#include "serial.h"
#include "peripherials.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
static volatile ms_t millisValue=0;

void __interrupt () isr(void)
{
    if(TMR1IF && TMR1IE){
        TMR1H = TMR1_H_PRES;             // preset for timer1 MSB register
        TMR1L = TMR1_L_PRES;             // preset for timer1 LSB register        
        TMR1IF = 0;
        ++millisValue;
    }else if(RCIF){
        // Check for UART errors before reading data
        if(RCSTAbits.FERR){
            // Framing error - read and discard byte to clear error
            uint8_t dummy = RCREG;
            (void)dummy; // Suppress unused warning
            uartErrors.framingErrors++;
            // Clear error by toggling CREN
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }else if(RCSTAbits.OERR){
            // Overrun error - clear by resetting CREN
            uartErrors.overrunErrors++;
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
            // Read to clear FIFO
            uint8_t dummy = RCREG;
            (void)dummy;
        }else{
            // No errors - read data into buffer
            uint8_t nextIndex = rxBuffer.rIndex + 1;
            if(nextIndex == SER_BUFFER){
                nextIndex = 0;
            }
            
            // Check if buffer would overflow
            if(nextIndex == rxBuffer.uIndex){
                // Buffer full - discard oldest data or increment error
                uartErrors.bufferOverflows++;
                // Read byte anyway to prevent UART overrun
                uint8_t dummy = RCREG;
                (void)dummy;
            }else{
                // Buffer has space - store the byte
                rxBuffer.buffer[rxBuffer.rIndex] = RCREG;
                rxBuffer.rIndex = nextIndex;
            }
        }
        RCIF = 0;
    }
}

ms_t millis(void)
{
    return millisValue;
}
