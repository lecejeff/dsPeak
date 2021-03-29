//***************************************************************************//
// File      : ADC.h
//
// About     : dsPIC33E ADC module is a SAR (successive-approximation) type
//             The ADC1 module can be configured for either 10-bit @ 1.1Msps or
//             12-bit @ 500ksps, while the ADC2 module can only be configured
//             for a 10-bit @ 1.1Msps operation             
//
// Functions : void ADC_init (void);
//             void ADC_start (void);
//             void ADC_stop (void);
//             uint8_t ADC_sample_status (void);
//             uint16_t ADC_get_channel (uint8_t channel);
//
// Includes  : general.h
//           
// Purpose   : Driver for the dsPIC33EP ADC 10bits and 12bits module
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#ifndef __ADC_H_
#define	__ADC_H_

#include "general.h"

#define ADC_SAMPLE_READY 1
#define ADC_SAMPLE_NOT_READY 0

typedef struct
{
    uint16_t value;
    uint8_t state;
    uint8_t mode;
    uint8_t resolution;
    uint8_t output_format;
}ADC_struct;

void ADC_init (void);
void ADC_start (void);
void ADC_stop (void);
uint8_t ADC_sample_status (void);
uint16_t ADC_get_channel (uint8_t channel);

#endif	

