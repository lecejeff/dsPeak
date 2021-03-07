//****************************************************************************//
// File      :  QEI.c
//
// Functions :  void QEI_init (unsigned char channel);
//              unsigned long QEI_get_pulse (unsigned char channel);
//              void QEI_reset_pulse (unsigned char channel);
//              void QEI_set_gear_derate (unsigned char channel, unsigned int new_gear_derate);
//              unsigned int QEI_get_gear_derate (unsigned char channel);
//              void QEI_set_cpr (unsigned char channel, unsigned int new_cpr);
//              unsigned int QEI_get_cpr (unsigned char channel);
//              unsigned long QEI_get_tour (unsigned char channel);
//              void QEI_reset_tour (unsigned char channel);
//              unsigned char QEI_get_event (unsigned char channel);
//              unsigned char QEI_get_direction (unsigned char channel);
//              unsigned int QEI_get_velocity (unsigned char channel);
//              void QEI_calculate_velocity (unsigned char channel);
//              void QEI_interrupt_handle (unsigned char channel);
//
// Includes  :  QEI.h
//
// Purpose   :  QEI driver for the dsPIC33EP
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
#ifndef __QEI_H__
#define __QEI_H__

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
    unsigned int motor_max_rpm;
    unsigned int speed_rpm;
    unsigned int speed_rps;
    unsigned long prev_pulse;
    unsigned long velocity;
    unsigned long pulse_diff;
    unsigned long pulse_per_tour;
}STRUCT_QEI;

#define QEI_1       0
#define QEI_2       1
#define QEI_ROT_ENC 2

#define QEI_QTY 3

#define QEI1B_PIN   PORTAbits.RA7
#define QEI1A_PIN   PORTAbits.RA6

#define QEI2B_PIN   PORTGbits.RG13
#define QEI2A_PIN   PORTGbits.RG12

#define ROT_ENCB_PIN PORTAbits.RA15
#define ROT_ENCA_PIN PORTAbits.RA14

#define QEI_DIR_BACKWARD 1
#define QEI_DIR_FORWARD 0

// Defines for 99:1 gearbox, 100RPM 12V motor
// Default values
#define QEI_MOT1_CPR 48                             // Encoder count per revolution
#define QEI_MOT1_GDR 99                             // Encoder gear derate
#define QEI_MOT1_MAX_RPM 110                        // Max RPM   
#define QEI_MOT1_PPT (QEI_MOT1_CPR * QEI_MOT1_GDR)  // Max pulse per rotation

#define QEI_MOT2_CPR 48                             // Encoder count per revolution
#define QEI_MOT2_GDR 47                             // Encoder gear derate
#define QEI_MOT2_MAX_RPM 110                        // Max RPM   
#define QEI_MOT2_PPT (QEI_MOT1_CPR * QEI_MOT1_GDR)  // Max pulse per rotation

#define QEI_ROT_ENC_CPR 48                             // Encoder count per revolution
#define QEI_ROT_ENC_GDR 47                             // Encoder gear derate
#define QEI_ROT_ENC_MAX_RPM 110                        // Max RPM   
#define QEI_ROT_ENC_PPR (QEI_MOT1_CPR * QEI_MOT1_GDR)  // Max pulse per rotation

#define MAX_PULSE_CNT 100000000
#define MAX_TOUR_CNT 100000000

void QEI_init (unsigned char channel);
void QEI_set_fs (unsigned char channel, unsigned int refresh_freq);
unsigned int QEI_get_fs (unsigned char channel);
unsigned long QEI_get_pulse (unsigned char channel);
void QEI_reset_pulse (unsigned char channel);
void QEI_reset_distance (unsigned char channel);
unsigned long QEI_get_distance (unsigned char channel);
void QEI_set_gear_derate (unsigned char channel, unsigned int new_gear_derate);
unsigned int QEI_get_gear_derate (unsigned char channel);
void QEI_set_cpr (unsigned char channel, unsigned int new_cpr);
unsigned int QEI_get_cpr (unsigned char channel);
unsigned long QEI_get_tour (unsigned char channel);
void QEI_reset_tour (unsigned char channel);
unsigned char QEI_get_event (unsigned char channel);
unsigned char QEI_get_direction (unsigned char channel);
unsigned int QEI_get_speed_rpm (unsigned char channel);
unsigned int QEI_get_speed_rps (unsigned char channel);
void QEI_calculate_velocity (unsigned char channel);
unsigned int QEI_get_velocity (unsigned char channel);
unsigned int QEI_get_max_rpm (unsigned char channel);
void QEI_set_max_rpm (unsigned char channel, unsigned int max_rpm);
void QEI_interrupt_handle (unsigned char channel);

#endif