//****************************************************************************//
// File      :  timer.c
//
// Functions :  void TIMER_init (uint8_t timer, uint8_t prescaler, uint32_t freq);
//              void TIMER_update_freq (uint8_t timer, uint8_t prescaler, uint32_t new_freq);
//              void TIMER_start (uint8_t timer);
//              void TIMER_stop (uint8_t timer);
//              uint8_t TIMER_get_state (uint8_t timer, uint8_t type);
//
// Includes  :  timer.h
//
// Purpose   :  Driver for the dsPIC33EP 16-bit / 32-bit TIMER core
//              9 seperate 16-bit timers on dsPeak
//              TIMER_1 : 
//              TIMER_2 : 
//              TIMER_3 : 
//              TIMER_4 : 
//              TIMER_5 : 
//              TIMER_6 : 
//              TIMER_7 : 
//              TIMER_8 : 
//              TIMER_9 : Reserved to SPI_2 for non-blocking function calls
//              
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "Timer.h"

TIMER_STRUCT TIMER_struct[TIMER_QTY];

//*****void TIMER_init (uint8_t timer, uint8_t prescaler, uint32_t freq)******//
//Description : Function initializes timer module with interrupt at specified
//              freq
//
//Function prototype : void TIMER_init (uint8_t timer, uint8_t prescaler, uint32_t freq)
//
//Enter params       :  uint8_t timer : TIMER_x module
//                      uint8_t prescaler : TIMER_x prescaler
//                      uint32_t freq : Timer frequency
//
//Exit params        :  None
//
//Function call      :  TIMER_init(TIMER_1, TIMER_PRESCALER_256, 10);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void TIMER_init (uint8_t timer, uint8_t prescaler, uint32_t freq)
{
    // 16-bit timers
    // With 1x prescaler, clock freq is 70MHz
    // Minimum timer frequency : 70000000 / freq, 0<freq<=65535 = >>1069Hz<<
    // With 8x prescaler, clock freq is 70MHz / 8 = 8.75MHz
    // Minimum timer frequency : 8.75MHz / freq, 0<=freq<=65535 = >>134Hz<<
    // With 64x prescaler, clock freq is 70MHz / 64 = 1.09375MHz
    // Minimum timer frequency : 1.09375MHz / freq, 0<=freq<=65535 = >>17Hz<<    
    // with 256x prescaler, clock freq is 70MHz / 256 = 273437.5Hz 
    // Minimum timer frequency : 273437 / freq, 0<=freq<=65535 = >>5Hz<<
    TIMER_struct[timer].freq = freq;
    TIMER_struct[timer].int_state = 0;
    TIMER_struct[timer].run_state = 0;
    switch(timer)
    {
        case TIMER_1:  
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
            }
            break;
            
        case TIMER_2:
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
            }
            break;
            
        case TIMER_3:
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
            }
            break;
            
        case TIMER_4:
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
            }
            break;
            
        case TIMER_5:
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
            }
            break;
            
        // PID FS timer
        case TIMER_6:
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
            }
            break;
            
        case TIMER_7:
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
            }
            break;
            
        case TIMER_8:
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
            }
            break;
            
        case TIMER_9:
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
            }
            break;          
    }
}

uint32_t TIMER_get_freq (uint8_t timer)
{
    return TIMER_struct[timer].freq;
}

//void TIMER_update_freq (uint8_t timer, uint8_t prescaler, uint32_t new_freq)//
//Description : Function updates timer module to new frequency using the 
//              specified timer prescaler
//
//Function prototype : void TIMER_update_freq (uint8_t timer, uint8_t prescaler, uint32_t new_freq)
//
//Enter params       : uint8_t timer : TIMER_x module
//                     uint32_t new_freq  : Timer frequency
//
//Exit params        : None
//
//Function call      : TIMER_update_freq(TIMER_1, 8, 150);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void TIMER_update_freq (uint8_t timer, uint8_t prescaler, uint32_t new_freq)
{
    switch (timer)
    {
        case TIMER_1:
            TIMER_stop(TIMER_1);
            TMR1 = 0;
            T1CONbits.TCKPS = prescaler;
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
            }            
            TIMER_start(TIMER_1);
            break;
            
        case TIMER_2:
            TIMER_stop(TIMER_2);
            TMR2 = 0;
            T2CONbits.TCKPS = prescaler;
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
            TIMER_start(TIMER_2);
            break;
            
        case TIMER_3:
            TIMER_stop(TIMER_3);
            TMR3 = 0;
            T3CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_3);
            break;
            
        case TIMER_4:
            TIMER_stop(TIMER_4);
            TMR4 = 0;
            T4CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_4);
            break;
            
        case TIMER_5:
            TIMER_stop(TIMER_5);
            TMR5 = 0;
            T5CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_5);
            break;
            
        case TIMER_6:
            TIMER_stop(TIMER_6);
            TMR6 = 0;
            T6CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_6);
            break;
            
        case TIMER_7:
            TIMER_stop(TIMER_7);
            TMR7 = 0;
            T7CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_7);
            break;
            
        case TIMER_8:
            TIMER_stop(TIMER_8);
            TMR8 = 0;
            T8CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_8);
            break;
            
        case TIMER_9:
            TIMER_stop(TIMER_9);
            TMR9 = 0;
            T9CONbits.TCKPS = prescaler;
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
            } 
            TIMER_start(TIMER_9);
            break;            
    }
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
void TIMER_start (uint8_t timer)
{
    switch (timer)
    {
        case TIMER_1:
            TMR1 = 0;
            IEC0bits.T1IE = 1;      // Enable timer interrupt
            IFS0bits.T1IF = 0;      // Clear timer flag
            T1CONbits.TON = 1;      // Start timer            
            break;
            
        case TIMER_2:
            TMR2 = 0;
            IEC0bits.T2IE = 1;      // Enable timer interrupt
            IFS0bits.T2IF = 0;      // Clear timer flag
            T2CONbits.TON = 1;      // Start timer
            break;
            
        case TIMER_3:
            TMR3 = 0;
            IEC0bits.T3IE = 1;      // Enable timer interrupt
            IFS0bits.T3IF = 0;      // Clear timer flag
            T3CONbits.TON = 1;      // Start timer
            break;
            
        case TIMER_4:
            TMR4 = 0;
            IEC1bits.T4IE = 1;      // Enable timer interrupt
            IFS1bits.T4IF = 0;      // Clear timer flag
            T4CONbits.TON = 1;      // Start timer
            break;
            
        case TIMER_5:
            TMR5 = 0;
            IEC1bits.T5IE = 1;      // Enable timer interrupt
            IFS1bits.T5IF = 0;      // Clear timer flag 
            T5CONbits.TON = 1;      // Start timer
            break;
            
        case TIMER_6:
            TMR6 = 0;
            IEC2bits.T6IE = 1;      // Enable timer interrupt
            IFS2bits.T6IF = 0;      // Cleat timer flag
            T6CONbits.TON = 1;      // Start timer
            break;
            
        case TIMER_7:
            TMR7 = 0;
            IEC3bits.T7IE = 1;      // Enable timer interrupt
            IFS3bits.T7IF = 0;      // Clear timer flag
            T7CONbits.TON = 1;      // Start timer
            break;
            
        case TIMER_8:
            TMR8 = 0;
            IEC3bits.T8IE = 1;      // Enable timer interrupt
            IFS3bits.T8IF = 0;      // Clear timer flag   
            T8CONbits.TON = 1;      // Start timer       
            break;
            
        case TIMER_9:
            TMR9 = 0;
            IPC13bits.T9IP = 7;     // SW PWM timer, high interrupt priority
            IEC3bits.T9IE = 1;      // Enable timer interrupt
            IFS3bits.T9IF = 0;      // Clear timer flag    
            T9CONbits.TON = 1;      // Start timer
            break;            
    }    
    TIMER_struct[timer].run_state = 1; // Timer is started
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
void TIMER_stop (uint8_t timer)
{
    switch (timer)
    {
        case TIMER_1:
            T1CONbits.TON = 0;      // Stop timer               
            IEC0bits.T1IE = 0;      // Disable timer interrupt
            IFS0bits.T1IF = 0;      // Clear timer flag         
            break;
            
        case TIMER_2:
            T2CONbits.TON = 0;      // Stop timer                
            IEC0bits.T2IE = 0;      // Disable timer interrupt
            IFS0bits.T2IF = 0;      // Clear timer flag
            break;
            
        case TIMER_3:
            T3CONbits.TON = 0;      // Stop timer            
            IEC0bits.T3IE = 0;      // Disable timer interrupt
            IFS0bits.T3IF = 0;      // Clear timer flag
            break;
            
        case TIMER_4:
            T4CONbits.TON = 0;      // Stop timer            
            IEC1bits.T4IE = 0;      // Disable timer interrupt
            IFS1bits.T4IF = 0;      // Clear timer flag
            break;
            
        case TIMER_5:
            T5CONbits.TON = 0;      // Stop timer            
            IEC1bits.T5IE = 0;      // Disable timer interrupt
            IFS1bits.T5IF = 0;      // Clear timer flag 
            break;
            
        case TIMER_6:
            T6CONbits.TON = 0;      // Stop timer            
            IEC2bits.T6IE = 0;      // Disable timer interrupt
            IFS2bits.T6IF = 0;      // Cleat timer flag
            break;
            
        case TIMER_7:
            T7CONbits.TON = 0;      // Stop timer            
            IEC3bits.T7IE = 0;      // Disable timer interrupt
            IFS3bits.T7IF = 0;      // Clear timer flag
            break;
            
        case TIMER_8:
            T8CONbits.TON = 0;      // Stop timer            
            IEC3bits.T8IE = 0;      // Disable timer interrupt
            IFS3bits.T8IF = 0;      // Clear timer flag          
            break;
            
        case TIMER_9:
            T9CONbits.TON = 0;      // Stop timer            
            IEC3bits.T9IE = 0;      // Disable timer interrupt
            IFS3bits.T9IF = 0;      // Clear timer flag    
            break;            
    }
    TIMER_struct[timer].run_state = 0; // Timer is stopped
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
uint8_t TIMER_get_state (uint8_t timer, uint8_t type)
{
    switch (type)
    {
        case TIMER_RUN_STATE:
            return TIMER_struct[timer].run_state;
            break;
            
        case TIMER_INT_STATE:
            if (TIMER_struct[timer].int_state)
            {
                TIMER_struct[timer].int_state = 0;
                return 1;
            }
            else return 0;
            break;
            
        default:
            return 1;
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
}