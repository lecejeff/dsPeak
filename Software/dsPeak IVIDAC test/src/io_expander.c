//****************************************************************************//
// File      : io_expander.c
//
// Functions :  
//
// Includes  : io_expander.h
// Jean-Francois Bilodeau    MPLab X v5.00    10/09/2018 
//****************************************************************************//
#include "io_expander.h"

void EXPANDER_init (STRUCT_EXPANDER *exp, uint8_t address, uint16_t output_reset_value)
{   
    exp->i2c_address = address;
    exp->exp_out_all = output_reset_value;
    
    EXPANDER_i2c_write(exp, exp->exp_out_all);  // Set default output to 0
}

// PCF8575, Address + P7..0 + +15..8 byte ordering
void EXPANDER_i2c_write (STRUCT_EXPANDER *exp, uint16_t value)
{
    uint8_t write_buffer[3];
    write_buffer[0] = exp->i2c_address;             // Set device adress    
    write_buffer[1] = value;                        // LSB word contains MSB byte for IO expander
    write_buffer[2] = ((value & 0xFF00)>>8);        // MSB word contains LSB byte for IO expander    
    I2C_master_write(I2C_port_1, write_buffer, 3); 
}

void EXPANDER_write_bit (STRUCT_EXPANDER *exp, uint16_t bit_pos, uint8_t value)
{
    if (bit_pos <= 16)
    {        
        if (value==1){exp->exp_out_all |= (1 << bit_pos);}
        else {exp->exp_out_all &= ~(1 << bit_pos);}
        EXPANDER_i2c_write(exp, exp->exp_out_all);
    }
}

void EXPANDER_write_port (STRUCT_EXPANDER *exp, uint8_t port, uint8_t value)
{
    if (port == LOW_PORT)
    {
        exp->exp_out_all = ((exp->exp_out_all & 0x00FF) | (value << 8));
    }
    else
    {
        exp->exp_out_all = ((exp->exp_out_all & 0xFF00) | value);
    }
    EXPANDER_i2c_write(exp, exp->exp_out_all);  
}

void EXPANDER_write_all (STRUCT_EXPANDER *exp, uint16_t value)
{
    exp->exp_out_all = value;
    EXPANDER_i2c_write(exp, exp->exp_out_all);
}

uint8_t EXPANDER_get_output_single (STRUCT_EXPANDER *exp, uint16_t bit_pos)
{
    return ((exp->exp_out_all>>bit_pos) & 0x01);
}

uint8_t EXPANDER_get_output_port (STRUCT_EXPANDER *exp, uint8_t port)
{
    if (port == LOW_PORT)
    {
        return ((exp->exp_out_all & 0xFF00)>>8);
    }
    else
    {
        return (exp->exp_out_all & 0x00FF);
    }    
}

uint16_t EXPANDER_get_output_all (STRUCT_EXPANDER *exp)
{
    return exp->exp_out_all;
}