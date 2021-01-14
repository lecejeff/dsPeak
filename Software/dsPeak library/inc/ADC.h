//***************************************************************************//
// File      : ADC.h
//
// Functions :  void ADC_init (void);
//              void ADC_start (void);
//              void ADC_stop (void);
//              unsigned char ADC_sample_status (void);
//              unsigned int ADC_get_channel (unsigned char channel);
//
// Includes  :  general.h
//           
// Purpose   :  Driver for the dsPIC33EP ADC 10bits and 12bits module
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#ifndef __ADC_H_
#define	__ADC_H_

#include "general.h"

#define ADC_SAMPLE_READY 1
#define ADC_SAMPLE_NOT_READY 0

void ADC_init (void);
void ADC_start (void);
void ADC_stop (void);
unsigned char ADC_sample_status (void);
unsigned int ADC_get_channel (unsigned char channel);

#endif	

