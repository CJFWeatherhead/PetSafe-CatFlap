/**
 * Mock XC8 Register Implementation
 * Provides runtime variables for testing hardware interactions
 */

#include "xc_hardware_mock.h"
#include <stdint.h>

// Port registers
uint8_t PORTA = 0;
uint8_t PORTB = 0;
uint8_t PORTC = 0;
uint8_t TRISA = 0xFF;
uint8_t TRISB = 0xFF;
uint8_t TRISC = 0xFF;
uint8_t ANSEL = 0xFF;
uint8_t ANSELH = 0xFF;

// Timer0 registers
uint8_t T0IE = 0;
uint8_t T0IF = 0;
uint8_t TMR0 = 0;
uint8_t OPTION_REG = 0xFF;

// UART registers
uint8_t TXREG = 0;
uint8_t RCREG = 0;
uint8_t TXSTA = 0;
uint8_t RCSTA = 0;
uint8_t SPBRG = 0;
uint8_t TXIF = 0;
uint8_t RCIF = 0;

// EEPROM registers
uint8_t EEADR = 0;
uint8_t EEDATA = 0;
uint8_t EECON1 = 0;
uint8_t EECON2 = 0;
uint8_t RD = 0;
uint8_t WR = 0;
uint8_t WREN = 0;

// Interrupt registers
uint8_t GIE = 0;
uint8_t PEIE = 0;
uint8_t INTE = 0;
uint8_t INTF = 0;

// ADC registers
uint8_t ADRESH = 0;
uint8_t ADRESL = 0;
uint8_t ADCON0 = 0;
uint8_t ADCON1 = 0;
uint16_t ADRES = 0;

// PWM registers
uint8_t CCP1CON = 0;
uint8_t CCPR1L = 0;

// Timer registers
uint8_t T1CON = 0;
uint8_t TMR1H = 0;
uint8_t TMR1L = 0;
uint8_t T2CON = 0;
uint8_t PR2 = 0;

// Interrupt registers
uint8_t INTCON = 0;
uint8_t PIE1 = 0;
uint8_t PIE2 = 0;
uint8_t PIR1 = 0;
uint8_t PIR2 = 0;

// Additional registers
uint8_t CCP2CON = 0;
uint8_t WPUB = 0;

// Bit-addressable structures
OPTION_REG_bits_t OPTION_REGbits = {0};
T1CON_bits_t T1CONbits = {0};
PIR1_bits_t PIR1bits = {0};
PIE1_bits_t PIE1bits = {0};
INTCON_bits_t INTCONbits = {0};
ADCON0_bits_t ADCON0bits = {0};
RCSTA_bits_t RCSTAbits = {0};
T2CON_bits_t T2CONbits = {0};
TRISC_bits_t TRISCbits = {0};
PORTA_bits_t PORTAbits = {0};
PORTB_bits_t PORTBbits = {0};
PORTC_bits_t PORTCbits = {0};

// Mock ADC result
uint16_t mockADCResult = 0;

// Mock millisecond counter
uint32_t mockMillis = 0;
