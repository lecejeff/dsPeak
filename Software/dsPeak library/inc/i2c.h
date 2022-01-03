//***************************************************************************//
// File      : I2C.h
//
// Functions :  void I2C_init (uint8_t port, uint8_t mode, uint8_t address);
//              void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length);
//              void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length);
//              void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length);
//              uint8_t I2C_wait (uint8_t port);
//              uint8_t I2C_read_state (uint8_t port);
//              uint8_t * I2C_get_rx_buffer (uint8_t port);
//              uint8_t I2C_rx_done (uint8_t port);
//              void I2C_change_address (uint8_t adr);
//              void I2C_clear_rx_buffer (uint8_t port);
//
// Includes  : dspeak_generic.h
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#ifndef __I2C_H__
#define	__I2C_H__

#include "dspeak_generic.h"

#define I2C_port_1 0
#define I2C_port_2 1
#define I2C_MODULE_QTY 2

#define PULSE_GOBBLER_DELAY 

#define I2C_FREQ_100k 690   
#define I2C_FREQ_400k 165

#define I2C_mode_master 0
#define I2C_mode_slave 1
#define EEPROM_TX_LENGTH 32
#define EEPROM_RX_LENGTH 32
#define I2C_MSG_LENGTH 5

#define I2C_WRITE 0
#define I2C_READ 1

typedef struct
{   
    uint8_t i2c_tx_counter;
    uint8_t i2c_rx_counter;
    uint8_t i2c_tx_data[EEPROM_TX_LENGTH];
    uint8_t i2c_rx_data[EEPROM_RX_LENGTH];
    uint8_t i2c_int_counter;
    uint8_t i2c_write_length;
    uint8_t i2c_read_length;
    uint8_t i2c_done;
    uint8_t i2c_message_mode;
}STRUCT_I2C;

void I2C_init (uint8_t port, uint8_t mode, uint8_t address);
void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length);
void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length);
void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length);
uint8_t I2C_wait (uint8_t port);
uint8_t I2C_read_state (uint8_t port);
uint8_t * I2C_get_rx_buffer (uint8_t port);
uint8_t I2C_rx_done (uint8_t port);
uint8_t I2C_get_ack_state (uint8_t port);
void I2C_change_address (uint8_t adr);
void I2C_clear_rx_buffer (uint8_t port);


#endif	/* I2C_H */