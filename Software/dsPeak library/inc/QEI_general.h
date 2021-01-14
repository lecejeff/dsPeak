//****************************************************************************//
// File      :  QEI_general.h
//
// Functions :  None
//
// Includes  :  general.h
//
// Purpose   :  QEI general definitions used by the QEI driver
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
#ifndef __QEIGENERAL_H__
#define __QEIGENERAL_H__

#include "general.h"

typedef struct
{
    unsigned int refresh_freq;
    unsigned long pulse_for_tour;
    unsigned long pulse_cnter_dist;
    unsigned long pulse_cnter;
    unsigned long pulse_getter;
    unsigned long tour_cnter_dist;
    unsigned long tour_cnter;
    unsigned long tour_getter;
    unsigned char int_event;
    unsigned char direction;
    unsigned int motor_cpr;
    unsigned int motor_gear_derate;
    unsigned int speed_rpm;
    unsigned long prev_pulse;
    unsigned long new_pulse;
    unsigned long pulse_diff;
    unsigned long pulse_per_tour;
}STRUCT_QEI;

#define QEI_QTY 1

#define QEI_1 0

#define QEI1B_PIN PORTAbits.RA3
#define QEI1A_PIN PORTAbits.RA2

#define QEI_DIR_BACKWARD 1
#define QEI_DIR_FORWARD 0

// Defines for 47:1 gearbox, 100RPM 12V motor
// Default values
#define COUNT_PER_REVOLUTION 48     // Encoder resolution
#define GEAR_DERATE 47              // Gear derate ratio
#define MOTOR_MAX_RPM 110            //   
#define PULSE_PER_TOUR (COUNT_PER_REVOLUTION * GEAR_DERATE)

#define MAX_PULSE_CNT 100000000
#define MAX_TOUR_CNT 100000000

#endif