//***************************************************************************//
// File      : codec.h
//
// About     : dsPeak features an on-board audio in/out codec based on NXPs
//             SGTL5000 integrated circuit. It has an audio in line, a micro in,
//             an audio out and a speaker out. In addition, this codec has an
//             integrated equalizer / bass-tone enhancer and many other features
//             such as programmable mic gain, volume control, filtering and
//             automatic volume control.
//
// Functions : 
//
// Includes  : general.h
//           
// Purpose   : Driver for the dsPeak SGTL5000 audio codec using DCI as audio
//             interface and SPI as configuration interface 
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#ifndef __CODEC_H_
#define	__CODEC_H_

#include "general.h"
#include "spi.h"

typedef struct
{
    
}CODEC_struct;

#endif	

