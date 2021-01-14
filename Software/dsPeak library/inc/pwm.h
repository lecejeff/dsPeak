//****************************************************************************//
// File      : PWM.h
//
// Functions :  void PWM_init (void);
//              void PWM_change_duty (unsigned char channel, unsigned char duty);
//              unsigned char PWM_get_position (unsigned char channel);
//
// Includes  : general.h
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020   
//****************************************************************************//
#ifndef __PWM_H__
#define __PWM_H__

#include "general.h"

#define PWM_4L 0
#define PWM_4H 1
#define PWM_5H 2
#define PWM_6L 3
#define PWM_6H 4
#define PWM_QTY 5

#define PWM_CLOCK_PRESCALE 16L // MUST FIT WITH DIVIDER IN PCLKDIV
#define PWM4H_PHASE 40000L
#define PWM4L_PHASE 40000L
#define PWM5H_PHASE 50L
#define PWM6H_PHASE 50L
#define PWM6L_PHASE 50L

typedef struct
{
    unsigned int range;
    unsigned int base_value;
    unsigned int end_value;
    unsigned char value_p;  
    unsigned int new_duty;
}STRUCT_PWM;

void PWM_init (void);
void PWM_change_duty (unsigned char channel, unsigned char duty);
unsigned char PWM_get_position (unsigned char channel);
#endif