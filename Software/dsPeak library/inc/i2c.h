//***************************************************************************//
// File      : I2C.h
//
// Functions :  void I2C_init (unsigned char port, unsigned char mode, unsigned char address);
//              void I2C_fill_transmit_buffer (unsigned char port, unsigned char *ptr, unsigned char length);
//              void I2C_master_write (unsigned char port, unsigned char *data, unsigned char length);
//              void I2C_master_read (unsigned char port, unsigned char *data, unsigned char w_length, unsigned char r_length);
//              unsigned char I2C_wait (unsigned char port);
//              unsigned char I2C_read_state (unsigned char port);
//              unsigned char * I2C_get_rx_buffer (unsigned char port);
//              unsigned char I2C_rx_done (unsigned char port);
//              void I2C_change_address (unsigned char adr);
//              void I2C_clear_rx_buffer (unsigned char port);
//
// Includes  : general.h
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#ifndef __I2C_H__
#define	__I2C_H__

#include "general.h"

#define I2C_port_1 0
#define I2C_port_2 1

#define I2C_mode_master 0
#define I2C_mode_slave 1
#define EEPROM_TX_LENGTH 32
#define EEPROM_RX_LENGTH 32
#define I2C_MSG_LENGTH 5
#define I2C_FREQ_100k 660        /* define i2c frequency (100khz) */
#define I2C_FREQ_400k 156
#define I2C_WRITE 0
#define I2C_READ 1

#define I2C_MODULE_QTY 2

void I2C_init (unsigned char port, unsigned char mode, unsigned char address);
void I2C_fill_transmit_buffer (unsigned char port, unsigned char *ptr, unsigned char length);
void I2C_master_write (unsigned char port, unsigned char *data, unsigned char length);
void I2C_master_read (unsigned char port, unsigned char *data, unsigned char w_length, unsigned char r_length);
unsigned char I2C_wait (unsigned char port);
unsigned char I2C_read_state (unsigned char port);
unsigned char * I2C_get_rx_buffer (unsigned char port);
unsigned char I2C_rx_done (unsigned char port);
void I2C_change_address (unsigned char adr);
void I2C_clear_rx_buffer (unsigned char port);

typedef struct
{   
    unsigned char i2c_tx_counter;
    unsigned char i2c_rx_counter;
    unsigned char i2c_tx_data[EEPROM_TX_LENGTH];
    unsigned char i2c_rx_data[EEPROM_RX_LENGTH];
    unsigned char i2c_int_counter;
    unsigned char i2c_write_length;
    unsigned char i2c_read_length;
    unsigned char i2c_done;
    unsigned char i2c_message_mode;
}STRUCT_I2C;
#endif	/* I2C_H */