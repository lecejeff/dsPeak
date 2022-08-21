//****************************************************************************//
// File      : PWM.c
//
// Functions :  void PWM_init (void);
//              void PWM_change_duty (uint8_t channel, uint8_t duty);
//              uint8_t PWM_get_position (uint8_t channel);
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
void PWM_init (STRUCT_PWM *pwm, uint8_t channel, uint8_t type)
{
    PTCONbits.PTEN = 0;     // Shutdown PWM engine if it was on 
    PTCON2bits.PCLKDIV = 6; // Clock divide by 1
    pwm->PWM_channel = channel;  
    pwm->pwm_type = type;
    
    switch (pwm->PWM_channel)
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
            SPHASE1 = (uint16_t)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            SDC1 = (uint16_t)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load pwm->pwm_type, the PWM channel acts as a 0-100% variable duty cycle
            if (type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = SDC1;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(SDC1 * PWM1L_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(SDC1 * PWM1L_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_1H:
            DTR1 = 0;               // No PWM dead time for PWM1
            ALTDTR1 = 0;            //   
            IOCON1bits.PENH = 1;    // Enable PWM_1H
            IOCON1bits.PMOD = 3;    // True independent PWM
            PWMCON1bits.ITB = 1;    // Independent time base for PWM_1L / PWM_1H pair
            FCLCON1bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISEbits.TRISE1 = 0;   // RE1 is an output
            // The phase is defined in pwm.h for each PWM channel
            PHASE1 = (uint16_t)(FOSC / (PWM1H_PHASE * PWM_CLOCK_PRESCALE));
            PDC1 = (uint16_t)(FOSC / (PWM1H_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PDC1;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PDC1 * PWM1H_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(PDC1 * PWM1H_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_2L:
            DTR2 = 0;               // No PWM dead time for PWM2
            ALTDTR2 = 0;            //   
            IOCON2bits.PENL = 1;    // Enable PWM_2L
            IOCON2bits.PMOD = 3;    // True independent PWM
            PWMCON2bits.ITB = 1;    // Independent time base for PWM_2L / PWM_2H pair
            FCLCON2bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISEbits.TRISE2 = 0;   // RE2 is an output
            // The phase is defined in pwm.h for each PWM channel
            SPHASE2 = (uint16_t)(FOSC / (PWM2L_PHASE * PWM_CLOCK_PRESCALE));
            SDC2 = (uint16_t)(FOSC / (PWM2L_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = SDC2;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(SDC2 * PWM2L_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(SDC2 * PWM2L_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_2H:
            DTR2 = 0;               // No PWM dead time for PWM2
            ALTDTR2 = 0;            //   
            IOCON2bits.PENH = 1;    // Enable PWM_2H
            IOCON2bits.PMOD = 3;    // True independent PWM
            PWMCON2bits.ITB = 1;    // Independent time base for PWM_2L / PWM_2H pair
            FCLCON2bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISEbits.TRISE3 = 0;   // RE3 is an output
            // The phase is defined in pwm.h for each PWM channel
            PHASE2 = (uint16_t)(FOSC / (PWM2H_PHASE * PWM_CLOCK_PRESCALE));
            PDC2 = (uint16_t)(FOSC / (PWM2H_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PDC2;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PDC2 * PWM2H_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(PDC2 * PWM2H_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_4H:
            DTR4 = 0;               // No PWM dead time for PWM4
            ALTDTR4 = 0;            //   
            IOCON4bits.PENH = 1;    // Enable PWM_4H
            IOCON4bits.PMOD = 3;    // True independent PWM
            PWMCON4bits.ITB = 1;    // Independent time base for PWM_4L / PWM_4H pair
            FCLCON4bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISEbits.TRISE7 = 0;   // RE7 is an output
            // The phase is defined in pwm.h for each PWM channel
            PHASE4 = (uint16_t)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));
            PDC4 = (uint16_t)(FOSC / (PWM4H_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PDC4;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PDC4 * PWM4H_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PDC4 * PWM4H_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_5L:
            DTR5 = 0;               // No PWM dead time for PWM5
            ALTDTR5 = 0;            //   
            IOCON5bits.PENL = 1;    // Enable PWM_5L
            IOCON5bits.PMOD = 3;    // True independent PWM
            PWMCON5bits.ITB = 1;    // Independent time base for PWM_5L / PWM_5H pair
            FCLCON5bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISCbits.TRISC1 = 0;   // RC1 is an output
            // The phase is defined in pwm.h for each PWM channel
            SPHASE5 = (uint16_t)(FOSC / (PWM5L_PHASE * PWM_CLOCK_PRESCALE));
            SDC5 = (uint16_t)(FOSC / (PWM5L_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = SDC5;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(SDC5 * PWM5L_SERVO_BASE);                
                pwm->end_value = (uint16_t)(SDC5 * PWM5L_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }               
            break;
            
        case PWM_5H:
            DTR5 = 0;               // No PWM dead time for PWM5
            ALTDTR5 = 0;            //   
            IOCON5bits.PENH = 1;    // Enable PWM_5H
            IOCON5bits.PMOD = 3;    // True independent PWM
            PWMCON5bits.ITB = 1;    // Independent time base for PWM_5L / PWM_5H pair
            FCLCON5bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISCbits.TRISC2 = 0;   // RC2 is an output
            // The phase is defined in pwm.h for each PWM channel
            PHASE5 = (uint16_t)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE));
            PDC5 = (uint16_t)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PDC5;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PDC5 * PWM5H_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PDC5 * PWM5H_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }                     
            break;
            
        case PWM_6L:
            DTR6 = 0;               // No PWM dead time for PWM6
            ALTDTR6 = 0;            //   
            IOCON6bits.PENL = 1;    // Enable PWM_6L
            IOCON6bits.PMOD = 3;    // True independent PWM
            PWMCON6bits.ITB = 1;    // Independent time base for PWM_6L / PWM_6H pair
            FCLCON6bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISCbits.TRISC3 = 0;   // RC3 is an output
            // The phase is defined in pwm.h for each PWM channel
            SPHASE6 = (uint16_t)(FOSC / (PWM6L_PHASE * PWM_CLOCK_PRESCALE));
            SDC6 = (uint16_t)(FOSC / (PWM6L_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = SDC6;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(SDC6 * PWM6L_SERVO_BASE);                
                pwm->end_value = (uint16_t)(SDC6 * PWM6L_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_6H:
            DTR6 = 0;               // No PWM dead time for PWM6
            ALTDTR6 = 0;            //   
            IOCON6bits.PENH = 1;    // Enable PWM_6H
            IOCON6bits.PMOD = 3;    // True independent PWM
            PWMCON6bits.ITB = 1;    // Independent time base for PWM_6L / PWM_6H pair
            FCLCON6bits.FLTMOD = 3; // No PWM fault can occur
            
            TRISCbits.TRISC4 = 0;   // RC4 is an output
            // The phase is defined in pwm.h for each PWM channel
            PHASE6 = (uint16_t)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));
            PDC6 = (uint16_t)(FOSC / (PWM6H_PHASE * PWM_CLOCK_PRESCALE));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PDC6;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PDC6 * PWM6H_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PDC6 * PWM6H_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }             
            break;
            
            default:
            break;
    }
    
    PWM_change_duty_perc(pwm, 0);   // Initialize the channel at either 0 duty cycle, or base duty cycle if servo    
    PTCONbits.PTEN = 1;             // Enable hardware PWM generator 
}

//**********uint8_t PWM_get_position (uint8_t channel)************//
//Description : Function returns actual duty cycle percentage of PWMx channel
//
//Function prototype : uint8_t PWM_get_position (uint8_t channel)
//
//Enter params       : uint8_t channel : PWMx channel
//
//Exit params        : uint8_t : actual position in percentage 
//
//Function call      : pwm_pos = PWM_get_position(PWM_1H);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
///*****************************************************************************
uint8_t PWM_get_position (STRUCT_PWM *pwm)
{
    return pwm->value_p;
}

//*******void PWM_change_duty (uint8_t channel, uint8_t duty)*****//
//Description : Function changes the duty cycle of the selected PWM channel
//
//Function prototype : void PWM_change_duty (uint8_t channel, uint8_t duty)
//
//Enter params       : uint8_t channel : PWMx channel
//                     uint8_t duty : new duty cycle
//
//Exit params        : None 
//
//Function call      : PWM_change_duty(PWM_1H, 30);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
///*****************************************************************************
void PWM_change_duty_perc (STRUCT_PWM *pwm, uint8_t duty)
{
    if (duty > 100){duty = 100;}

    if (pwm->pwm_type == PWM_TYPE_LOAD)
    {
        pwm->new_duty = (uint16_t)((pwm->range * (float)duty/100.0));                   // Change duty percentage;
    }
    else
    {
        pwm->new_duty = (uint16_t)((pwm->range * ((float)duty/100.0)) + pwm->base_value);// Change duty percentage                     
    }
    pwm->value_p = duty;     // Update struct variable with new percentage

    switch (pwm->PWM_channel)
    {        
        case PWM_1L:               
            SDC1 = pwm->new_duty;    // Update duty cycle
            break;  

        case PWM_1H:
            PDC1 = pwm->new_duty;    // Update duty cycle
            break;                

        case PWM_2L:               
            SDC2 = pwm->new_duty;    // Update duty cycle
            break;  

        case PWM_2H:
            PDC2 = pwm->new_duty;    // Update duty cycle
            break;                

        case PWM_4H:
            PDC4 = pwm->new_duty;    // Update duty cycle
            break; 

        case PWM_5L:
            SDC5 = pwm->new_duty;    // Update duty cycle
            break;

        case PWM_5H:
            PDC5 = pwm->new_duty;    // Update duty cycle
            break;

        case PWM_6L:
            SDC6 = pwm->new_duty;    // Update duty cycle
            break;             

        case PWM_6H:
            PDC6 = pwm->new_duty;    // Update duty cycle
            break;

        default:    
            break;
    }
}