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

#include "QEI_general.h"

void QEI_init (unsigned char channel, unsigned int refresh_freq);
void QEI_set_fs (unsigned char channel, unsigned int refresh_freq);
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
unsigned int QEI_get_velocity (unsigned char channel);
void QEI_calculate_velocity (unsigned char channel);
void QEI_interrupt_handle (unsigned char channel);
#endif