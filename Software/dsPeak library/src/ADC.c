//***************************************************************************//
// File      : ADC.c
//
// Functions :  void ADC_init (void);
//              void ADC_start (void);
//              void ADC_stop (void);
//              unsigned char ADC_sample_status (void);
//              unsigned int ADC_get_channel (unsigned char channel);
//
// Includes  :  ADC.h
//           
// Purpose   :  Driver for the dsPIC33EP ADC core
//              ADC input on 4 seperate channels :
//              1) Motor 1 current sense
//              2) Motor 2 current sense
//              3) Intellitrol analog input control
//              4) Battery monitor
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#include "ADC.h"

//*************************void ADC_init (void)*******************************//
//Description : Function initializes the dsPIC ADC core
//
//Function prototype : void ADC_init (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_init();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void ADC_init (void)
{
    ADC_stop();                     // Stop ADC if it was already in use
//   
//    // Initialize dsPIC pins to analog inputs
//    // MOTOR_1_CURRENT is AN1 on RA1
//    TRISAbits.TRISA1 = 1;   // Set MOTOR_1_CURRENT pin to input
//    ANSELAbits.ANSA1 = 1;   // Set MOROT_1_CURRENT pin to analog input mode
//    
//    // MOTOR_2_CURRENT is AN3 on RB1
//    TRISBbits.TRISB1 = 1;   // Set MOTOR_2_CURRENT pin to input
//    ANSELBbits.ANSB1 = 1;   // Set MOROT_2_CURRENT pin to analog input mode
//    
//    // ANALOG_input is AN5 on RB3
//    TRISBbits.TRISB3 = 1;   // Set ANALOG_input pin to input
//    ANSELBbits.ANSB3 = 1;   // Set ANALOG_input pin to analog input mode
//    
//    // BATTERY_monitor is AN6 on RC0
//    TRISCbits.TRISC0 = 1;   // Set BATTERY_monitor pin to input
//    ANSELCbits.ANSC0 = 1;   // Set BATTERY_monitor pin to analog input mode       
       
    // Initialize ADC core logic for 12-bits, unsigned integer with auto-conv
    // and automatic sequential sampling of the 4 inputs
    AD1CON1bits.AD12B = 1;  // Select 12 bits resolution
    AD1CON1bits.FORM = 0;   // Select unsigned integer format
    AD1CON1bits.SSRC = 7;   // Internal counter ends conversion (auto-convert)
    AD1CON1bits.SSRCG = 0;  // 
    AD1CON1bits.ASAM = 1;   // Automatic sampling
    
    AD1CON2bits.VCFG = 0;   // AVdd and AVss as voltage reference
    AD1CON2bits.CSCNA = 1;  // Scans input for CH0+
    AD1CON2bits.SMPI = 3;   // Scan 4 different channels (N-1)
    
    AD1CON3 = 0x0F0F;       // Sample for 15*TAD before converting
    AD1CON4 = 0x0000;
    AD1CSSH = 0x0000;
    AD1CSSL = 0x006A;       // Select AN1, AN3, AN5 and AN6 for scanning      
}

//************************void ADC_start (void)*******************************//
//Description : Function starts the dsPIC ADC core and enables interrupt
//
//Function prototype : void ADC_start (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_start();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void ADC_start (void)
{
    IFS0bits.AD1IF = 0;     // Clear flag value
    IEC0bits.AD1IE = 1;     // Enable ADC interrupt
    AD1CON1bits.ADON = 1;   // Start the ADC core
}

//*************************void ADC_stop (void)*******************************//
//Description : Function stops the dsPIC ADC core and disables interrupt
//
//Function prototype : void ADC_stop (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_stop();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void ADC_stop (void)
{
    AD1CON1bits.ADON = 0;   // Stop the ADC core
    IEC0bits.AD1IE = 0;     // Disable ADC interrupt  
    IFS0bits.AD1IF = 0;     // Clear flag value     
}

//***************unsigned char ADC_sample_status (void)***********************//
//Description : Function returns 1 when a new ADC sample is ready to be read
//
//Function prototype : unsigned char ADC_sample_status (void)
//
//Enter params       : None
//
//Exit params        : unsigned char : sample status (1 = ready)
//
//Function call      : unsigned char = ADC_sample_status();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
unsigned char ADC_sample_status (void)
{
    if (ADC_struct.data_ready == ADC_SAMPLE_READY)
    {
        ADC_struct.data_ready = ADC_SAMPLE_NOT_READY;   // Clear flag
        return ADC_SAMPLE_READY;                        // Sample ready to be read
    }
    else return ADC_SAMPLE_NOT_READY;

}

//**********unsigned int ADC_get_channel (unsigned char channel)**************//
//Description : Function returns the value of the selected ADC channel
//
//Function prototype : unsigned int ADC_get_channel (unsigned char channel)
//
//Enter params       : unsigned char channel : ADC channel to read (see ADC.h)
//
//Exit params        : unsigned int : sample value between 0 and 2^12 - 1
//
//Function call      : unsigned int = ADC_get_channel(ADC_MOTOR_1_CHANNEL);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
unsigned int ADC_get_channel (unsigned char channel)
{
    switch(channel)
    {
        case ADC_MOTOR_1_CHANNEL:
            return ADC_struct.motor_1_current;
            break;
            
        case ADC_MOTOR_2_CHANNEL:
            return ADC_struct.motor_2_current;
            break;
            
        case ADC_ANALOG_INPUT_CHANNEL:
            return ADC_struct.analog_input;
            break;
            
        case ADC_BATTERY_CHANNEL:
            return ADC_struct.battery_voltage;
            break;
            
        default:
            return 0;
            break;
    }
}


void __attribute__((__interrupt__, no_auto_psv))_AD1Interrupt(void)
{
    IFS0bits.AD1IF = 0;
    
    // Sum the result for a total of ADC_avg.average_length 
    ADC_avg.motor1_average = ADC_avg.motor1_average + ADC1BUF0;
    ADC_avg.motor2_average = ADC_avg.motor2_average + ADC1BUF1;
    ADC_avg.analog_in_average = ADC_avg.analog_in_average + ADC1BUF2;
    ADC_avg.battery_monitor_average = ADC_avg.battery_monitor_average + ADC1BUF3;
    
    // When the average counter is exceeded, calculate average value for each channel
    // and set the data_ready flag
    if (++ADC_avg.average_counter > ADC_avg.average_length)
    {
        ADC_avg.average_counter = 0;
        ADC_struct.motor_1_current = (unsigned int)(ADC_avg.motor1_average / ADC_avg.average_length);
        ADC_struct.motor_2_current = (unsigned int)(ADC_avg.motor2_average / ADC_avg.average_length);
        ADC_struct.analog_input = (unsigned int)(ADC_avg.analog_in_average / ADC_avg.average_length);
        ADC_struct.battery_voltage = (unsigned int)(ADC_avg.battery_monitor_average / ADC_avg.average_length);
        ADC_avg.motor1_average = 0;
        ADC_avg.motor2_average = 0;
        ADC_avg.analog_in_average = 0;
        ADC_avg.battery_monitor_average = 0;
        ADC_struct.data_ready = ADC_SAMPLE_READY;
    }
}