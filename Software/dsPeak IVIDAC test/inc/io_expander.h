//****************************************************************************//
// File      : io_expander.h
//
// Functions :  
//
// Includes  : i2c.h
// Jean-Francois Bilodeau    MPLab X v5.00    10/09/2018 
//****************************************************************************//
#ifndef __io_expander_h__
#define __io_expander_h__
#include "i2c.h"

// Selected IO expander is PCF8575, 16-bit wide I2C expander

#define LOW_PORT    0   // Low port is bits [7..0] out of [15..0]
#define HIGH_PORT   1   // High port is bits [15..8] out of [15..0]

typedef struct
{
    uint8_t b0 : 1;
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
    uint8_t b8 : 1;
    uint8_t b9 : 1;
    uint8_t b10 : 1;
    uint8_t b11 : 1;
    uint8_t b12 : 1;
    uint8_t b13 : 1;
    uint8_t b14 : 1;
    uint8_t b15 : 1;    
}STRUCT_EXPANDER_PORT;

typedef union
{
    STRUCT_EXPANDER_PORT expander_bit;
    uint16_t expander_word;
}UNION_EXPANDER;

typedef struct
{
    UNION_EXPANDER expander;
    uint8_t i2c_address;
    uint16_t exp_out_all;
}STRUCT_EXPANDER;

// High-level transactional functions with the IO expander
void EXPANDER_init (STRUCT_EXPANDER *exp, uint8_t address, uint16_t output_reset_value);
void EXPANDER_write_bit (STRUCT_EXPANDER *exp, uint16_t bit_pos, uint8_t value);
void EXPANDER_write_port (STRUCT_EXPANDER *exp, uint8_t port, uint8_t value);
void EXPANDER_write_all (STRUCT_EXPANDER *exp, uint16_t value);
uint8_t EXPANDER_get_output_single (STRUCT_EXPANDER *exp, uint16_t bit_pos);
uint8_t EXPANDER_get_output_port (STRUCT_EXPANDER *exp, uint8_t port);
uint16_t EXPANDER_get_output_all (STRUCT_EXPANDER *exp);

// Low-level I2C function
void EXPANDER_i2c_write (STRUCT_EXPANDER *exp, uint16_t value);
#endif