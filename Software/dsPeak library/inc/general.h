//***************************************************************************//
// File      : general.h
//
// Includes  : xc.h
//
// Purpose   : General header file
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2020
//****************************************************************************//

#ifndef __GENERAL_H_
#define	__GENERAL_H_

#define FOSC 140000000
#define FCY (FOSC/2)    // FCY of 70MIPS

#include <xc.h>
#include <libpic30.h>
#include <dsp.h>

void DSPIC_init (void);

#endif	/* GENERAL_H */

