//***************************************************************************//
// File      : general.h
//
// Includes  : xc.h
//
// Purpose   : General header file
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2020
//****************************************************************************//

#ifndef __GENERAL_H_
#define	__GENERAL_H_

#define FOSC 140000000
#define FCY (FOSC/2)    // FCY of 70MIPS

#include <xc.h>
#include <libpic30.h>
#include <dsp.h>
#include <string.h>

#define RGB_LED_RED     LATBbits.LATB3
#define RGB_LED_GREEN   LATBbits.LATB4
#define RGB_LED_BLUE    LATBbits.LATB5

void DSPIC_init (void);
void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL);
int bcd_to_decimal(unsigned char x) ;
unsigned int dec2bcd(unsigned int num);
void RGB_LED_set_color (unsigned long color);
unsigned char hex_to_dec (unsigned char hex);

#endif	/* GENERAL_H */

