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
#ifndef __dspeak_generic_h_
#define	__dspeak_generic_h_

// If using primary oscillator (20MHz) with PLL, uncomment #define POSC_20MHz
#define POSC_20MHz_70MIPS
#ifdef POSC_20MHz_70MIPS
    #define FOSC 140000000UL
    #define FCY 70000000UL    // FCY of 70MIPS
    void dsPeak_posc_20MHz_init (void);
    void dsPeak_auxosc_48MHz_init (void);
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

// Microchip libraries
#include <xc.h>
#include <stdint.h>
#include <libpic30.h>
#include <dsp.h>
#include <string.h>
#include "DMA.h"
// End of Microchip libraries

typedef struct
{
    uint8_t state;
    uint8_t prev_state;
    uint16_t debounce_cnt;
    uint16_t debounce_thres;
    uint8_t debounce_state;
    uint8_t do_once;
    uint8_t channel;
}STRUCT_BUTTON;

typedef struct
{
    uint8_t state;
    uint8_t prev_state;
    uint8_t channel;
}STRUCT_LED;

// dsPeak generic I/Os definition
// Basically LEDs, Pushbutton, Encoder switch, buzzer, RGB LED
// ---------------------- dsPeak on-board pushbuttons ------------------------//
#define DSPEAK_BTN1_RD          PORTAbits.RA9
#define DSPEAK_BTN1_DIR         TRISAbits.TRISA9
#define DSPEAK_BTN2_RD          PORTFbits.RF2
#define DSPEAK_BTN2_DIR         TRISFbits.TRISF2
#define DSPEAK_BTN3_RD          PORTFbits.RF8
#define DSPEAK_BTN3_DIR         TRISFbits.TRISF8
#define DSPEAK_BTN4_RD          PORTAbits.RA4
#define DSPEAK_BTN4_DIR         TRISAbits.TRISA4
#define BTN_1                   0
#define BTN_2                   1
#define BTN_3                   2
#define BTN_4                   3
#define OUTPUT                  0
#define INPUT                   1
#define BTN_QTY                 4

// ---------------------- dsPeak on-board LEDs -------------------------------//
#define DSPEAK_LED1_WR          LATHbits.LATH8
#define DSPEAK_LED1_DIR         TRISHbits.TRISH8
#define DSPEAK_LED2_WR          LATHbits.LATH9
#define DSPEAK_LED2_DIR         TRISHbits.TRISH9
#define DSPEAK_LED3_WR          LATHbits.LATH10
#define DSPEAK_LED3_DIR         TRISHbits.TRISH10
#define DSPEAK_LED4_WR          LATHbits.LATH11
#define DSPEAK_LED4_DIR         TRISHbits.TRISH11
#define LED_1                   0
#define LED_2                   1
#define LED_3                   2
#define LED_4                   3
#define HIGH                    1
#define LOW                     0  
#define LED_QTY                 4

// --------------------- dsPeak encoder pushbutton ---------------------------//
#define DSPEAK_ENC_SW_STATE     PORTAbits.RA5
#define DSPEAK_ENC_SW_DIR       TRISAbits.TRISA5

// ---------------------------- dsPeak buzzer --------------------------------//
#define DSPEAK_BUZZ_WR          LATDbits.LATD8
#define DSPEAK_BUZZ_DIR         TRISDbits.TRISD8

// ---------------------- dsPeak on-board RGB LED ----------------------------//
#define DSPEAK_RGB_RED_WR       LATBbits.LATB3    
#define DSPEAK_RGB_RED_DIR      TRISBbits.TRISB3  
#define DSPEAK_RGB_GREEN_WE     LATBbits.LATB4    
#define DSPEAK_RGB_GREEN_DIR    TRISBbits.TRISB4  
#define DSPEAK_RGB_BLUE_WE      LATBbits.LATB5    
#define DSPEAK_RGB_BLUE_DIR     TRISBbits.TRISB5 

// ------------------------- dsPeak MikroBUS #1 ------------------------------//
// AIN
#define DSPEAK_MKB1_AIN1_DIR    TRISEbits.TRISE8
#define DSPEAK_MKB1_AIN1_ANS    ANSELEbits.ANSE8
#define DSPEAK_MKB1_AIN1_WR     LATEbits.LATE8
#define DSPEAK_MKB1_AIN1_RD     PORTEbits.RE8
// RST
#define DSPEAK_MKB1_RST_DIR     TRISHbits.TRISH14
#define DSPEAK_MKB1_RST_WR      LATHbits.LATH14
#define DSPEAK_MKB1_RST_RD      PORTHbits.RH14
// SPI CSn
#define DSPEAK_MKB1_CSn_DIR     TRISHbits.TRISH15
#define DSPEAK_MKB1_CSn_WR      LATHbits.LATH15
#define DSPEAK_MKB1_CSn_RD      PORTHbits.RH15
// INT
#define DSPEAK_MKB1_INT_DIR     TRISDbits.TRISD0
#define DSPEAK_MKB1_INT_WR      LATDbits.LATD0
#define DSPEAK_MKB1_INT_RD      PORTDbits.RD0
// PWM
#define DSPEAK_MKB1_PWM_DIR     TRISEbits.TRISE7
#define DSPEAK_MKB1_PWM_WR      LATEbits.LATE7
#define DSPEAK_MKB1_PWM_RD      PORTEbits.RE7

// ------------------------- dsPeak MikroBUS #2 ------------------------------//
// AIN
#define DSPEAK_MKB2_AIN1_DIR    TRISEbits.TRISE9
#define DSPEAK_MKB2_AIN1_ANS    ANSELEbits.ANSE9
#define DSPEAK_MKB2_AIN1_WR     LATEbits.LATE9
#define DSPEAK_MKB2_AIN1_RD     PORTEbits.RE9
// RST
#define DSPEAK_MKB2_RST_DIR     TRISHbits.TRISH12
#define DSPEAK_MKB2_RST_WR      LATHbits.LATH12
#define DSPEAK_MKB2_RST_RD      PORTHbits.RH12
// SPI CSn
#define DSPEAK_MKB2_CSn_DIR     TRISHbits.TRISH13
#define DSPEAK_MKB2_CSn_WR      LATHbits.LATH13
#define DSPEAK_MKB2_CSn_RD      PORTHbits.RH13
// INT
#define DSPEAK_MKB2_INT_DIR     TRISDbits.TRISD11
#define DSPEAK_MKB2_INT_WR      LATDbits.LATD11
#define DSPEAK_MKB2_INT_RD      PORTDbits.RD11
// PWM
#define DSPEAK_MKB2_PWM_DIR     TRISCbits.TRISC1
#define DSPEAK_MKB2_PWM_WR      LATCbits.LATC1
#define DSPEAK_MKB2_PWM_RD      PORTCbits.RC1

// ----------------------- dsPeak MikroBUS shared ----------------------------//
// SPI SCLK
#define DSPEAK_MKB_SCLK_DIR     TRISDbits.TRISD7
#define DSPEAK_MKB_SCLK_WR      LATDbits.LATD7
#define DSPEAK_MKB_SCLK_RD      PORTDbits.RD7
// SPI MISO
#define DSPEAK_MKB_MISO_DIR     TRISDbits.TRISD13
#define DSPEAK_MKB_MISO_WR      LATDbits.LATD13
#define DSPEAK_MKB_MISO_RD      PORTDbits.RD13
// SPI MOSI
#define DSPEAK_MKB_MOSI_DIR     TRISFbits.TRISF0
#define DSPEAK_MKB_MOSI_WR      LATFbits.LATF0
#define DSPEAK_MKB_MOSI_RD      PORTFbits.RF0
// SDA
#define DSPEAK_MKB_SDA_DIR      TRISDbits.TRISD9
#define DSPEAK_MKB_SDA_WR       LATDbits.LATD9
#define DSPEAK_MKB_SDA_RD       PORTDbits.RD9
// SCL
#define DSPEAK_MKB_SCL_DIR      TRISDbits.TRISD10
#define DSPEAK_MKB_SCL_WR       LATDbits.LATD10
#define DSPEAK_MKB_SCL_RD       PORTDbits.RD10
// UART TX
#define DSPEAK_MKB_UART_TX_DIR  TRISFbits.TRISF1
#define DSPEAK_MKB_UART_TX_WR   LATFbits.LATF1
#define DSPEAK_MKB_UART_TX_RD   PORTFbits.RF1
// UART RX
#define DSPEAK_MKB_UART_RX_DIR  TRISGbits.TRISG1
#define DSPEAK_MKB_UART_RX_WR   LATGbits.LATG1
#define DSPEAK_MKB_UART_RX_RD   PORTGbits.RG1

// ---------------------------- Bringup defines ----------------------------- //
// Before release of the documentation, these defines should be removes
#define BRINGUP_DSPEAK_1
//#define BRINGUP_DSPEAK_2

// To use USB_UART debug port on dsPeak, uncomment the following line
#define UART_DEBUG_ENABLE  
    
// MikroBus various ClickBOard bringup defines
// To use RS-485 CLICK 6 on UART_2
// #define RS485_CLICK_UART2
    
// To use the FTDI EVE port, uncomment the following line
//#define EVE_SCREEN_ENABLE
    
   
void dsPeak_init(void);
void hex_to_ascii(uint8_t in, uint8_t *out_h, uint8_t *out_l);
int16_t bcd_to_decimal(uint8_t bcd);
uint16_t dec_to_bcd(uint16_t dec);
void RGB_LED_set_color (uint32_t color);
uint8_t hex_to_dec (uint8_t hex);
uint8_t dsPeak_button_init (STRUCT_BUTTON *btn, uint8_t channel, uint16_t debounce_thres);
uint8_t dsPeak_button_debounce (STRUCT_BUTTON *btn);
uint8_t dsPeak_button_get_state (STRUCT_BUTTON *btn);
uint8_t dsPeak_led_init (STRUCT_LED *led, uint8_t channel, uint8_t value);
uint8_t dsPeak_led_write (STRUCT_LED *led, uint8_t value);

void uint16_to_byte8 (uint16_t *ptr_16, uint8_t *ptr_8, uint16_t length);
void byte8_to_uint16 (uint8_t *ptr_8, uint16_t *ptr_16, uint16_t length);

#endif	/* __dspeak_generic_H_ */

