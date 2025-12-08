/**
 * Mock XC8 Header for Testing
 * This file provides stub definitions for PIC microcontroller registers
 * to allow testing on non-embedded platforms
 */

#ifndef XC_HARDWARE_MOCK_H
#define XC_HARDWARE_MOCK_H

// Prevent the real xc.h from being included
#ifndef xc_h
#define xc_h
#endif

#ifndef XC_H
#define XC_H
#endif

#include <stdint.h>
#include <stdbool.h>

// Define CPU frequency for testing
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 19660800UL
#endif

// Delay macros for testing (no-op)
#define __delay_ms(x) 
#define __delay_us(x)

// Interrupt control
#define ei()
#define di()

// NOP operation
#define NOP() 

// Mock register definitions
extern uint8_t PORTA;
extern uint8_t PORTB;
extern uint8_t PORTC;
extern uint8_t TRISA;
extern uint8_t TRISB;
extern uint8_t TRISC;
extern uint8_t ANSEL;
extern uint8_t ANSELH;

extern uint8_t T0IE;
extern uint8_t T0IF;
extern uint8_t TMR0;
extern uint8_t OPTION_REG;

extern uint8_t TXREG;
extern uint8_t RCREG;
extern uint8_t TXSTA;
extern uint8_t RCSTA;
extern uint8_t SPBRG;
extern uint8_t TXIF;
extern uint8_t RCIF;

extern uint8_t EEADR;
extern uint8_t EEDATA;
extern uint8_t EECON1;
extern uint8_t EECON2;
extern uint8_t RD;
extern uint8_t WR;
extern uint8_t WREN;

extern uint8_t GIE;
extern uint8_t PEIE;
extern uint8_t INTE;
extern uint8_t INTF;

// ADC registers
extern uint8_t ADRESH;
extern uint8_t ADRESL;
extern uint8_t ADCON0;
extern uint8_t ADCON1;
extern uint16_t ADRES;

// PWM registers
extern uint8_t CCP1CON;
extern uint8_t CCPR1L;

// Timer registers
extern uint8_t T1CON;
extern uint8_t TMR1H;
extern uint8_t TMR1L;
extern uint8_t T2CON;
extern uint8_t PR2;

// Bit definitions
#define RA0 0
#define RA1 1
#define RA2 2
#define RA3 3
#define RA4 4
#define RA5 5

#define RB0 0
#define RB1 1
#define RB2 2
#define RB3 3
#define RB4 4
#define RB5 5
#define RB6 6
#define RB7 7

#define RC0 0
#define RC1 1
#define RC2 2
#define RC3 3
#define RC4 4
#define RC5 5
#define RC6 6
#define RC7 7

#endif /* XC_MOCK_H */
