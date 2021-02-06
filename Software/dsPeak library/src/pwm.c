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
//Description : 
//
//Function prototype : void PWM_init(void)
//
//Enter params       : None
//
//Exit params        : None 
//
//Function call      : PWM_init;
//
//
///*****************************************************************************
void PWM_init (unsigned char channel)
{
    PTCONbits.PTEN = 0;     // Shutdown PWM engine if it was on 
    
    switch (channel)
    {
        case PWM_1L:
            DTR1 = 0;               // No PWM dead time for PWM1
            ALTDTR1 = 0;            //   
            IOCON1bits.PENL = 1;    // Enable PWM_1L
            IOCON1bits.PMOD = 3;    // True independent PWM
            PWMCON1bits.ITB = 1;    // Independent time base for PWM_1L / PWM_1H pair
            FCLCON1bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISEbits.TRISE0 = 0;   // RE0 is an output
            // The phase is defined in pwm.h for each PWM channel
            SPHASE1 = (unsigned int)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            SDC1 = (unsigned int)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            PWM_struct[PWM_1L].base_value = 0;                 
            PWM_struct[PWM_1L].end_value = SDC1;                
            PWM_struct[PWM_1L].range = PWM_struct[PWM_1L].end_value - PWM_struct[PWM_1L].base_value;   
            
            PWM_change_duty(PWM_1L, 0);
            break;
            
        case PWM_1H:
            DTR1 = 0;               // No PWM dead time for PWM1
            ALTDTR1 = 0;            //   
            IOCON1bits.PENH = 1;    // Enable PWM_1L
            IOCON1bits.PMOD = 3;    // True independent PWM
            PWMCON1bits.ITB = 1;    // Independent time base for PWM_1L / PWM_1H pair
            FCLCON1bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISEbits.TRISE1 = 0;   // RE1 is an output
            // The phase is defined in pwm.h for each PWM channel
            PHASE1 = (unsigned int)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            PDC1 = (unsigned int)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            PWM_struct[PWM_1H].base_value = 0;                 
            PWM_struct[PWM_1H].end_value = SDC1;                
            PWM_struct[PWM_1H].range = PWM_struct[PWM_1H].end_value - PWM_struct[PWM_1H].base_value;  
            
            PWM_change_duty(PWM_1H, 0);
            break;
            
        case PWM_2L:
            break;
            
        case PWM_2H:
            break;
            
        case PWM_4H:
            break;
            
        case PWM_5L:
            break;
            
        case PWM_5H:
            break;
            
        case PWM_6L:
            break;
            
        case PWM_6H:
            break;
            
            default:
            break;
    }
    
//    DTR4 = 0;               // No faults on PWM4 
//    ALTDTR4 = 0;
//    IOCON4 = 0xCC00;        // Configure PWM4H and PWM4L independent mode 
//    PWMCON4 = 0x0200;
//    FCLCON4 = 0x0003;    
//    
//    DTR5 = 0;               // No faults on PWM5
//    ALTDTR5 = 0; 
//    IOCON5 = 0xAC00;        // Configure PWM5H independent mode with INVERTED polarity
//    IOCON5bits.PENH = 1;
//    IOCON5bits.PENL = 0;
//    PWMCON5 = 0x0200;
//    FCLCON5 = 0x0003;
//
//    DTR6 = 0;               // No faults on PWM6 
//    ALTDTR6 = 0;
//    IOCON6 = 0xFC00;        // Configure PWM6H and PWM6L independent mode with INVERTED polarity
//    PWMCON6 = 0x0200;
//    FCLCON6 = 0x0003;
//  
//    PTCON2bits.PCLKDIV = 4; // Divide PWM master clock by 16
//    //PTCON2bits.PCLKDIV = 6; // Divide PWM master clock by 64    
//
//    // PWM equations from dsPIC33E datasheet :
//    // PHASEx = FOSC / (Fpwm * clock prescaler)
//    // PDCx = (FOSC / (Fpwm * clock prescaler)) * desired_duty (0->100%)
//
//    TRISAbits.TRISA7 = 0;   //PWM4L is an output 
//    SPHASE4 = (unsigned int)(FOSC / (PWM4L_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
//    SDC4 = (unsigned int)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 750us
//    PWM_struct[PWM_4L].base_value = 0;                  //base value at 40 % max 
//    PWM_struct[PWM_4L].end_value = SDC4;                //end value at 20ms
//    PWM_struct[PWM_4L].range = PWM_struct[PWM_4L].end_value - PWM_struct[PWM_4L].base_value;
//
//    //
//    TRISAbits.TRISA10 = 0;   //PWM4H is an output 
//    PHASE4 = (unsigned int)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
//    PDC4 = (unsigned int)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 1ms
//    PWM_struct[PWM_4H].base_value = 0;               //base value at 40 % max 
//    PWM_struct[PWM_4H].end_value = PDC4;             //end value at 20ms
//    PWM_struct[PWM_4H].range = PWM_struct[PWM_4H].end_value - PWM_struct[PWM_4H].base_value;   
//    
//    
//    // RGB LED PWM initialization 
//    
//    // RGB BLUE LED initialization
//    //TRISCbits.TRISC8 = 0;   //PWM5H is an output 
//    PHASE5 = (unsigned int)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE)); //Phase of 50Hz (20ms)
//    PDC5 = (unsigned int)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE));   //
//    PWM_struct[PWM_5H].base_value = 0;      //base value at 0
//    PWM_struct[PWM_5H].end_value = PDC5;    //end value at 20ms
//    PWM_struct[PWM_5H].range = PWM_struct[PWM_5H].end_value - PWM_struct[PWM_5H].base_value;
//    
//    // RGB GREEN LED initialization
//    //TRISCbits.TRISC7 = 0;   //PWM6L is an output 
//    SPHASE6 = (unsigned int)(FOSC / (PWM6L_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
//    SDC6 = (unsigned int)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 750us
//    PWM_struct[PWM_6L].base_value = 0;           //base value at 0
//    PWM_struct[PWM_6L].end_value = SDC6;         //end value at 20ms
//    PWM_struct[PWM_6L].range = PWM_struct[PWM_6L].end_value - PWM_struct[PWM_6L].base_value;
//
//    // RGB RED LED initialization
//    //TRISCbits.TRISC6 = 0;   //PWM6H is an output 
//    PHASE6 = (unsigned int)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));//Phase of 50Hz (20ms)
//    PDC6 = (unsigned int)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));//Duty cycle base is 1ms
//    PWM_struct[PWM_6H].base_value = 0;               //base value at 0
//    PWM_struct[PWM_6H].end_value = PDC6;             //end value at 20ms
//    PWM_struct[PWM_6H].range = PWM_struct[PWM_6H].end_value - PWM_struct[PWM_6H].base_value;                                  
    
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
            case PWM_1L:
                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].range * (float)duty/100.0));// Change duty percentage;
                SDC1 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
                break;  
                
            case PWM_1H:
                PWM_struct[channel].value_p = duty;
                PWM_struct[channel].new_duty = (unsigned int)((PWM_struct[channel].range * (float)duty/100.0));// Change duty percentage;
                PDC1 = PWM_struct[channel].new_duty; // Write the value to duty cycle register 
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