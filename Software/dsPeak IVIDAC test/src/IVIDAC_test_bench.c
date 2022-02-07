#include "IVIDAC_test_bench.h"

void IVIDAC_bench_com_init (STRUCT_IVIDAC_MESSAGE *msg)
{
    uint8_t i = 0;
    
    msg->msg_flag = 0;
    msg->rx_checksum = 0;
    msg->tx_checksum = 0;
    msg->prev_rx_checksum = 0;
    
    for (; i < UART_MAX_RX; i++)
    {
        msg->IVIDAC_message_buf[i] = 0;
    }

}

uint8_t IVIDAC_parse_uart_rx_buffer (STRUCT_IVIDAC_MESSAGE *msg, uint8_t * rx_buf)
{
    uint8_t i = 0;
    for (i=0; i < UART_MAX_RX; i++)
    {
        msg->IVIDAC_message_buf[i] = *rx_buf++;
        if (i < CHECKSUM_OFFSET)  // Checksum is at offset CHECKSUM_OFFSET -> 5
        {
            msg->rx_checksum = msg->rx_checksum + msg->IVIDAC_message_buf[i];
        }
    }
    
    // If calculated checksum = content of the last byte received
    if (msg->rx_checksum == msg->IVIDAC_message_buf[CHECKSUM_OFFSET])
    {   
        msg->prev_rx_checksum = msg->rx_checksum;        
        msg->rx_checksum = 0;  // Clear checksum for next message       
        return 1;
    } 
    
    else 
    {
        msg->prev_rx_checksum = msg->rx_checksum;
        msg->rx_checksum = 0;
        return 0;               // Clear checksum for next message
    }    
}

uint8_t IVIDAC_return_rx_checksum (STRUCT_IVIDAC_MESSAGE *msg)
{
    return msg->prev_rx_checksum;
}

