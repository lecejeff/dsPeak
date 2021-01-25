//****************************************************************************//
// File      : Timer.c
//
// Functions :  void TIMER_init (unsigned char timer, unsigned long freq);
//              void TIMER_update_freq (unsigned char timer, unsigned long new_freq);
//              void TIMER_start (unsigned char timer);
//              void TIMER_stop (unsigned char timer);
//              unsigned char TIMER_get_state (unsigned char timer);
//
// Includes  : Timer.h
//
//Jean-Francois Bilodeau    MPLab X v5.00    10/09/2018    
//****************************************************************************//
#include "Timer.h"

TIMER_STRUCT TIMER_struct[TIMER_QTY];

// Current timer allocation
//*********void TIMER_init (unsigned char timer, unsigned long Freq)**********//
//Description : Function initializes timer module with interrupt at specified
//              freq
//
//Function prototype : void TIMER_init (unsigned char timer, unsigned long Freq)
//
//Enter params       : unsigned char timer : TIMER_x module
//                     unsigned long freq  : Timer frequency
//
//Exit params        : None
//
//Function call      : TIMER_init(TIMER_1, 150);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
void TIMER_init (unsigned char timer, unsigned long Freq)
{
    // 16 bit timers. With 8x prescaler, clock freq is 70MHz / 8 = 8.75MHz
    // Minimum frequency timer : 8.75MHz / Freq <= 65535, Freq >= 133
    // with 256x prescaler, clock freq is 70MHz / 256 = 273437.5Hz 
    // Minimum frequency timer : 273437 / Freq <= 65535, Freq >= 4
       
    TIMER_struct[timer].int_state = 0;
    TIMER_struct[timer].run_state = 0;
    switch(timer)
    {
        case TIMER_1:  
            T1CONbits.TON = 0;      // Stop Timer
            T1CONbits.TCS = 0;      // Select internal instruction cycle clock
            T1CONbits.TGATE = 0;    // Disable Gated Timer mode
            T1CONbits.TCKPS = 1;    // Select 1:8 Prescaler
            TMR1 = 0x00;            // Clear timer register
            PR1 = ((FCY / ALL_TIMER_PRESCALER) / Freq);  
            break;
            
        case TIMER_2:
            T2CONbits.TON = 0;      // Stop Timer
            T2CONbits.TCS = 0;      // Select internal instruction cycle clock
            T2CONbits.TGATE = 0;    // Disable Gated Timer mode
            T2CONbits.TCKPS = 1;    // Select 1:8 Prescaler
            TMR2 = 0x00;            // Clear timer register
            PR2 = ((FCY / ALL_TIMER_PRESCALER) / Freq); 
            break;
            
        case TIMER_3:
            T3CONbits.TON = 0;      // Stop Timer
            T3CONbits.TCS = 0;      // Select internal instruction cycle clock
            T3CONbits.TGATE = 0;    // Disable Gated Timer mode
            T3CONbits.TCKPS = 1;    // Select 1:8 Prescaler
            TMR3 = 0x00;            // Clear timer register
            PR3 = ((FCY / ALL_TIMER_PRESCALER) / Freq);
            break;
            
        case TIMER_4:
            T4CONbits.TON = 0;      // Stop Timer
            T4CONbits.TCS = 0;      // Select internal instruction cycle clock
            T4CONbits.TGATE = 0;    // Disable Gated Timer mode
            T4CONbits.TCKPS = 1;    // Select 1:8 Prescaler
            TMR4 = 0x00;            // Clear timer register
            PR4 = ((FCY / ALL_TIMER_PRESCALER) / Freq);
            break;
            
        case TIMER_5:
            T5CONbits.TON = 0;      // Stop Timer
            T5CONbits.TCS = 0;      // Select internal instruction cycle clock
            T5CONbits.TGATE = 0;    // Disable Gated Timer mode
            T5CONbits.TCKPS = 1;    // Select 1:8 Prescaler
            TMR5 = 0x00;            // Clear timer register
            PR5 = ((FCY / ALL_TIMER_PRESCALER) / Freq);            
            break;  
            
        // PID FS timer
        case TIMER_6:
            T6CONbits.TON = 0;      // Stop Timer
            T6CONbits.TCS = 0;      // Select internal instruction cycle clock
            T6CONbits.TGATE = 0;    // Disable Gated Timer mode
            T6CONbits.TCKPS = 3;    // Select 1:256 Prescaler
            TMR6 = 0x00;            // Clear timer register
            PR6 = ((FCY / 256) / Freq);
            break;
            
        case TIMER_7:
            T7CONbits.TON = 0;      // Stop Timer
            T7CONbits.TCS = 0;      // Select internal instruction cycle clock
            T7CONbits.TGATE = 0;    // Disable Gated Timer mode
            T7CONbits.TCKPS = 3;    // Select 1:256 Prescaler
            TMR7 = 0x00;            // Clear timer register
            PR7 = ((FCY / 256) / Freq); 
            break;
            
        case TIMER_8:
            T8CONbits.TON = 0;      // Stop Timer
            T8CONbits.TCS = 0;      // Select internal instruction cycle clock
            T8CONbits.TGATE = 0;    // Disable Gated Timer mode
            T8CONbits.TCKPS = 3;    // Select 1:256 Prescaler
            TMR8 = 0x00;            // Clear timer register
            PR8 = ((FCY / 256) / Freq);           
            break;
            
            // HEARTBEAT TIMER, PRESCALER DIFFERS (256)
        case TIMER_9:
            T9CONbits.TON = 0;      // Stop Timer
            T9CONbits.TCS = 0;      // Select internal instruction cycle clock
            T9CONbits.TGATE = 0;    // Disable Gated Timer mode
            T9CONbits.TCKPS = 3;    // Select 1:256 Prescaler
            TMR9 = 0x00;            // Clear timer register
            PR9 = ((FCY / 256) / Freq);               
            break;              
    }
}

//****void TIMER_update_freq (unsigned char timer, unsigned long new_freq)****//
//Description : Function updates timer module to new frequency
//
//Function prototype : void TIMER_update_freq (unsigned char timer, unsigned long new_freq)
//
//Enter params       : unsigned char timer : TIMER_x module
//                     unsigned long new_freq  : Timer frequency
//
//Exit params        : None
//
//Function call      : TIMER_init(TIMER_1, 150);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
void TIMER_update_freq (unsigned char timer, unsigned long new_freq)
{
    switch (timer)
    {
        case TIMER_1:
            TIMER_stop(TIMER_1);
            TMR1 = 0;
            PR1 = ((FCY / ALL_TIMER_PRESCALER) / new_freq);    
            TIMER_start(TIMER_1);
            break;
            
        case TIMER_2:
            TIMER_stop(TIMER_2);
            TMR2 = 0;
            PR2 = ((FCY / ALL_TIMER_PRESCALER) / new_freq);
            TIMER_start(TIMER_2);
            break;
            
        case TIMER_3:
            TIMER_stop(TIMER_3);
            TMR3 = 0;
            PR3 = ((FCY / ALL_TIMER_PRESCALER) / new_freq); 
            TIMER_start(TIMER_3);
            break;
            
        case TIMER_4:
            TIMER_stop(TIMER_4);
            TMR4 = 0;
            PR4 = ((FCY / ALL_TIMER_PRESCALER) / new_freq);
            TIMER_start(TIMER_4);
            break;
            
        case TIMER_5:
            TIMER_stop(TIMER_5);
            TMR5 = 0;
            PR5 = ((FCY / ALL_TIMER_PRESCALER) / new_freq);
            TIMER_start(TIMER_5);
            break;
            
        case TIMER_6:
            TIMER_stop(TIMER_6);
            TMR6 = 0;
            PR6 = ((FCY / 256) / new_freq);
            TIMER_start(TIMER_6);
            break;
            
        case TIMER_7:
            TIMER_stop(TIMER_7);
            TMR7 = 0;
            PR7 = ((FCY / 256) / new_freq);
            TIMER_start(TIMER_7);
            break;
            
        case TIMER_8:
            TIMER_stop(TIMER_8);
            TMR8 = 0;
            PR8 = ((FCY / 256) / new_freq);
            TIMER_start(TIMER_8);
            break;
            
        case TIMER_9:
            TIMER_stop(TIMER_9);
            TMR9 = 0;
            PR9 = ((FCY / 256) / new_freq);
            TIMER_start(TIMER_9);
            break;            
    }
}

//*******************void TIMER_start (unsigned char timer)*******************//
//Description : Function starts timer module
//
//Function prototype : void TIMER_start (unsigned char timer)
//
//Enter params       : unsigned char timer : TIMER_x module
//
//Exit params        : None
//
//Function call      : TIMER_start(TIMER_1);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
void TIMER_start (unsigned char timer)
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
            IEC3bits.T9IE = 1;      // Enable timer interrupt
            IFS3bits.T9IF = 0;      // Clear timer flag    
            T9CONbits.TON = 1;      // Start timer
            break;            
    }    
    TIMER_struct[timer].run_state = 1; // Timer is started
}

//********************void TIMER_stop (unsigned char timer)*******************//
//Description : Function stops timer module
//
//Function prototype : void TIMER_stop (unsigned char timer)
//
//Enter params       : unsigned char timer : TIMER_x module
//
//Exit params        : None
//
//Function call      : TIMER_stop(TIMER_1);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
void TIMER_stop (unsigned char timer)
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

//*************unsigned char TIMER_get_state (unsigned char timer)************//
//Description : Function returns timer module state
//
//Function prototype : unsigned char TIMER_get_state (unsigned char timer)
//
//Enter params       : unsigned char timer : TIMER_x module
//
//Exit params        : unsigned char : state (0 stopped, 1 active)
//
//Function call      : unsigned char = TIMER_get_state(TIMER_1);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
unsigned char TIMER_get_state (unsigned char timer, unsigned char type)
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
    __builtin_btg(&LATC, 2);
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