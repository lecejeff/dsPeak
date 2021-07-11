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

#ifndef __dspeak_generic_H_
#define	__dspeak_generic_H_

// If using primary oscillator (20MHz) with PLL, uncomment #define POSC_20MHz
#define POSC_20MHz_70MIPS
#ifdef POSC_20MHz_70MIPS
    #define FOSC 140000000UL
    #define FCY (FOSC/2)    // FCY of 70MIPS
    void dsPeak_posc_20MHz_init (void);
#endif

// If using Fast RC oscillator (7.37MHz) with PLL, uncomment #define POSC_FRC
//#define POSC_FRC_66MIPS
#ifdef POSC_FRC_66MIPS
    #define FOSC 132660000L
    #define FCY (FOSC/2)    // FCY of 66.33MHz
    void dsPeak_posc_FRC_init (void);
#endif

//#define SOSC_32KHZ
#ifdef SOSC_32KHZ
    void dsPeak_sosc_32kHz_init (void);
#endif

#include <stdint.h>
#include <xc.h>
#include <libpic30.h>
#include <dsp.h>
#include <string.h>

// dsPeak generic I/Os definition
// Basically LEDs, Pushbutton, Encoder switch, buzzer, RGB LED
// ---------------------- dsPeak on-board pushbuttons ------------------------//
#define DSPEAK_BTN1_STATE       PORTAbits.RA9
#define DSPEAK_BTN1_DIR         TRISAbits.TRISA9
#define DSPEAK_BTN2_STATE       PORTFbits.RF2
#define DSPEAK_BTN2_DIR         TRISFbits.TRISF2
#define DSPEAK_BTN3_STATE       PORTFbits.RF8
#define DSPEAK_BTN3_DIR         TRISFbits.TRISF8
#define DSPEAK_BTN4_STATE       PORTAbits.RA4
#define DSPEAK_BTN4_DIR         TRISAbits.TRISA4

// ---------------------- dsPeak on-board LEDs -------------------------------//
#define DSPEAK_LED1_STATE       LATHbits.LATH8
#define DSPEAK_LED1_DIR         TRISHbits.TRISH8
#define DSPEAK_LED2_STATE       LATHbits.LATH9
#define DSPEAK_LED2_DIR         TRISHbits.TRISH9
#define DSPEAK_LED3_STATE       LATHbits.LATH10
#define DSPEAK_LED3_DIR         TRISHbits.TRISH10
#define DSPEAK_LED4_STATE       LATHbits.LATH11
#define DSPEAK_LED4_DIR         TRISHbits.TRISH11

// --------------------- dsPeak encoder pushbutton ---------------------------//
#define DSPEAK_ENC_SW_STATE     PORTAbits.RA5
#define DSPEAK_ENC_SW_DIR       TRISAbits.TRISA5

// ---------------------------- dsPeak buzzer --------------------------------//
#define DSPEAK_BUZZ_STATE       LATDbits.LATD8
#define DSPEAK_BUZZ_DIR         TRISDbits.TRISD8

// ---------------------- dsPeak on-board RGB LED ----------------------------//
#define DSPEAK_RGB_RED_STATE    LATBbits.LATB3    
#define DSPEAK_RGB_RED_DIR      TRISBbits.TRISB3  
#define DSPEAK_RGB_GREEN_STATE  LATBbits.LATB4    
#define DSPEAK_RGB_GREEN_DIR    TRISBbits.TRISB4  
#define DSPEAK_RGB_BLUE_STATE   LATBbits.LATB5    
#define DSPEAK_RGB_BLUE_DIR     TRISBbits.TRISB5 


void hex_to_ascii (uint8_t in, uint8_t *out_h, uint8_t *out_l);
int16_t bcd_to_decimal(uint8_t bcd);
uint16_t dec_to_bcd(uint16_t dec);
void RGB_LED_set_color (uint32_t color);
uint8_t hex_to_dec (uint8_t hex);

#endif	/* __dspeak_generic_H_ */

