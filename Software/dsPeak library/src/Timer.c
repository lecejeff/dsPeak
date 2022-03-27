//****************************************************************************//
// File      :  timer.c
//
// Functions :  
//
// Includes  :  timer.h
//
// Purpose   :  Driver for the dsPIC33EP 16-bit / 32-bit TIMER core
//              9 seperate 16-bit timers on dsPeak
//              Timer 2-3, 4-5, 6-7 and 8-9 can be paired to a 32b timer
//              
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, Ing
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "Timer.h"

STRUCT_TIMER TIMER_struct[TIMER_QTY];

//
//Description : 
//
//Function prototype : 
//
//Enter params       :  
//
//Exit params        :  
//
//Function call      : 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_init (STRUCT_TIMER *timer, uint8_t channel, uint8_t mode, uint8_t prescaler, uint32_t freq)
{
    uint32_t period = 0;
    if (freq == 1){freq = 1;}    // Cannot divide by 0, otherwise will cause math error trap
    
    // With 1x prescaler, Timer clock = Fcy = 70MHz
    // 16b mode, minimum timer frequency : 70000000 / freq, 1 <= freq <= 65535 = 1069Hz
    // 32b mode, minimum timer frequency : 70000000 / freq, 1 <= freq <= (2^32)-1 = 0.016Hz
    // Maximum timer frequency : 70MHz
    
    // With 8x prescaler, Timer clock is Fcy / 8 -> 8.75MHz
    // 16b mode, minimum timer frequency : 8.75MHz / freq, 1 <= freq <= 65535 = 134Hz
    // 32b mode, minimum timer frequency : 8.75MHz / freq, 1 <= freq <= (2^32)-1 = 0.002Hz
    // Maximum timer frequency : 8.75MHz
    
    // With 64x prescaler, Timer clock is Fcy / 64 -> 1.09375MHz
    // 16b mode, minimum timer frequency : 1.09375MHz / freq, 1 <= freq <= 65535 = 17Hz
    // 32b mode, minimum timer frequency : 1.09375MHz / freq, 1 <= freq <= (2^32)-1 = 0.000254Hz
    // Maximum timer frequency : 1.09375MHz
    
    // with 256x prescaler, Timer clock is Fcy / 256 -> 273437.5Hz
    // 16b mode, minimum timer frequency : 273437.5 / freq, 1 <= freq <= 65535 = 5Hz
    // 32b mode, minimum timer frequency : 273437.5 / freq, 1 <= freq <= (2^32)-1 = 0.0000633Hz
    // Maximum timer frequency : 273437.5Hz
    switch(channel)
    {
        case TIMER_1:  
            if (mode == TIMER_MODE_32B)
            {
                return 0;   // Error, Timer1 is the Type A timer and cannot be set to 32B mode
            }
            
            else if (mode == TIMER_MODE_16B)
            {
                T1CON = 0;                  // Clear Timer1 control register
                T1CONbits.TON = 0;          // Stop Timer
                T1CONbits.TCS = 0;          // Select internal instruction cycle clock
                T1CONbits.TGATE = 0;        // Disable Gated Timer mode
                T1CONbits.TCKPS = prescaler;// Apply prescaler
                TMR1 = 0x00;                // Clear timer register
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR1 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR1 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR1 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR1 = ((FCY / 256) / freq);
                        break;   
                        
                    default:
                        return 0;
                        break;
                }
            }
            
            else
                return 0;
            break;
            
        case TIMER_2:
            // 32b mode timer, use timer control from Type B timer and int control from Type C timer
            if (mode == TIMER_MODE_32B)
            {   
                T2CON = 0;          // Clear Timer2 control register
                T3CON = 0;          // Clear associated timer for 32b operation
                T2CONbits.T32 = 1;
                T2CONbits.TCS = 0; 
                T2CONbits.TGATE = 0;  
                T2CONbits.TCKPS = prescaler; 
                TMR2 = 0x00;
                TMR3 = 0x00;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / freq);
                        break;    
                        
                    default:
                        return 0;
                        break;
                } 
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR3 = ((period & 0xFFFF0000)>>16);
                PR2 = period & 0x0000FFFF;                
            }
            
            // Regular 16-bit timer
            else if (mode == TIMER_MODE_16B)
            {
                T2CON = 0;              // Clear Timer2 control register
                T2CONbits.TON = 0;      
                T2CONbits.TCS = 0; 
                T2CONbits.TGATE = 0;  
                T2CONbits.TCKPS = prescaler;    
                TMR2 = 0x00;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR2 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR2 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR2 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR2 = ((FCY / 256) / freq);
                        break;  
                        
                    default:
                        return 0;
                        break;
                }                
            }
            else 
                return 0;
            break;
            
        case TIMER_3:
            if (mode == TIMER_MODE_32B)
            {
                return 0;               // Must use pair timer for 32b operation
            }  
            
            else if (mode == TIMER_MODE_16B)
            {
                T3CON = 0;              // Clear Timer3 control register
                T3CONbits.TON = 0;    
                T3CONbits.TCS = 0;    
                T3CONbits.TGATE = 0;    
                T3CONbits.TCKPS = prescaler;    
                TMR3 = 0x00;          
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR3 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR3 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR3 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR3 = ((FCY / 256) / freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                }
            }
            else
                return 0;
            break;
            
        case TIMER_4:
            if (mode == TIMER_MODE_32B)
            {
                T4CON = 0;          // Clear Timer4 control register
                T5CON = 0;          // Clear associated timer for 32b operation
                T4CONbits.T32 = 1;
                T4CONbits.TCS = 0; 
                T4CONbits.TGATE = 0;  
                T4CONbits.TCKPS = prescaler; 
                TMR4 = 0x00;
                TMR5 = 0x00;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / freq);
                        break;  
                        
                    default:
                        return 0;
                        break;
                } 
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR5 = ((period & 0xFFFF0000)>>16);
                PR4 = period & 0x0000FFFF;                 
            }
            
            else if (mode == TIMER_MODE_16B)
            {
                T4CON = 0;          // Clear Timer4 control register
                T4CONbits.TON = 0;     
                T4CONbits.TCS = 0;     
                T4CONbits.TGATE = 0;  
                T4CONbits.TCKPS = prescaler; 
                TMR4 = 0x00;          
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR4 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR4 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR4 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR4 = ((FCY / 256) / freq);
                        break;   
                        
                    default:
                        return 0;
                        break;
                }                
            }            
            else 
                return 0;
            break;
            
        case TIMER_5:
            if (mode == TIMER_MODE_32B)
            {
                return 0;               // Must use pair timer for 32b operation
            }
            
            else if (mode == TIMER_MODE_16B)
            {
                T5CON = 0;              // Clear Timer5 control register
                T5CONbits.TON = 0;      
                T5CONbits.TCS = 0;     
                T5CONbits.TGATE = 0;    
                T5CONbits.TCKPS = prescaler; 
                TMR5 = 0x00;            
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR5 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR5 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR5 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR5 = ((FCY / 256) / freq);
                        break;   
                        
                    default:
                        return 0;
                        break;
                }
            }
            else
                return 0;
            break;
            
        case TIMER_6:
            if (mode == TIMER_MODE_32B)
            {
                T6CON = 0;          // Clear Timer6 control register
                T7CON = 0;          // Clear associated timer for 32b operation
                T6CONbits.T32 = 1;
                T6CONbits.TCS = 0; 
                T6CONbits.TGATE = 0;  
                T6CONbits.TCKPS = prescaler; 
                TMR6 = 0x00;
                TMR7 = 0x00;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                } 
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR7 = ((period & 0xFFFF0000)>>16);
                PR6 = period & 0x0000FFFF;                 
            }
            
            else if (mode == TIMER_MODE_16B)
            {
                T6CON = 0;              // Clear Timer6 control register
                T6CONbits.TON = 0;      
                T6CONbits.TCS = 0;     
                T6CONbits.TGATE = 0;    
                T6CONbits.TCKPS = prescaler;    
                TMR6 = 0x00;            
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR6 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR6 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR6 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR6 = ((FCY / 256) / freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                }
            }
            else
                return 0;
            break;
            
        case TIMER_7:
            if (mode == TIMER_MODE_32B)
            {
                return 0;
            }
            
            else if (mode == TIMER_MODE_16B)
            {
                T7CON = 0;              // Clear Timer7 control register
                T7CONbits.TON = 0;     
                T7CONbits.TCS = 0;     
                T7CONbits.TGATE = 0;  
                T7CONbits.TCKPS = prescaler;   
                TMR7 = 0x00;           
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR7 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR7 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR7 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR7 = ((FCY / 256) / freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                }
            }
            else
                return 0;
            break;
            
        case TIMER_8:
            if (mode == TIMER_MODE_32B)
            {
                T8CON = 0;          // Clear Timer8 control register
                T9CON = 0;          // Clear associated timer for 32b operation
                T8CONbits.T32 = 1;
                T8CONbits.TCS = 0; 
                T8CONbits.TGATE = 0;  
                T8CONbits.TCKPS = prescaler; 
                TMR8 = 0x00;
                TMR9 = 0x00;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                } 
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR9 = ((period & 0xFFFF0000)>>16);
                PR8 = period & 0x0000FFFF;                    
            }
            else if (mode == TIMER_MODE_16B)
            {
                T8CON = 0;              // Clear Timer8 control register
                T8CONbits.TON = 0;     
                T8CONbits.TCS = 0;     
                T8CONbits.TGATE = 0;   
                T8CONbits.TCKPS = prescaler;    
                TMR8 = 0x00;           
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR8 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR8 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR8 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR8 = ((FCY / 256) / freq);
                        break;   

                    default:
                        return 0;
                        break;
                }
            }   
            else
                return 0;
            break;
            
        case TIMER_9:
            if (mode == TIMER_MODE_32B)
            {
                return 0;
            }
            
            else if (TIMER_MODE_16B)
            {
                T9CON = 0;
                T9CONbits.TON = 0;   
                T9CONbits.TCS = 0;     
                T9CONbits.TGATE = 0; 
                T9CONbits.TCKPS = prescaler; 
                TMR9 = 0x00;        
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR9 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR9 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR9 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR9 = ((FCY / 256) / freq);
                        break; 

                    default:
                        return 0;
                        break;
                }
            }
            else 
                return 0;
            break;         
                                           
        default: 
            return 0;
            break;
    }
    
    timer->TIMER_channel = channel;
    timer->running = 0;
    timer->freq = freq;
    timer->prescaler = prescaler;
    timer->int_state = 0; 
    timer->mode = mode;
    return 1;
}

uint32_t TIMER_get_freq (STRUCT_TIMER *timer)
{
    return timer->freq;
}

//
//Description : 
//
//Function prototype : 
//
//Enter params       : 
//
//Exit params        : 
//
//Function call      : 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_update_freq (STRUCT_TIMER *timer, uint8_t prescaler, uint32_t new_freq)
{
    uint32_t period = 0;
    switch (timer->TIMER_channel)
    {
        case TIMER_1:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;       // Timer1 cannot be used as a 32b timer
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR1 = 0;
                T1CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR1 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR1 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR1 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR1 = ((FCY / 256) / new_freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                }            
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_2:
            if (timer->mode == TIMER_MODE_32B)
            {
                TIMER_stop(timer);
                TMR2 = 0;
                TMR3 = 0;
                T2CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / new_freq);
                        break; 
                    
                    default:
                        return 0;
                        break;
                }
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR3 = ((period & 0xFFFF0000)>>16);
                PR2 = period & 0x0000FFFF; 
                TIMER_start(timer);                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR2 = 0;
                T2CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR2 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR2 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR2 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR2 = ((FCY / 256) / new_freq);
                        break;                    
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_3:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR3 = 0;
                T3CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR3 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR3 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR3 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR3 = ((FCY / 256) / new_freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_4:
            if (timer->mode == TIMER_MODE_32B)
            {
                TIMER_stop(timer);
                TMR4 = 0;
                TMR5 = 0;
                T4CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / new_freq);
                        break;   

                    default:
                        return 0;
                        break;
                }
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR5 = ((period & 0xFFFF0000)>>16);
                PR4 = period & 0x0000FFFF; 
                TIMER_start(timer);                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR4 = 0;
                T4CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR4 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR4 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR4 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR4 = ((FCY / 256) / new_freq);
                        break;   

                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_5:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR5 = 0;
                T5CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR5 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR5 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR5 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR5 = ((FCY / 256) / new_freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_6:
            if (timer->mode == TIMER_MODE_32B)
            {
                TIMER_stop(timer);
                TMR6 = 0;
                TMR7 = 0;
                T6CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / new_freq);
                        break;  
                        
                    default:
                        return 0;
                        break;
                } 
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR7 = ((period & 0xFFFF0000)>>16);
                PR6 = period & 0x0000FFFF; 
                TIMER_start(timer);                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR6 = 0;
                T6CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR6 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR6 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR6 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR6 = ((FCY / 256) / new_freq);
                        break;  
                        
                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_7:
            if (timer->mode == TIMER_MODE_32B)
            { 
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR7 = 0;
                T7CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR7 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR7 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR7 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR7 = ((FCY / 256) / new_freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_8:
            if (timer->mode == TIMER_MODE_32B)
            {
                TIMER_stop(timer);
                TMR8 = 0;
                TMR9 = 0;
                T8CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        period = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        period = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        period = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        period = ((FCY / 256) / new_freq);
                        break;  
                        
                    default:
                        return 0;
                        break;
                } 
                // PR[y] holds the most significant word
                // PR[x] holds the less significant word                
                PR9 = ((period & 0xFFFF0000)>>16);
                PR8 = period & 0x0000FFFF; 
                TIMER_start(timer);                 
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR8 = 0;
                T8CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR8 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR8 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR8 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR8 = ((FCY / 256) / new_freq);
                        break;   
                    
                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        case TIMER_9:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR9 = 0;
                T9CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR9 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR9 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR9 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR9 = ((FCY / 256) / new_freq);
                        break;     
                        
                    default:
                        return 0;
                        break;
                } 
                TIMER_start(timer);
            }
            else
                return 0;
            break; 
            
        default:
            return 0;
            break;
    }
    return 1;
}

//**********************void TIMER_start (uint8_t timer)**********************//
//Description : Function starts timer module
//
//Function prototype : void TIMER_start (uint8_t timer)
//
//Enter params       : uint8_t timer : TIMER_x module
//
//Exit params        : None
//
//Function call      : TIMER_start(TIMER_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_start (STRUCT_TIMER *timer)
{
    switch (timer->TIMER_channel)
    {
        case TIMER_1:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR1 = 0;
                IEC0bits.T1IE = 1;      // Enable timer interrupt
                IFS0bits.T1IF = 0;      // Clear timer flag
                T1CONbits.TON = 1;      // Start timer      
            }
            else
                return 0;
            break;
            
        case TIMER_2:
            if (timer->mode == TIMER_MODE_32B)
            {
                TMR2 = 0;
                TMR3 = 0;
                IEC0bits.T3IE = 1;      // Enable timer interrupt, must use Timer[y] interrupt registers
                IFS0bits.T3IF = 0;      // Clear timer flag, must use Timer[y] interrupt registers
                T2CONbits.TON = 1;      // Start timer, must use Timer[x] control registers                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR2 = 0;
                IEC0bits.T2IE = 1;      // Enable timer interrupt
                IFS0bits.T2IF = 0;      // Clear timer flag
                T2CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;
            
        case TIMER_3:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR3 = 0;
                IEC0bits.T3IE = 1;      // Enable timer interrupt
                IFS0bits.T3IF = 0;      // Clear timer flag
                T3CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;
            
        case TIMER_4:
            if (timer->mode == TIMER_MODE_32B)
            {
                TMR4 = 0;
                TMR5 = 0;
                IEC1bits.T5IE = 1;      // Enable timer interrupt, must use Timer[y] interrupt register
                IFS1bits.T5IF = 0;      // Clear timer flag, must use Timer[y] interrupt register
                T4CONbits.TON = 1;      // Start timer, must use Timer[x] control register                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR4 = 0;
                IEC1bits.T4IE = 1;      // Enable timer interrupt
                IFS1bits.T4IF = 0;      // Clear timer flag
                T4CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;
            
        case TIMER_5:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR5 = 0;
                IEC1bits.T5IE = 1;      // Enable timer interrupt
                IFS1bits.T5IF = 0;      // Clear timer flag 
                T5CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;
            
        case TIMER_6:
            if (timer->mode == TIMER_MODE_32B)
            {
                TMR6 = 0;
                TMR7 = 0;
                IEC3bits.T7IE = 1;      // Enable timer interrupt, must use Timer[y] interrupt registers
                IFS3bits.T7IF = 0;      // Cleat timer flag, must use Timer[y] interrupt registers
                T6CONbits.TON = 1;      // Start timer, must use Timer[x] control registers                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR6 = 0;
                IEC2bits.T6IE = 1;      // Enable timer interrupt
                IFS2bits.T6IF = 0;      // Cleat timer flag
                T6CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;
            
        case TIMER_7:
            if (timer->mode == TIMER_MODE_32B)
            { 
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR7 = 0;
                IEC3bits.T7IE = 1;      // Enable timer interrupt
                IFS3bits.T7IF = 0;      // Clear timer flag
                T7CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;
            
        case TIMER_8:
            if (timer->mode == TIMER_MODE_32B)
            {
                TMR8 = 0;
                TMR9 = 0;
                IEC3bits.T9IE = 1;      // Enable timer interrupt, must use Timer[y] interrupt registers
                IFS3bits.T9IF = 0;      // Clear timer flag, must use Timer[y] interrupt registers   
                T8CONbits.TON = 1;      // Start timer, must use Timer[x] control registers                    
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR8 = 0;
                IEC3bits.T8IE = 1;      // Enable timer interrupt
                IFS3bits.T8IF = 0;      // Clear timer flag   
                T8CONbits.TON = 1;      // Start timer     
            }
            else
                return 0;
            break;
            
        case TIMER_9:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR9 = 0;
                IEC3bits.T9IE = 1;      // Enable timer interrupt
                IFS3bits.T9IF = 0;      // Clear timer flag    
                T9CONbits.TON = 1;      // Start timer
            }
            else
                return 0;
            break;

        default:
            return 0;
            break;
    }    
    timer->running = 1;
    return 1;
}

//***********************void TIMER_stop (uint8_t timer)**********************//
//Description : Function stops timer module
//
//Function prototype : void TIMER_stop (uint8_t timer)
//
//Enter params       : uint8_t timer : TIMER_x module
//
//Exit params        : None
//
//Function call      : TIMER_stop(TIMER_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_stop (STRUCT_TIMER *timer)
{
    switch (timer->TIMER_channel)
    {
        case TIMER_1:
            if (timer->mode == TIMER_MODE_32B)
            {
                    return 0;           // Invalid, Timer1 is 16b only
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T1CONbits.TON = 0;      // Stop timer               
                IEC0bits.T1IE = 0;      // Disable timer interrupt
                IFS0bits.T1IF = 0;      // Clear timer flag       
            }
            else
                return 0;
            break;
            
        case TIMER_2:
            if (timer->mode == TIMER_MODE_32B)
            {
                T2CONbits.TON = 0;      // Stop timer, must use Timer[x] control register                
                IEC0bits.T3IE = 0;      // Disable timer interrupt, must use Timer[y] interrupt register
                IFS0bits.T3IF = 0;      // Clear timer flag, must use Timer[y] interrupt register                
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T2CONbits.TON = 0;      // Stop timer                
                IEC0bits.T2IE = 0;      // Disable timer interrupt
                IFS0bits.T2IF = 0;      // Clear timer flag
            }
            else 
                return 0;
            break;
            
        case TIMER_3:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;           // Error, Timer3 cannot be set in 32b mode. Must call Timer 2 for 32b mode
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T3CONbits.TON = 0;      // Stop timer            
                IEC0bits.T3IE = 0;      // Disable timer interrupt
                IFS0bits.T3IF = 0;      // Clear timer flag
            }
            else
                return 0;
            break;
            
        case TIMER_4:
            if (timer->mode == TIMER_MODE_32B)
            {
                T4CONbits.TON = 0;      // Stop timer, must use Timer[x] control register                 
                IEC1bits.T5IE = 0;      // Disable timer interrupt, must use Timer[y] interrupt register
                IFS1bits.T5IF = 0;      // Clear timer flag, must use Timer[y] interrupt register                    
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T4CONbits.TON = 0;      // Stop timer            
                IEC1bits.T4IE = 0;      // Disable timer interrupt
                IFS1bits.T4IF = 0;      // Clear timer flag                
            }
            else 
                return 0;

            break;
            
        case TIMER_5:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T5CONbits.TON = 0;      // Stop timer            
                IEC1bits.T5IE = 0;      // Disable timer interrupt
                IFS1bits.T5IF = 0;      // Clear timer flag 
            }
            else
                return 0;
            break;
            
        case TIMER_6:
            if (timer->mode == TIMER_MODE_32B)
            {
                T6CONbits.TON = 0;      // Stop timer, must use Timer[x] control register                 
                IEC3bits.T7IE = 0;      // Disable timer interrupt, must use Timer[y] interrupt register
                IFS3bits.T7IF = 0;      // Clear timer flag, must use Timer[y] interrupt register                        
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T6CONbits.TON = 0;      // Stop timer            
                IEC2bits.T6IE = 0;      // Disable timer interrupt
                IFS2bits.T6IF = 0;      // Cleat timer flag
            }
            else
                return 0;
            break;
            
        case TIMER_7:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T7CONbits.TON = 0;      // Stop timer            
                IEC3bits.T7IE = 0;      // Disable timer interrupt
                IFS3bits.T7IF = 0;      // Clear timer flag
            }
            else
                return 0;
            break;
            
        case TIMER_8:
            if (timer->mode == TIMER_MODE_32B)
            {
                T8CONbits.TON = 0;      // Stop timer, must use Timer[x] control register                 
                IEC3bits.T9IE = 0;      // Disable timer interrupt, must use Timer[y] interrupt register
                IFS3bits.T9IF = 0;      // Clear timer flag, must use Timer[y] interrupt register                    
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T8CONbits.TON = 0;      // Stop timer            
                IEC3bits.T8IE = 0;      // Disable timer interrupt
                IFS3bits.T8IF = 0;      // Clear timer flag        
            }
            else
                return 0;
            break;
            
        case TIMER_9:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T9CONbits.TON = 0;      // Stop timer            
                IEC3bits.T9IE = 0;      // Disable timer interrupt
                IFS3bits.T9IF = 0;      // Clear timer flag    
            }
            else
                return 0;
            break;            
            
        default:
            return 0;
            break;
    }
    timer->running = 0; // Timer is stopped
    return 1;
}

//***********uint8_t TIMER_get_state (uint8_t timer, uint8_t type)************//
//Description : Function returns timer module state
//
//Function prototype : uint8_t TIMER_get_state (uint8_t timer)
//
//Enter params       : uint8_t timer : TIMER_x module
//
//Exit params        : uint8_t : state (0 stopped, 1 active)
//
//Function call      : uint8_t = TIMER_get_state(TIMER_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_get_state (STRUCT_TIMER *timer, uint8_t type)
{
    switch (type)
    {
        case TIMER_RUN_STATE:
            return timer->running;
            break;
            
        case TIMER_INT_STATE:
            if (timer->int_state)
            {
                timer->int_state = 0;
                return 1;
            }
            else return 0;
            break;
            
        default:
            return 0;
            break;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    TIMER_struct[TIMER_1].int_state = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
    TIMER_struct[TIMER_2].int_state = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_T3Interrupt(void)
{
    IFS0bits.T3IF = 0;
    TIMER_struct[TIMER_3].int_state = 1;
    if (TIMER_struct[TIMER_2].mode == TIMER_MODE_32B)
    {
        TIMER_struct[TIMER_2].int_state = 1;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_T4Interrupt(void)
{
    IFS1bits.T4IF = 0;
    TIMER_struct[TIMER_4].int_state = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_T5Interrupt(void)
{
    IFS1bits.T5IF = 0;
    TIMER_struct[TIMER_5].int_state = 1;
    if (TIMER_struct[TIMER_4].mode == TIMER_MODE_32B)
    {
        TIMER_struct[TIMER_4].int_state = 1;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_T6Interrupt(void)
{
    IFS2bits.T6IF = 0;
    TIMER_struct[TIMER_6].int_state = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_T7Interrupt(void)
{
    IFS3bits.T7IF = 0;
    TIMER_struct[TIMER_7].int_state = 1;
    if (TIMER_struct[TIMER_6].mode == TIMER_MODE_32B)
    {
        TIMER_struct[TIMER_6].int_state = 1;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_T8Interrupt(void)
{
    IFS3bits.T8IF = 0;    
    TIMER_struct[TIMER_8].int_state = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_T9Interrupt(void)
{
    IFS3bits.T9IF = 0;
    TIMER_struct[TIMER_9].int_state = 1;
    if (TIMER_struct[TIMER_8].mode == TIMER_MODE_32B)
    {
        TIMER_struct[TIMER_8].int_state = 1;
    }
}