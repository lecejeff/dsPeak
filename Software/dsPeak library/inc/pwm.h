//****************************************************************************//
// File      : PWM.h
//
// Functions :  void PWM_init (void);
//              void PWM_change_duty (uint8_t channel, uint8_t duty);
//              uint8_t PWM_get_position (uint8_t channel);
//
// Includes  : general.h
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020   
//****************************************************************************//
#ifndef __PWM_H__
#define __PWM_H__

#include "general.h"

#define PWM_1L  0
#define PWM_1H  1
#define PWM_2L  2
#define PWM_2H  3
#define PWM_4H  4
#define PWM_5L  5
#define PWM_5H  6
#define PWM_6L  7
#define PWM_6H  8

#define PWM_QTY 9

#define PWM_CLOCK_PRESCALE 64L      // MUST FIT WITH DIVIDER IN PTCON2bits.PCLKDIV

#define PWM1H_PHASE 40000L
#define PWM1H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM1H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM1L_PHASE         40000L  // Phase in Hz
#define PWM1L_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM1L_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM2H_PHASE 40000L
#define PWM2H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM2H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM2L_PHASE 40000L
#define PWM2L_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM2L_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM4H_PHASE 50L
#define PWM4H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM4H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM5H_PHASE 50L
#define PWM5H_SERVO_BASE    0.03F   // Ratio in % of phase, base duty cycle
#define PWM5H_SERVO_END     0.12F    // Ratio in % of phase, max duty cycle

#define PWM5L_PHASE 50L
#define PWM5L_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM5L_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM6H_PHASE 50L
#define PWM6H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle
#define PWM6H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle

#define PWM6L_PHASE 50L
#define PWM6L_SERVO_BASE    0.046F   // Ratio in % of phase, base duty cycle
#define PWM6L_SERVO_END     0.105F    // Ratio in % of phase, max duty cycle

#define PWM_TYPE_SERVO  0
#define PWM_TYPE_LOAD   1

typedef struct
{
    uint16_t range;
    uint16_t base_value;
    uint16_t end_value;
    uint8_t value_p;  
    uint16_t new_duty;
    uint8_t pwm_type;
}STRUCT_PWM;

void PWM_init (uint8_t channel, uint8_t type);
void PWM_change_duty (uint8_t channel, uint8_t type, uint8_t duty);
uint8_t PWM_get_position (uint8_t channel);
#endif