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

// Interrupt registers
extern uint8_t INTCON;
extern uint8_t PIE1;
extern uint8_t PIE2;
extern uint8_t PIR1;
extern uint8_t PIR2;

// Additional registers
extern uint8_t CCP2CON;
extern uint8_t WPUB;

// Bit-addressable structures for testing
typedef struct {
    unsigned nRBPU : 1;
    unsigned INTEDG : 1;
    unsigned T0CS : 1;
    unsigned T0SE : 1;
    unsigned PSA : 1;
    unsigned PS0 : 1;
    unsigned PS1 : 1;
    unsigned PS2 : 1;
} OPTION_REG_bits_t;
extern OPTION_REG_bits_t OPTION_REGbits;

typedef struct {
    unsigned TMR1ON : 1;
    unsigned TMR1CS : 1;
    unsigned T1SYNC : 1;
    unsigned T1OSCEN : 1;
    unsigned T1CKPS0 : 1;
    unsigned T1CKPS1 : 1;
    unsigned TMR1GE : 1;
    unsigned T1GINV : 1;
} T1CON_bits_t;
extern T1CON_bits_t T1CONbits;

typedef struct {
    unsigned TMR1IF : 1;
    unsigned TMR2IF : 1;
    unsigned CCP1IF : 1;
    unsigned SSPIF : 1;
    unsigned TXIF : 1;
    unsigned RCIF : 1;
    unsigned ADIF : 1;
    unsigned TMR1GIF : 1;
} PIR1_bits_t;
extern PIR1_bits_t PIR1bits;

typedef struct {
    unsigned TMR1IE : 1;
    unsigned TMR2IE : 1;
    unsigned CCP1IE : 1;
    unsigned SSPIE : 1;
    unsigned TXIE : 1;
    unsigned RCIE : 1;
    unsigned ADIE : 1;
    unsigned TMR1GIE : 1;
} PIE1_bits_t;
extern PIE1_bits_t PIE1bits;

typedef struct {
    unsigned RBIF : 1;
    unsigned INTF : 1;
    unsigned T0IF : 1;
    unsigned RBIE : 1;
    unsigned INTE : 1;
    unsigned T0IE : 1;
    unsigned PEIE : 1;
    unsigned GIE : 1;
} INTCON_bits_t;
extern INTCON_bits_t INTCONbits;

typedef struct {
    unsigned ADON : 1;
    unsigned GO_DONE : 1;
    unsigned CHS0 : 1;
    unsigned CHS1 : 1;
    unsigned CHS2 : 1;
    unsigned CHS3 : 1;
    unsigned VCFG0 : 1;
    unsigned ADFM : 1;
} ADCON0_bits_t;
extern ADCON0_bits_t ADCON0bits;

typedef struct {
    unsigned CREN : 1;
    unsigned SREN : 1;
    unsigned FERR : 1;
    unsigned OERR : 1;
    unsigned : 1;
    unsigned : 1;
    unsigned RC9 : 1;
    unsigned SPEN : 1;
} RCSTA_bits_t;
extern RCSTA_bits_t RCSTAbits;

typedef struct {
    unsigned TMR2ON : 1;
    unsigned T2CKPS0 : 1;
    unsigned T2CKPS1 : 1;
    unsigned TOUTPS0 : 1;
    unsigned TOUTPS1 : 1;
    unsigned TOUTPS2 : 1;
    unsigned TOUTPS3 : 1;
    unsigned : 1;
} T2CON_bits_t;
extern T2CON_bits_t T2CONbits;

typedef struct {
    unsigned TRISC0 : 1;
    unsigned TRISC1 : 1;
    unsigned TRISC2 : 1;
    unsigned TRISC3 : 1;
    unsigned TRISC4 : 1;
    unsigned TRISC5 : 1;
    unsigned TRISC6 : 1;
    unsigned TRISC7 : 1;
} TRISC_bits_t;
extern TRISC_bits_t TRISCbits;

typedef struct {
    unsigned RA0_bit : 1;
    unsigned RA1 : 1;
    unsigned RA2_bit : 1;
    unsigned RA3 : 1;
    unsigned RA4 : 1;
    unsigned RA5 : 1;
    unsigned RA6 : 1;
    unsigned RA7 : 1;
} PORTA_bits_t;
extern PORTA_bits_t PORTAbits;

typedef struct {
    unsigned RB0_bit : 1;
    unsigned RB1 : 1;
    unsigned RB2 : 1;
    unsigned RB3 : 1;
    unsigned RB4 : 1;
    unsigned RB5 : 1;
    unsigned RB6 : 1;
    unsigned RB7 : 1;
} PORTB_bits_t;
extern PORTB_bits_t PORTBbits;

typedef struct {
    unsigned RC0 : 1;
    unsigned RC1 : 1;
    unsigned RC2 : 1;
    unsigned RC3 : 1;
    unsigned RC4 : 1;
    unsigned RC5 : 1;
    unsigned RC6 : 1;
    unsigned RC7 : 1;
} PORTC_bits_t;
extern PORTC_bits_t PORTCbits;

// Mock ADC result for testing
extern uint16_t mockADCResult;

// Mock millisecond counter
extern uint32_t mockMillis;

#endif /* XC_HARDWARE_MOCK_H */
