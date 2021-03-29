//****************************************************************************//
// File      :  UART.h
//
// Functions :  
//
// Includes  :  general.h
//
// Purpose   :  Driver for the dsPIC33EP UART core
//              
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
#ifndef __UART_H__
 #define __UART_H__

#include "general.h"

#define UART_MAX_TX 64
#define UART_MAX_RX 64 

#define UART_1 0
#define UART_2 1
#define UART_3 2

#define UART_QTY 3

typedef struct
{
    uint8_t UART_operation;
    uint8_t UART_tx_length;
    uint8_t UART_tx_data[UART_MAX_TX];
    uint8_t UART_rx_length;
    uint8_t UART_rx_data[UART_MAX_RX];
    uint8_t UART_rx_done;
    uint8_t UART_tx_done;
    uint8_t UART_rx_counter;
    uint8_t UART_tx_counter;
}STRUCT_UART;

void UART_init (uint8_t channel, uint32_t baud, uint8_t buf_length);
void UART_putc (uint8_t channel, uint8_t data);
void UART_putstr (uint8_t channel, char *str);
void UART_putbuf (uint8_t channel, uint8_t *buf, uint8_t length);
void UART_putbuf_dma (uint8_t channel, uint8_t *buf, uint8_t length);
void UART_putc_ascii (uint8_t channel, uint8_t data);

void UART_rx_interrupt (uint8_t channel);
void UART_tx_interrupt (uint8_t channel);

uint8_t UART_rx_done (uint8_t channel);
uint8_t UART_tx_done (uint8_t channel);
uint8_t * UART_get_rx_buffer (uint8_t channel);
void UART_clear_rx_buffer (uint8_t channel);
void UART_send_tx_buffer (uint8_t channel);
void UART_fill_tx_buffer (uint8_t channel, uint8_t *data, uint8_t length);



#endif