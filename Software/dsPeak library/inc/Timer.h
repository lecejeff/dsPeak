//****************************************************************************//
// File      : Timer.h
//
// Functions :  void TIMER_init (unsigned char timer, unsigned long freq);
//              void TIMER_update_freq (unsigned char timer, unsigned long new_freq);
//              void TIMER_start (unsigned char timer);
//              void TIMER_stop (unsigned char timer);
//              unsigned char TIMER_get_state (unsigned char timer);
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

#define ALL_TIMER_PRESCALER 8

typedef struct
{
    unsigned char int_state;
    unsigned char run_state;
}TIMER_STRUCT;

void TIMER_init (unsigned char timer, unsigned long freq);
void TIMER_update_freq (unsigned char timer, unsigned long new_freq);
void TIMER_start (unsigned char timer);
void TIMER_stop (unsigned char timer);
unsigned char TIMER_get_state (unsigned char timer, unsigned char type);
#endif	/* TIMER_H */

