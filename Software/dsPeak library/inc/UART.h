//****************************************************************************//
// File      :  UART.h
//
// Includes  :  dspeak_generic.h
//
// Purpose   :  Driver for the dsPIC33EP UART core
//              3x seperate UART channels on dsPeak :
//              UART_1 : RS-485 bus interface
//              UART_2 : MikroBus 1 and 2 interface
//              UART_3 : USB-UART debug interface
//              
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#ifndef __UART_H__
 #define __UART_H__

#include "dspeak_generic.h"

#define UART_MAX_TX 255
#define UART_MAX_RX 255 

#define UART_1 0
#define UART_2 1
#define UART_3 2
#define UART_4 3

#define UART_QTY 4

#define UART_TX_COMPLETE    1
#define UART_TX_IDLE        0

#define UART_RX_COMPLETE    1
#define UART_RX_IDLE        0  

#define UART1_DMA_ENABLE
#define UART2_DMA_ENABLE
#define UART3_DMA_ENABLE
//#define UART4_DMA_ENABLE


typedef struct
{
    uint8_t UART_operation;
    uint8_t DMA_tx_channel;
    uint8_t DMA_rx_channel;
    uint8_t UART_channel;
    uint8_t tx_length;
    uint8_t tx_buf_length;
    uint8_t tx_buf[UART_MAX_TX];
    uint8_t rx_buf_length;
    uint8_t rx_buf[UART_MAX_RX];
    uint8_t rx_done;
    uint8_t tx_done;
    uint8_t rx_counter;
    uint8_t tx_counter;
}STRUCT_UART;

void UART_init (STRUCT_UART *uart, uint8_t channel, uint32_t baud, 
                uint8_t tx_buf_length, uint8_t rx_buf_length);
void UART_putc (STRUCT_UART *uart, uint8_t data);
void UART_putstr (STRUCT_UART *uart, const char *string);
uint8_t UART_putstr_dma (STRUCT_UART *uart, const char *string);
void UART_putbuf (STRUCT_UART *uart, uint8_t *buf, uint8_t length);
uint8_t UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint8_t length);
void UART_putc_ascii (STRUCT_UART *uart, uint8_t data);
uint8_t UART_rx_done (STRUCT_UART *uart);
uint8_t UART_tx_done (STRUCT_UART *uart);
uint8_t * UART_get_rx_buffer (STRUCT_UART *uart);
uint8_t UART_get_trmt_state (STRUCT_UART *uart);
void UART_clear_rx_buffer (STRUCT_UART *uart, uint8_t clr_byte);
uint8_t UART_get_rx_buffer_length (STRUCT_UART *uart);
void UART_send_tx_buffer (STRUCT_UART *uart);
void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint8_t length);

#endif