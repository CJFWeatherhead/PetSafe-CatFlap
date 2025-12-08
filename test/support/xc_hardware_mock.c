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
