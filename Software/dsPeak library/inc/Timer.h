//****************************************************************************//
// File      :  timer.h
//
// Functions :  void TIMER_init (uint8_t timer, uint8_t prescaler, uint32_t freq);
//              void TIMER_update_freq (uint8_t timer, uint8_t prescaler, uint32_t new_freq);
//              void TIMER_start (uint8_t timer);
//              void TIMER_stop (uint8_t timer);
//              uint8_t TIMER_get_state (uint8_t timer, uint8_t type);
//
// Includes  :  general.h
//
// Purpose   :  Driver for the dsPIC33EP 16-bit / 32-bit TIMER core
//              9 seperate 16-bit timers on dsPeak
//              TIMER_1 : 
//              TIMER_2 : 
//              TIMER_3 : 
//              TIMER_4 : 
//              TIMER_5 : 
//              TIMER_6 : 
//              TIMER_7 : 
//              TIMER_8 : 
//              TIMER_9 : 
//              
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#ifndef __TIMER_H_
#define	__TIMER_H_

#include "dspeak_generic.h"

#define TIMER_1             0
#define TIMER_2             1
#define TIMER_3             2
#define TIMER_4             3
#define TIMER_5             4
#define TIMER_6             5
#define TIMER_7             6
#define TIMER_8             7
#define TIMER_9             8

#define TIMER_QTY           9

#define TIMER_RUN_STATE     0
#define TIMER_INT_STATE     1

#define TIMER_PRESCALER_1   0
#define TIMER_PRESCALER_8   1
#define TIMER_PRESCALER_64  2
#define TIMER_PRESCALER_256 3

#define ALL_TIMER_PRESCALER 8

typedef struct
{
    uint8_t int_state;
    uint8_t run_state;
    uint32_t freq;
    uint8_t prescaler;
}TIMER_STRUCT;

void TIMER_init (uint8_t timer, uint8_t prescaler, uint32_t freq);
void TIMER_update_freq (uint8_t timer, uint8_t prescaler, uint32_t new_freq);
void TIMER_start (uint8_t timer);
void TIMER_stop (uint8_t timer);
uint8_t TIMER_get_state (uint8_t timer, uint8_t type);
#endif	/* TIMER_H */

