//****************************************************************************//
// File      : Timer.h
//
// Functions :  void TIMER_init (uint8_t timer, uint32_t freq);
//              void TIMER_update_freq (uint8_t timer, uint32_t new_freq);
//              void TIMER_start (uint8_t timer);
//              void TIMER_stop (uint8_t timer);
//              uint8_t TIMER_get_state (uint8_t timer);
//
// Includes  : general.h
//
//Jean-Francois Bilodeau    MPLab X v5.00    10/09/2018    
//****************************************************************************//
#ifndef __TIMER_H_
#define	__TIMER_H_

#include "general.h"

#define TIMER_1 0
#define TIMER_2 1
#define TIMER_3 2
#define TIMER_4 3
#define TIMER_5 4
#define TIMER_6 5
#define TIMER_7 6
#define TIMER_8 7
#define TIMER_9 8

#define TIMER_QTY 9

#define TIMER_RUN_STATE 0
#define TIMER_INT_STATE 1

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

