//****************************************************************************//
// File      :  main.c
//
// Includes  :  dspeak_generic.h
//              Project-related .h files
//
// Purpose   : dsPeak lesson 1 : Oscillator and I/Os
//
// The purpose of this 1st lesson is to learn the basics of how to configure the
// oscillators on dsPeak (Primary, FRC, PLL and secondary oscillator)
// We will also cover the basic theory of dsPeak I/Os, remappable peripherals,
// built-in pull-ups, change notice, analog and digital port configurations
// 
// Recommended dsPIC33E Family reference manuals / datasheet :
//      1) DS70000598C  - I/O Ports
//      2) DS70580C     - Oscillator
//      3) DS70616G     - MCU Datasheet
//      4) DS80000526H  - MCU Errata
//
// Recommended dsPeak documentation :
//      1) dsPeak schematic
//
// Intellitrol                   MPLab X v5.45                        07/07/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//

//----------------------------------------------------------------------------//
// dsPIC33EP512MU814 configuration fuses, do not remove
// Configuration settings can be changed in "Production -> Set configuration bits"
// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Primary oscillator at POR is FRC
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select Bit (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function Bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock-switching enabled, Fail-safe clock monitor disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = ON               // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled )
#pragma config ALTI2C1 = ON             // Alternate I2C pins for I2C1 (ASDA1/ASCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = ON             // Alternate I2C pins for I2C2 (ASDA2/ASCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable Bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Aux Flash may be written)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)
// End of dsPIC33EP512MU814 configuration fuses ------------------------------//

#include "dspeak_generic.h"

int main (void)
{
    // Before playing with oscillator, make sure watchdog timer is disabled
    RCONbits.SWDTEN = 0;                            // Watchdog timer disabled 
    INTCON1bits.NSTDIS = 0;                         // Nested interrupt enabled 
    
    // Start by configuring the primary oscillator
    #ifdef POSC_20MHz_70MIPS
        dsPeak_posc_20MHz_init();
    #endif

    #ifdef POSC_FRC_66MIPS
        dsPeak_posc_FRC_init();
    #endif

    // Initialize the LED1 I/O port to an output. LED1 is on RH8
    // First, the port must be initialized to an output. The register that sets
    // the port to an output is TRISx (TRIstate, X being port letter)
    // dsPIC mnemonic : 0 = output, 1 = input
    TRISHbits.TRISH8 = 0;   // This line sets RH8 to an output
    
    // Then we must set the value of the output port to either 1 or 0 (VDD or GND)
    // The LED is connected with a resistor to GND. A '1' will light the LED and
    // a '0' will turn off the LED
    // The register that sets the value of the port is LATx (LATch, X being port letter)
    LATHbits.LATH8 = 1;     // This line sets RH8 to VDD and lights up LED1
    
    while (1)
    {
        // In the while loop, we will make the LED flash using the delay function
        __delay_ms(1000);                   // Halt program execution for 1000ms (1s)
        LATHbits.LATH8 = !LATHbits.LATH8;   // Flip RH8 value 
    }
    return 0;
}