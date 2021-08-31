//****************************************************************************//
// File      :  UART.h
//
// Functions :  void UART_init (uint8_t channel, uint32_t baud, uint8_t buf_length);
//              void UART_putc (uint8_t channel, uint8_t data);
//              void UART_putstr (uint8_t channel, const char *str);
//              void UART_putbuf (uint8_t channel, uint8_t *buf, uint8_t length);
//              void UART_putbuf_dma (uint8_t channel, uint8_t *buf, uint8_t length);
//              void UART_putc_ascii (uint8_t channel, uint8_t data);
//              uint8_t UART_rx_done (uint8_t channel);
//              uint8_t UART_tx_done (uint8_t channel);
//              uint8_t * UART_get_rx_buffer (uint8_t channel);
//              void UART_clear_rx_buffer (uint8_t channel);
//              void UART_send_tx_buffer (uint8_t channel);
//              void UART_fill_tx_buffer (uint8_t channel, uint8_t *data, uint8_t length);
//
// Includes  :  dspeak_generic.h
//
// Purpose   :  Driver for the dsPIC33EP UART core
//              3x seperate UART channels on dsPeak :
//              UART_1 : RS-485 bus interface
//              UART_2 : MikroBus 1 and 2 interface
//              UART_3 : USB-UART debug interface
//              
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#ifndef __UART_H__
 #define __UART_H__

#include "dspeak_generic.h"

#define UART_MAX_TX 128
#define UART_MAX_RX 128 

#define UART_1 0
#define UART_2 1
#define UART_3 2

#define UART_QTY 3

#define UART_TX_COMPLETE    1
#define UART_TX_IDLE        0

#define UART_RX_COMPLETE    1
#define UART_RX_IDLE        0  

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
void UART_putstr (uint8_t channel, const char *str);
void UART_putbuf (uint8_t channel, uint8_t *buf, uint8_t length);
void UART_putbuf_dma (uint8_t channel, uint8_t *buf, uint8_t length);
void UART_putc_ascii (uint8_t channel, uint8_t data);
uint8_t UART_rx_done (uint8_t channel);
uint8_t UART_tx_done (uint8_t channel);
uint8_t * UART_get_rx_buffer (uint8_t channel);
void UART_clear_rx_buffer (uint8_t channel);
uint8_t UART_get_rx_buffer_length (uint8_t channel);
void UART_send_tx_buffer (uint8_t channel);
void UART_fill_tx_buffer (uint8_t channel, uint8_t *data, uint8_t length);

#endif