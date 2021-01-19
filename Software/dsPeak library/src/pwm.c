//****************************************************************************//
// File      : PWM.c
//
// Functions :  void PWM_init (void);
//              void PWM_change_duty (unsigned char channel, unsigned char duty);
//              unsigned char PWM_get_position (unsigned char channel);
//
// Includes  : pwm.h
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020   
//****************************************************************************//
#include "pwm.h"
STRUCT_PWM PWM_struct[PWM_QTY];

///**************************void PWM_init(void)****************************//
//Description : Function initializes the PWM registers for channels 1,2,3,5,6
//              7,8,9,10,11,12 at central period of 1.45mS, with range of 900us
//              to 2200us
//
//Function prototype : void PWM_init(void)
//
//Enter params       : None
//
//Exit params        : None 
//
//Function call      : PWM_init;
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
///*****************************************************************************
void PWM_init (void)
{
    PTCONbits.PTEN = 0;     // Shutdown PWM engine if it was on 
    
    DTR4 = 0;               // No faults on PWM4 
    ALTDTR4 = 0;
    IOCON4 = 0xCC00;        // Configure PWM4H and PWM4L independent mode 
    PWMCON4 = 0x0200;
    FCLCON4 = 0x0003;    
    
    DTR5 = 0;               // No faults on PWM5
    ALTDTR5 = 0; 
    IOCON5 = 0xAC00;        // Configure PWM5H independent mode with INVERTED polarity
    IOCON5bits.PENH = 1;
    IOCON5bits.PENL = 0;
    PWMCON5 = 0x0200;
    FCLCON5 = 0x0003;

    DTR6 = 0;               // No faults on PWM6 
    ALTDTR6 = 0;
    IOCON6 = 0xFC00;        // Configure PWM6H and PWM6L independent mode with INVERTED polarity
    PWMCON6 = 0x0200;
    FCLCON6 = 0x0003;
  
    PTCON2bits.PCLKDIV = 4; // Divide PWM master clock by 16
    //PTCON2bits.PCLKDIV = 6; // Divide PWM master clock by 64    

    // PWM equations from dsPIC33E datasheet :
    // PHASEx = FOSC / (Fpwm * clock prescaler)
    // PDCx = (FOSC / (Fpwm * clock prescaler)) * desired_duty (0->100%)

    TRISAbits.TRISA7 = 0;   //PWM4L is an output 
    SPHASE4 = (unsigned int)(FOSC / (PWM4L_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
    SDC4 = (unsigned int)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 750us
    PWM_struct[PWM_4L].base_value = 0;               //base value at 40 % max 
    PWM_struct[PWM_4L].end_value = SDC4;         //end value at 20ms
    PWM_struct[PWM_4L].range = PWM_struct[PWM_4L].end_value - PWM_struct[PWM_4L].base_value;

    //
    TRISAbits.TRISA10 = 0;   //PWM4H is an output 
    PHASE4 = (unsigned int)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
    PDC4 = (unsigned int)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 1ms
    PWM_struct[PWM_4H].base_value = 0;               //base value at 40 % max 
    PWM_struct[PWM_4H].end_value = PDC4;             //end value at 20ms
    PWM_struct[PWM_4H].range = PWM_struct[PWM_4H].end_value - PWM_struct[PWM_4H].base_value;   
    
    
    // RGB LED PWM initialization 
    
    // RGB BLUE LED initialization
    //TRISCbits.TRISC8 = 0;   //PWM5H is an output 
    PHASE5 = (unsigned int)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE)); //Phase of 50Hz (20ms)
    PDC5 = (unsigned int)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE));   //
    PWM_struct[PWM_5H].base_value = 0;      //base value at 0
    PWM_struct[PWM_5H].end_value = PDC5;    //end value at 20ms
    PWM_struct[PWM_5H].range = PWM_struct[PWM_5H].end_value - PWM_struct[PWM_5H].base_value;
    
    // RGB GREEN LED initialization
    //TRISCbits.TRISC7 = 0;   //PWM6L is an output 
    SPHASE6 = (unsigned int)(FOSC / (PWM6L_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
    SDC6 = (unsigned int)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 750us
    PWM_struct[PWM_6L].base_value = 0;           //base value at 0
    PWM_struct[PWM_6L].end_value = SDC6;         //end value at 20ms
    PWM_struct[PWM_6L].range = PWM_struct[PWM_6L].end_value - PWM_struct[PWM_6L].base_value;

    // RGB RED LED initialization
    //TRISCbits.TRISC6 = 0;   //PWM6H is an output 
    PHASE6 = (unsigned int)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
    PDC6 = (unsigned int)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 1ms
    PWM_struct[PWM_6H].base_value = 0;               //base value at 0
    PWM_struct[PWM_6H].end_value = PDC6;             //end value at 20ms
    PWM_struct[PWM_6H].range = PWM_struct[PWM_6H].end_value - PWM_struct[PWM_6H].base_value;                                  
    
    // By default, do not drive the motor to any speed. Set duty to 0
    PWM_change_duty(PWM_4L, 0);     // DRV8873 EN2
    PWM_change_duty(PWM_4H, 0);     // DRV8873 EN1
    
    // By default, do not drive the RGB LED. Set duty to 0
    PWM_change_duty(PWM_5H, 0);   // B on RGB led
    PWM_change_duty(PWM_6L, 0);   // G on RGB led
    PWM_change_duty(PWM_6H, 0);   // R on RGB led  
    
    PTCONbits.PTEN = 1; // Enable hardware PWM generator 
}

//**********unsigned char PWM_get_position (unsigned char channel)************//
//Description : Function returns actual duty cycle percentage of PWMx channel
//
//Function prototype : unsigned char PWM_get_position (unsigned char channel)
//
//Enter params       : unsigned char channel : PWMx channel
//
//Exit params        : unsigned char : actual position in percentage 
//
//Function call      : pwm_pos = PWM_get_position(PWM_1H);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
///*****************************************************************************
unsigned char PWM_get_position (unsigned char channel)
{
    return PWM_struct[channel].value_p;
}

//*******void PWM_change_duty (unsigned char channel, unsigned char duty)*****//
//Description : Function changes the duty cycle of the selected PWM channel
//
//Function prototype : void PWM_change_duty (unsigned char channel, unsigned char duty)
//
//Enter params       : unsigned char channel : PWMx channel
//                     unsigned char duty : new duty cycle
//
//Exit params        : None 
//
//Function call      : PWM_change_duty(PWM_1H, 30);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
///*****************************************************************************
void PWM_change_duty (unsigned char channel, unsigned char duty)
{
    if (duty <= 100)
    {
        switch (channel)
        {        
            case PWM_4L:

                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].range * (float)duty/100.0));// Change duty percentage;
                SDC4 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
                break;             

            case PWM_4H:
                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].range * (float)duty/100.0));// Change duty percentage;
                PDC4 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
                break;            
            case PWM_5H:
                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].end_value * (float)duty/100.0));// Change duty percentage 
                PDC5 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
                break;

            case PWM_6L:
                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].end_value * (float)duty/100.0));// Change duty percentage 
                SDC6 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
                break;             

            case PWM_6H:
                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].end_value * (float)duty/100.0));// Change duty percentage 
                PDC6 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
                break;

            default:    
                break;
        }
    }
}