/*
 * File:   lesson1_1.c
 * Author: jeanf
 *
 * Created on July 11, 2021, 9:29 AM
 */
#include "lesson_one.h"
void exercise_1_IO_ports (void)
{
    // Exercise 1 - I/O ports
    // Setup the on-board LED1 to an output and set it's value to ON (1)
    // Setup an on-board pushbutton to an input and map the button state to LED2
    
    //------------------------ Direct register method ------------------------//
    //------------------------------------------------------------------------//
    // Initialize the LED1 I/O port to an output. LED1 is on RH8 (see dsPeak schematic)
    // First, the port must be initialized to an output. The register that sets
    // the port to an output is TRISx (TRIstate, X being port letter)
    // dsPIC mnemonic : 0 = output, 1 = input
    TRISHbits.TRISH8 = 0;   // This line sets RH8 to an output 
    
    // Then we must set the value of the output port to either 1 or 0 (VDD or GND)
    // The LED is connected with a resistor to GND. A '1' will light the LED and
    // a '0' will turn off the LED
    // The register that sets the value of the port is LATx (LATch, X being port letter)
    LATHbits.LATH8 = 1;     // This line sets RH8 to VDD and lights up LED1 
    
    // Now that LED1 is initialized and set to ON, we want to initialize BTN1 to
    // an input, and map the LED2 to the state of BTN1, which means :
    // If BTN1 = 1 (not pressed), the LED2 is ON (=1)
    // If BTN1 = 0 (pressed), the LED2 is OFF (=0)
    // Start by initializing LED2 to an output. LED2 is on RH9
    TRISHbits.TRISH9 = 0;   // This line sets RH9 to an output     
    
    // Set BTN1 to an input. BTN1 is on RA9
    TRISAbits.TRISA9 = 1;   // This line sets RA9 to an input    

    //---------------------------- Define method -----------------------------//
    //------------------------------------------------------------------------//    
    // With known and fixed hardware (such as a development board like dsPeak),
    // it is good practice to define fixed I/O ports in a .h file and use a 
    // relevant name for the I/Os instead of using the TRISx and LATx registers.
    // In dspeak_generic.h, each individual I/O has a name defined that uses the
    // respective TRISx and LATx register to set the direction and value
    DSPEAK_LED1_DIR = 0;    // This is the same as TRISHbits.TRISH8 = 0;
    DSPEAK_LED1_STATE = 1;  // This is the same as LATHbits.LATH8 = 1;
    DSPEAK_LED2_DIR = 0;    // This is the same as TRISHbits.TRISH9 = 0;
    DSPEAK_BTN1_DIR = 1;    // This is the same as TRISAbits.TRISA9 = 1;
    
    while (1)
    {
        if (DSPEAK_BTN1_STATE == 0)
        {
            DSPEAK_LED2_STATE = 0;
        }
        else
        {
            DSPEAK_LED2_STATE = 1;
        }       
    }
}