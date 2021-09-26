#include "rot_encoder.h"

STRUCT_ENCODER ENCODER_struct;

void ENCODER_init (uint32_t fs_velocity)
{
    TRISAbits.TRISA14 = 1;  // Set rotary encoder A pulse pin to input
    TRISAbits.TRISA15 = 1;  // Set rotary encoder B pulse pin to input
    TRISAbits.TRISA5 = 1;   // Set rotary encoder SW pin to input
    CNENAbits.CNIEA15 = 1;  // Enable change notice interrupt on encoder B pulse pin  
    
    ENCODER_struct.counter = 0;
    ENCODER_struct.direction = 0;   
    ENCODER_struct.position = 0;
    ENCODER_struct.velocity_rpm = 0;
    ENCODER_struct.velocity_counter = 0;
    ENCODER_struct.fs_velocity = fs_velocity;
    ENCODER_struct.switch_state = 0;
    ENCODER_struct.switch_debouncer = 1;
    ENCODER_struct.count_per_rev = 0;
    ENCODER_struct.channel_change = 0;
    ENCODER_struct.A_state = PORTAbits.RA14;
    ENCODER_struct.B_state = PORTAbits.RA15;
    ENCODER_struct.maximum_cpr = ENCODER_MAX_CPR;
    
    IEC1bits.CNIE = 1;      // Enable change notice interrupt
    IPC4bits.CNIP = 1;      
}

uint8_t ENCODER_get_direction (void)
{
    return ENCODER_struct.direction;
}

uint16_t ENCODER_get_position (void)
{
    return ENCODER_struct.count_per_rev;
}

uint16_t ENCODER_get_velocity (void)
{
    if (ENCODER_struct.velocity_counter > 0)
    {
        ENCODER_struct.velocity_rpm = (uint16_t)((ENCODER_struct.velocity_counter * ENCODER_struct.fs_velocity)/(ENCODER_struct.maximum_cpr));
        ENCODER_struct.velocity_counter = 0;
        return ENCODER_struct.velocity_rpm;
    }
    else
        return 0;    
}

uint8_t ENCODER_get_switch_state (void)
{
    // Negative edge interrupt fired and SW state is still 0
    if (PORTAbits.RA5 == 0)
    {
        if (++ENCODER_struct.switch_debouncer > 5)
        {
            ENCODER_struct.switch_debouncer = 5;
            return 0;
        }
        else
            return 1;
    }
    else
    {
        if (--ENCODER_struct.switch_debouncer < 1)
        {
            ENCODER_struct.switch_state = 1;
            ENCODER_struct.switch_debouncer = 1;
        }
        return 1;
    }      
}

void ENCODER_interrupt_handle (void)
{
    // Backward
    if (ENCODER_struct.channel_change == 1)
    {
        if (PORTAbits.RA14 == 1)
        {
            ENCODER_struct.direction = 1;
        }   

        // Forward
        else 
        {
            ENCODER_struct.direction = 0;
        }  
    }
        
    ENCODER_struct.counter++;
    ENCODER_struct.velocity_counter++;
    ENCODER_struct.count_per_rev++;
    
    if (ENCODER_struct.count_per_rev >= ENCODER_struct.maximum_cpr)
    {
        ENCODER_struct.count_per_rev = 0;
        ENCODER_struct.tour_counter++;
    }         
}

void __attribute__((__interrupt__, no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;   
    if ((PORTAbits.RA15 == 1) && (ENCODER_struct.B_state == 0))
    {
        ENCODER_struct.B_state = 1;
        ENCODER_struct.channel_change = 1;
        ENCODER_interrupt_handle();   // Handle the interrupt 
    }
    else
        ENCODER_struct.B_state = PORTAbits.RA15;
}
