//****************************************************************************//
// File      :  UART.h
//
// Functions :  void UART_init (unsigned char channel, unsigned long baud);
//              void UART_putc (unsigned char channel, unsigned char data);
//              void UART_putstr (unsigned char channel, char *str);
//              void UART_rx_interrupt (unsigned char channel);
//              unsigned char UART_rx_done (unsigned char channel);
//              void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL);
//              void UART_putc_ascii (unsigned char channel, unsigned char data);
//              void UART_putstr_ascii (unsigned char channel, char *str);
//              unsigned char * UART_get_rx_buffer (unsigned char channel);
//              void UART_respond_to_request (unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char channel);
//              void UART_clear_rx_buffer (unsigned char channel);
//              void UART_send_tx_buffer (unsigned char channel);
//              void UART_fill_transmit_buffer (unsigned char channel, unsigned char *data, unsigned char length);
//
// Includes  :  general.h
//
// Purpose   :  Driver for the dsPIC33EP UART core
//              
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
#ifndef __UART_H__
 #define __UART_H__

#include "general.h"

#define UART_ERROR 0xFF
#define UART_MAX_TX 256
#define UART_MAX_RX 256 

#define UART_CHECKSUM_INDEX (UART_MAX_RX - 1)
#define UART_1 0
#define UART_2 1
#define UART_3 2

#define UART_QTY 3

void UART_init (unsigned char channel, unsigned long baud, unsigned char buf_length);
void UART_putc (unsigned char channel, unsigned char data);
void UART_putstr (unsigned char channel, char *str);
void UART_rx_interrupt (unsigned char channel);
unsigned char UART_rx_done (unsigned char channel);
void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL);
void UART_putc_ascii (unsigned char channel, unsigned char data);
void UART_putstr_ascii (unsigned char channel, char *str);
unsigned char * UART_get_rx_buffer (unsigned char channel);
void UART_respond_to_request (unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char channel);
void UART_clear_rx_buffer (unsigned char channel);
void UART_send_tx_buffer (unsigned char channel);
void UART_fill_transmit_buffer (unsigned char channel, unsigned char *data, unsigned char length);

typedef struct
 {
    unsigned char UART_operation;
    unsigned char UART_tx_data[UART_MAX_TX];
    unsigned char UART_rx_length;
    unsigned char UART_rx_data[UART_MAX_RX];
    unsigned char UART_rx_done;
    unsigned char data_counter;
 }STRUCT_UART;
 
 
#endif //USART.h 