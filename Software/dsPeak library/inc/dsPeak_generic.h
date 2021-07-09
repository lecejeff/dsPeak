//***************************************************************************//
// File      : general.h
//
// Includes  : xc.h
//
// Purpose   : General header file
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
//****************************************************************************//

#ifndef __GENERAL_H_
#define	__GENERAL_H_

#define FOSC 140000000UL
#define FCY (FOSC/2)    // FCY of 70MIPS

#include <stdint.h>
#include <xc.h>
#include <libpic30.h>
#include <dsp.h>
#include <string.h>

// dsPeak generic I/Os definition
// Basically LEDs, Pushbutton, Encoder switch, buzzer
#define DSPEAK_IO_BTN1_STATE        PORTAbits.RA9
#define DSPEAK_IO_BTN1_DIR          TRISAbits.TRISA9
#define DSPEAK_IO_BTN2_STATE        PORTFbits.RF2
#define DSPEAK_IO_BTN2_DIR          TRISFbits.TRISF2

#define RGB_LED_RED     LATBbits.LATB3
#define RGB_LED_GREEN   LATBbits.LATB4
#define RGB_LED_BLUE    LATBbits.LATB5

void DSPIC_init (void);
void hex_to_ascii (uint8_t ucByte, uint8_t *ucByteH, uint8_t *ucByteL);
int bcd_to_decimal(uint8_t x) ;
uint16_t dec2bcd(uint16_t num);
void RGB_LED_set_color (uint32_t color);
uint8_t hex_to_dec (uint8_t hex);

#endif	/* GENERAL_H */

