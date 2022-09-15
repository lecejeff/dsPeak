//****************************************************************************//
// File      :  UART.c
//
// Includes  :  UART.h
//              string.h
//              DMA.h
//
// Purpose   :  Driver for the dsPIC33EP UART peripheral
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
#include "UART.h"
#include "string.h"
#include "DMA.h"

STRUCT_UART UART_struct[UART_QTY];

// Define UART_x channel DMA buffers (either transmit, receive or both)
#ifdef UART1_DMA_ENABLE
__eds__ uint8_t uart1_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
#endif

#ifdef UART2_DMA_ENABLE
__eds__ uint8_t uart2_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
#endif

#ifdef UART3_DMA_ENABLE
#ifdef UART_DEBUG_ENABLE
__eds__ uint8_t uart3_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
#endif
#endif

#ifdef UART4_DMA_ENABLE
__eds__ uint8_t uart4_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
#endif

//***void UART_init (STRUCT_UART *str, uint8_t channel, uint32_t baud, 
//                uint16_t tx_buf_length, uint16_t rx_buf_length)
//Description : Function initialize UART channel at specified baudrate with 
//              specified buffer length, which size cannot exceed 256 bytes
//
//Function prototype : void UART_init (uint8_t channel, uint32_t baud, uint8_t rx_buf_length)
//
//Enter params       : STRUCT_UART *str         : structure pointer type
//                   : uint8_t channel          : UART channel
//                   : uint32_t baud            : UART baudrate
//                   : uint8_t tx_buf_length    : UART transmit buffer length
//                   : uint8_t rx_buf_length    : UART receive buffer length
//
//Exit params        : None
//
//Function call      : UART_init (&UART_x, UART_1, 115200, 32, 32)
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_init (STRUCT_UART *uart, uint8_t channel, uint32_t baud, uint16_t tx_buf_length, 
                uint16_t rx_buf_length, uint8_t DMA_tx_channel)
{
    switch (channel)
    {
        // On dsPeak, UART_1 is physically connected to RS-485 frontend
        case UART_1:                        
            U1MODEbits.UEN = 0;             // Disable UART if it was previously used
            IEC4bits.U1EIE = 0;             // Disable UART error interrupt 
            IEC0bits.U1RXIE = 0;            // Disable UART receive interrupt   
            IEC0bits.U1TXIE = 0;            // Disable UART transmit interupt
            IFS4bits.U1EIF = 0;             // Clear UART error interrupt flag
            IFS0bits.U1RXIF = 0;            // Clear UART receive interrupt flag
            IFS0bits.U1TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART1 input/output pin mapping
            TRISDbits.TRISD15 = 0;          // RD15 configured as an output (UART1_TX)           
            TRISDbits.TRISD14 = 1;          // RD14 configured as an input (UART1_RX)
            CNPUDbits.CNPUD14 = 1;           // Since URXINV = 0, enable pull-up on RX pin
            TRISFbits.TRISF4 = 0;           // RF4 configured as an output (UART1_RTS)
            TRISFbits.TRISF5 = 1;           // RF5 configured as an input (UART1_CTS)
            
            // UART1 peripheral pin mapping
            RPOR4bits.RP79R = 0x01;         // RD15 (RP79) assigned to UART1_TX
            RPINR18bits.U1RXR = 78;         // RD14 (RPI78) assigned to UART1_RX
            RPOR9bits.RP100R = 0x02;        // RF4 (RP100) assigned to UART1_RTS
            RPINR18bits.U1CTSR = 101;       // RF5 (RP101) assigned to UART1_CTS
            
            if (baud > 115200)
            {
                U1BRG = (uint16_t)((FCY / (4 * baud))-1); // Set baudrate using high speed mode formula
                U1MODE = 0x8A08;                // Reset UART to 8-n-1, alt pins, and enable 485 pin control                     
            }
            else
            {
                U1BRG = (uint16_t)((FCY / (16 * baud))-1);// Set baudrate using normal speed formula
                U1MODE = 0x8A00;                // Reset UART to 8-n-1, alt pins, and enable 485 pin control                  
            }
            
            U1STA = 0x0440;                 // Reset status register and enable TX & RX
            IPC16bits.U1EIP = 3;            // Error interrupt priority
            IPC2bits.U1RXIP = 7;            // RX interrupt priority
            IPC3bits.U1TXIP = 7;            // TX interrupt priority
            IEC4bits.U1EIE = 1;             // Enable error interrupt 
            IEC0bits.U1RXIE = 1;            // Enable receive interrupt  

#ifdef UART1_DMA_ENABLE  
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(uart->DMA_tx_channel, DMAREQ_U1TX);
            DMA_set_peripheral_address(uart->DMA_tx_channel, (volatile uint16_t)&U1TXREG);
            DMA_set_buffer_offset_sgl(uart->DMA_tx_channel, __builtin_dmapage(uart1_dma_tx_buf), __builtin_dmaoffset(uart1_dma_tx_buf));
#endif
            break;
           
        case UART_2:
            U2MODEbits.UEN = 0;             // Disable UART if it was previously used
            IEC4bits.U2EIE = 0;             // Disable UART error interrupt 
            IEC1bits.U2RXIE = 0;            // Disable UART receive interrupt   
            IEC1bits.U2TXIE = 0;            // Disable UART transmit interupt
            IFS4bits.U2EIF = 0;             // Clear UART error interrupt flag
            IFS1bits.U2RXIF = 0;            // Clear UART receive interrupt flag
            IFS1bits.U2TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART2 input/output pin mapping
            TRISFbits.TRISF1 = 0;           // RF1 configured as an output (UART2_TX)           
            TRISGbits.TRISG1 = 1;           // RG1 configured as an input (UART2_RX)
            CNPUGbits.CNPUG1 = 1;           // Since URXINV = 0, enable pull-up on RX pin

            // UART2 peripheral pin mapping
            RPOR7bits.RP97R = 0x03;         // RF1 (RP97) assigned to UART2_TX
            RPINR19bits.U2RXR = 113;        // RG1 (RP113) assigned to UART2_RX
            
            if (baud > 115200)
            {
                U2BRG = ((FCY / (4 * baud))-1); // Set baudrate using high speed mode formula
                U2MODE = 0x8008;                // Reset UART to 8-n-1, alt pins, and enable 
            }
            else
            {
                U2BRG = ((FCY / (16 * baud))-1);// Set baudrate using normal speed mode formula
                U2MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable        
            }  

            // MikroBus RS-485 click test --------------------------------------
#ifdef RS485_CLICK_UART2
            TRISHbits.TRISH15 = 0;  // RE, receiver enable
            TRISDbits.TRISD0 = 0;   // DE, driver enable                       
            LATHbits.LATH15 = 0;    // By default, pull the 485 transceiver in listen mode
            LATDbits.LATD0 = 0;              
#endif
            
            // -----------------------------------------------------------------            
            U2STA  = 0x0440;                // Reset status register and enable TX & RX           
            IPC16bits.U2EIP = 3;            // Error interrupt priority
            IPC7bits.U2RXIP = 7;            // RX interrupt priority
            IPC7bits.U2TXIP = 7;
            IEC4bits.U2EIE = 1;             // Enable error interrupt
            IEC1bits.U2RXIE = 1;            // Enable receive interrupt

#ifdef UART2_DMA_ENABLE 
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(uart->DMA_tx_channel, DMAREQ_U2TX);
            DMA_set_peripheral_address(uart->DMA_tx_channel, (volatile uint16_t)&U2TXREG);
            DMA_set_buffer_offset_sgl(uart->DMA_tx_channel, __builtin_dmapage(uart2_dma_tx_buf), __builtin_dmaoffset(uart2_dma_tx_buf));               
#endif
            break;
            
        case UART_3: 
#ifdef UART_DEBUG_ENABLE
            U3MODEbits.UEN = 0;             // Disable UART if it was previously used
            IEC5bits.U3EIE = 0;             // Disable UART error interrupt 
            IEC5bits.U3RXIE = 0;            // Disable UART receive interrupt   
            IEC5bits.U3TXIE = 0;            // Disable UART transmit interupt
            IFS5bits.U3EIF = 0;             // Clear UART error interrupt flag
            IFS5bits.U3RXIF = 0;            // Clear UART receive interrupt flag
            IFS5bits.U3TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART3 input/output pin mapping
            TRISFbits.TRISF3 = 0;           // RF3 configured as an output (UART3_TX)
            TRISBbits.TRISB8 = 1;           // RB8 configured as an input (UART3_RX)
            CNPUBbits.CNPUB8 = 1;           // Since URXINV = 0, enable pull-up on RX pin
            
            // UART3 peripheral pin mapping
            RPOR8bits.RP99R = 0x1B;         // RF3 (RP99) assigned to UART3_TX
            RPINR27bits.U3RXR = 40;         // RB8 (RPI40) assigned to UART3_RX
            
            if (baud > 115200)
            {
                U3BRG = ((FCY / (4 * baud))-1); // Set baudrate using high speed mode formula
                U3MODE = 0x8008;                // Reset UART to 8-n-1, alt pins, and enable                      
            }
            else
            {
                U3BRG = ((FCY / (16 * baud))-1);// Set baudrate using normal speed mode formula
                U3MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable                
            }      
            
            U3STA  = 0x0440;                // Reset status register and enable TX & RX
            IPC20bits.U3EIP = 3;            // Error interrupt priority
            IPC20bits.U3RXIP = 4;           // RX interrupt priority
            IPC20bits.U3TXIP = 7;           // TX interrupt priority
            IEC5bits.U3EIE = 1;             // Enable error interrupt
            IEC5bits.U3RXIE = 1;            // Enable receive interrupt
            
#ifdef UART3_DMA_ENABLE   
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(uart->DMA_tx_channel, DMAREQ_U3TX);
            DMA_set_peripheral_address(uart->DMA_tx_channel, (volatile uint16_t)&U3TXREG);
            DMA_set_buffer_offset_sgl(uart->DMA_tx_channel, __builtin_dmapage(uart3_dma_tx_buf), __builtin_dmaoffset(uart3_dma_tx_buf));        
#endif
#endif
            break; 

        case UART_4: 
            U4MODEbits.UEN = 0;             // Disable UART if it was previously used
            IEC5bits.U4EIE = 0;             // Disable UART error interrupt 
            IEC5bits.U4RXIE = 0;            // Disable UART receive interrupt   
            IEC5bits.U4TXIE = 0;            // Disable UART transmit interupt
            IFS5bits.U4EIF = 0;             // Clear UART error interrupt flag
            IFS5bits.U4RXIF = 0;            // Clear UART receive interrupt flag
            IFS5bits.U4TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART4 input/output pin mapping - not assigned on dsPeak
            
            // UART4 peripheral pin mapping - not assigned on dsPeak
            
            if (baud > 115200)
            {
                U4BRG = ((FCY / (4 * baud))-1); // Set baudrate using high speed mode formula
                U4MODE = 0x8008;                // Reset UART to 8-n-1, alt pins, and enable                      
            }
            else
            {
                U4BRG = ((FCY / (16 * baud))-1);// Set baudrate using normal speed mode formula
                U4MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable                
            }      
            
            U4STA  = 0x0440;                // Reset status register and enable TX & RX
            IPC21bits.U4EIP = 3;            // Error interrupt priority
            IPC22bits.U4RXIP = 4;           // RX interrupt priority
            IPC22bits.U4TXIP = 4;           // TX interrupt priority
            IEC5bits.U4EIE = 1;             // Enable error interrupt
            IEC5bits.U4RXIE = 1;            // Enable receive interrupt
  
#ifdef UART4_DMA_ENABLE
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(uart->DMA_tx_channel, DMAREQ_U4TX);
            DMA_set_peripheral_address(uart->DMA_tx_channel, (volatile uint16_t)&U4TXREG);
            DMA_set_buffer_offset_sgl(uart->DMA_tx_channel, __builtin_dmapage(uart4_dma_tx_buf), __builtin_dmaoffset(uart4_dma_tx_buf));  
#endif
            break; 
            
        default:
            break;
    }
    
    if (rx_buf_length > UART_MAX_RX)
    {
        rx_buf_length = UART_MAX_RX;
    }
    uart->rx_buf_length = rx_buf_length;
    
    if (tx_buf_length > UART_MAX_TX)
    {
        tx_buf_length = UART_MAX_TX;
    }    
    uart->tx_buf_length = tx_buf_length;
    
    uart->tx_done = UART_TX_COMPLETE;// TX not done
    uart->tx_length = 0;             // TX not done
    uart->rx_done = 0;               // RX not done
    uart->rx_counter = 0;            // Reset rx data counter
    uart->tx_counter = 0;            // Reset tx data counter
    uart->UART_channel = channel;    
    // According to UART reference manual, users should add a software delay
    // between the UART enable and first transmission based on the baudrate
    __delay_ms(100);
}   

//************uint8_t UART_get_rx_buffer_length (STRUCT_UART *uart)************//
//Description : Function returns the receive buffer length of the structure
//
//Function prototype : uint8_t UART_get_rx_buffer_length (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : None
//
//Function call      : uint8_t = UART_get_rx_buffer_length(&UART_x);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint16_t UART_get_rx_buffer_length (STRUCT_UART *uart)
{
    return uart->rx_buf_length;
}

uint8_t UART_get_trmt_state (STRUCT_UART *uart)
{
    switch (uart->UART_channel)
    {
        case UART_1:
            return U1STAbits.TRMT;
            break;
            
        case UART_2:
            return U2STAbits.TRMT;
            break;
            
        case UART_3:
            return U3STAbits.TRMT;
            break;
            
        case UART_4:
            return U4STAbits.TRMT;
            break;            
            
        default: 
            return 0;
            break;
    }
}

//*************void UART_putc (STRUCT_UART *uart, uint8_t data)****************//
//Description : Function transmits a single character on selected channel via
//              interrupt.
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty
//
//Function prototype : void UART_putc (STRUCT_UART *uart, uint8_t data)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : uint8_t data         : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc(UART_1, 'A');
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putc (STRUCT_UART *uart, uint8_t data)
{
    switch(uart->UART_channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT);     // Wait for TX shift reg to be empty    
            uart->tx_length = 1;        // Set TX length
            uart->tx_buf[0] = data;     // Fill buffer                
            IEC0bits.U1TXIE = 1;        // Enable interrupt, which sets the TX interrupt flag            
            break;
            
        case UART_2:
            while(!U2STAbits.TRMT);
            uart->tx_length = 1;    // Set TX length
            uart->tx_buf[0] = data;// Fill buffer    
            IEC1bits.U2TXIE = 1;               
            break;
            
        case UART_3:
#ifdef UART_DEBUG_ENABLE
            while(!U3STAbits.TRMT);
            uart->tx_length = 1;    // Set TX length
            uart->tx_buf[0] = data;// Fill buffer    
            IEC5bits.U3TXIE = 1;   
#endif            
            break;
            
        case UART_4:
            while(!U4STAbits.TRMT);
            uart->tx_length = 1;    // Set TX length
            uart->tx_buf[0] = data;// Fill buffer    
            IEC5bits.U4TXIE = 1;               
            break;            
    }  
}  

//***********void UART_putc_ascii (STRUCT_UART *uart, uint8_t data)************//
//Description : Function converts byte to 2 corresponding ascii characters and 
//              send them through UART
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty
//
//Function prototype : void UART_putc_ascii (STRUCT_UART *uart, uint8_t data)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : uint8_t data         : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc_ascii(&UART_x, 'A');
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putc_ascii (STRUCT_UART *uart, uint8_t data)
{
    uint8_t buf[2];
    hex_to_ascii(data, &buf[0], &buf[1]);   // Convert byte to ascii values
    UART_putbuf(uart, buf, 2);               // Send both values through UART
}

//********void UART_putstr (STRUCT_UART *uart, const char *string)*************//
//Description : Function sends a string of character through UART_x channel
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty              
//
//Function prototype : void UART_putstr (STRUCT_UART *uart, const char *string)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : const char *string   : string of character
//
//Exit params        : None
//
//Function call      : UART_putstr(&UART_x, "Sending command through UART_x");
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putstr(STRUCT_UART *uart, const char *string)
{
    uint16_t i = 0;
    uint16_t length = strlen(string);

    // Wait for previous transaction to be completed
    while (UART_tx_done(uart) != UART_TX_COMPLETE);
    
    // Saturate strlen to UART_MAX_TX define
    if (length > uart->tx_buf_length)
    {
        length = uart->tx_buf_length;
    }

    // Fill TX buffer
    for (i=0; i < length; i++)
    {
        uart->tx_buf[i] = *string++;   // Copy data into transmit buffer  
    }               
    uart->tx_length = length;           // Set the transmit length
    uart->tx_done = UART_TX_IDLE;       // Clear the TX done flag
    UART_send_tx_buffer(uart);
}

//********void UART_putstr (STRUCT_UART *uart, const char *string)*************//
//Description : Function sends a string of character through UART_x channel
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty              
//
//Function prototype : void UART_putstr (STRUCT_UART *uart, const char *string)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : const char *string   : string of character
//
//Exit params        : None
//
//Function call      : UART_putstr(&UART_x, "Sending command through UART_x");
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_putstr_dma (STRUCT_UART *uart, const char *string)
{
    uint16_t i = 0;
    uint16_t length = strlen(string);

    // Wait for previous transaction to be completed
    if (DMA_get_txfer_state(uart->DMA_tx_channel) == DMA_TXFER_DONE)    // If DMA channel is free, fill buffer and transmit
    {
        if (UART_get_trmt_state(uart) == 1)            // TRMT empty and ready to accept new data         
        {    
            // Saturate strlen to UART_MAX_TX define
            if (length > uart->tx_buf_length)
            {
                length = uart->tx_buf_length;
            }

            // Fill TX buffer
            for (i=0; i < length; i++)
            {
                if (uart->UART_channel == UART_1)
                {
#ifdef UART1_DMA_ENABLE
                    uart1_dma_tx_buf[i] = *string++; 
#endif
                }
                else if (uart->UART_channel == UART_2)
                {
#ifdef UART2_DMA_ENABLE
                    uart2_dma_tx_buf[i] = *string++; 
#endif
                }
                else if (uart->UART_channel == UART_3)
                {
#ifdef UART3_DMA_ENABLE
                    uart3_dma_tx_buf[i] = *string++;
#endif
                }
                else if (uart->UART_channel == UART_4)
                {
#ifdef UART4_DMA_ENABLE
                    uart4_dma_tx_buf[i] = *string++;
#endif
                }             
                else
                    return 0;
            }  
            DMA_set_txfer_length(uart->DMA_tx_channel, length - 1);     // 0 = 1 txfer, so substract 1 
            DMA_enable(uart->DMA_tx_channel);
            UART_send_tx_buffer(uart);
            DMA_force_txfer(uart->DMA_tx_channel);         
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

//******void UART_putbuf (uint8_t channel, uint8_t *buf, uint16_t length)******//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty   
//
//Function prototype : void UART_putbuf (uint8_t channel, uint8_t *buf, uint16_t length)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t *buf : byte buffer
//                   : uint16_t length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_putbuf(UART_1, buf, 256);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putbuf (STRUCT_UART *uart, uint8_t *buf, uint16_t length)
{
    uint16_t i = 0;
    
    // Wait for previous transaction to have completed
    while (UART_tx_done(uart) != UART_TX_COMPLETE);
    
    // Saturate write length
    if (length > uart->tx_buf_length)
    {
        length = uart->tx_buf_length;
    }
    
    // Fill transmit buffer, set transmit length and TX done flag to idle
    for (i=0; i < length; i++)
    {
        uart->tx_buf[i] = *buf++;    // Copy data into transmit buffer   
    }               
    uart->tx_length = length;        // Set the transmit length  
    uart->tx_done = UART_TX_IDLE;    // Clear the TX done flag  
    UART_send_tx_buffer(uart);
}


//***uint8_t UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint8_t length)*****//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel using DMA
//
//Function prototype : void UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint8_t length)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//                   : uint8_t *buf     : write buffer
//                   : uint8_t length   : write length in bytes
//
//Exit params        : uint8_t
//
//Function call      : UART_putbuf_dma(&UART_x, buf, length(buf));
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint16_t length)
{
    uint16_t i = 0;               
    if (DMA_get_txfer_state(uart->DMA_tx_channel) == DMA_TXFER_DONE)    // If DMA channel is free, fill buffer and transmit
    {
        if (UART_get_trmt_state(uart) == 1)            // TRMT empty and ready to accept new data         
        {
            for (i=0; i < length; i++)
            {
                if (uart->UART_channel == UART_1)
                {
#ifdef UART1_DMA_ENABLE   
                    uart1_dma_tx_buf[i] = *buf++;
#endif
                }

                else if (uart->UART_channel == UART_2)
                {
#ifdef UART2_DMA_ENABLE
                    uart2_dma_tx_buf[i] = *buf++;                  
#endif
                }

                else if (uart->UART_channel == UART_3)
                {
#ifdef UART_DEBUG_ENABLE       
#ifdef UART3_DMA_ENABLE
                    uart3_dma_tx_buf[i] = *buf++;
#endif
#endif
                }

                else if (uart->UART_channel == UART_4)
                {
#ifdef UART4_DMA_ENABLE
                    uart4_dma_tx_buf[i] = *buf++;
#endif
                }                
                
                else
                    return 0;
            }
            DMA_set_txfer_length(uart->DMA_tx_channel, length - 1);     // 0 = 1 txfer, so substract 1 
            DMA_enable(uart->DMA_tx_channel);
            UART_send_tx_buffer(uart);
            DMA_force_txfer(uart->DMA_tx_channel);            
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;       
}

//**void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint8_t length)**//
//Description : Function fills the TX buffer and set it's length in the struct
//
//Function prototype : void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint8_t length)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//                   : uint8_t *data    : write buffer
//                   : uint8_t length   : write length
//
//Exit params        : None
//
//Function call      : UART_fill_tx_buffer(&UART_x, buf, length(buf));
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint16_t length)
{
    uint16_t i = 0;
    
    // Wait for previous transaction to be complete
    while (UART_tx_done(uart) != UART_TX_COMPLETE);
    
    // Saturate write length
    if (length > uart->tx_buf_length)
    {
        length = uart->tx_buf_length;
    }
    
    for (i=0; i < length; i++)
    {
        uart->tx_buf[i] = *data++; // Fill the transmit buffer
    }           
    uart->tx_length = length;       // Write TX buffer length
    uart->tx_done = UART_TX_IDLE;   // Set transfer state to IDLE
}

//*****************void UART_send_tx_buffer (STRUCT_UART *uart)****************//
//Description : Used complementary to UART_fill_tx_buffer
//              Functions sets the str UART channel interrupt flag to start the
//              transmission of the frame written in UART_fill_tx_buffer
//
//Function prototype : void UART_send_tx_buffer (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : None
//
//Function call      : UART_send_tx_buffer(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_send_tx_buffer (STRUCT_UART *uart)
{
    switch(uart->UART_channel)
    {
        case UART_1:
            IEC0bits.U1TXIE = 1;    // Enable TX interrupt, which set the TX interrupt flag
            break;
            
        case UART_2:
#ifdef RS485_CLICK_UART2
            // Put the transceiver in transmit mode           
            LATDbits.LATD0 = 1;     // DE = 1
            LATHbits.LATH15 = 1;    // RE = 1
#endif            
            IEC1bits.U2TXIE = 1;

            break;
            
        case UART_3:
#ifdef UART_DEBUG_ENABLE
            IEC5bits.U3TXIE = 1;
#endif
            break;
            
        case UART_4:
            IEC5bits.U4TXIE = 1;
            break;            
    }
}

//*************uint8_t * UART_get_rx_buffer (STRUCT_UART *str)****************//
//Description : Function returns a pointer to the first element of the 
//              receive buffer of the str
//
//Function prototype : uint8_t * UART_get_rx_buffer (STRUCT_UART *str)
//
//Enter params       : STRUCT_UART *str : structure pointer type
//
//Exit params        : uint8_t
//
//Function call      : uint8_t = UART_get_rx_buffer(&UART_x);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t * UART_get_rx_buffer (STRUCT_UART *uart)
{
    return &uart->rx_buf[0];
}

//******void UART_clear_rx_buffer (STRUCT_UART *uart, uint8_t clr_byte)********//
//Description : Function clears the str rx_buf to clr_byte value            
//
//Function prototype : void UART_clear_rx_buffer (STRUCT_UART *uart, uint8_t clr_byte)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : const char *string   : string of character
//
//Exit params        : None
//
//Function call      : UART_clear_rx_buffer(&UART_x, 0);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_clear_rx_buffer (STRUCT_UART *uart, uint8_t clr_byte)
{
    uint16_t i = 0;
    for (i=0; i < uart->rx_buf_length; i++)
    {
        uart->rx_buf[i] = clr_byte;
    }
    uart->rx_counter = 0;
}

//****************uint8_t UART_rx_done (STRUCT_UART *uart)*********************//
//Description : Function checks the str rx_done flag
//              If rx_done = 1, the reception of the frame is over, returns 1
//              If rx_done = 0, the reception of the frame is still in progress, returns 0
//
//Function prototype : uint8_t UART_rx_done (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : uint8_t
//
//Function call      : uint8_t = UART_rx_done(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_rx_done (STRUCT_UART *uart)
{
    if (uart->rx_done == UART_RX_COMPLETE)
    {
        uart->rx_done = UART_RX_IDLE;
        return UART_RX_COMPLETE;
    }
    else
        return UART_RX_IDLE;
}

//*****************uint8_t UART_tx_done (STRUCT_UART *uart)********************//
//Description : Function checks the str UART channel transmit shift register empty bit TRMT
//              If TRMT = 1 (last transmission completed), returns 1
//              If TMRT = 0 (transmission in progress or queued), return 0
//
//Function prototype : uint8_t UART_tx_done (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : uint8_t
//
//Function call      : uint8_t = UART_tx_done(&UART_x);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_tx_done (STRUCT_UART *uart)
{
    if (uart->tx_done == UART_TX_COMPLETE)
    {
        uart->tx_done = UART_TX_IDLE;
        return UART_TX_COMPLETE;  
    }
    else
        return UART_TX_IDLE;        
}

//**********************UART1 receive interrupt function**********************//
//Description : UART1 receive interrupt.
//
//Function prototype : _U1RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;      // clear RX interrupt flag
    uint8_t temp;
    if (UART_struct[UART_1].rx_counter < UART_struct[UART_1].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_1].rx_buf[UART_struct[UART_1].rx_counter++] = U1RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_1].rx_counter >= UART_struct[UART_1].rx_buf_length)          // All data received?
    {
        // Empty rxreg from any parasitic data
        if (U1STAbits.URXDA)
        {
            while (U1STAbits.URXDA)
            {
                temp = U1RXREG;
            }
        }
        UART_struct[UART_1].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_1].rx_counter = 0;                                            // Clear the RX counter
    }
}

//**********************UART1 transmit interrupt function*********************//
//Description : UART1 transmit interrupt.
//
//Function prototype : _U1TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;                                // clear TX interrupt flag
    if (UART_struct[UART_1].tx_length == 1)        // Single transmission
    {
        U1TXREG = UART_struct[UART_1].tx_buf[0]; // Transfer data to UART transmit buffer
        UART_struct[UART_1].tx_done = UART_TX_COMPLETE;          // Set the TX done flag
        IEC0bits.U1TXIE = 0;                           // Disable TX interrupt (no more data to send)
    }  

    if (UART_struct[UART_1].tx_length > 1)        // Multiple transmission
    {
        if (UART_struct[UART_1].tx_counter < UART_struct[UART_1].tx_length)         // At least 1 more byte to transfer 
        {
            U1TXREG = UART_struct[UART_1].tx_buf[UART_struct[UART_1].tx_counter++]; // Copy TX data to UART TX buffer            
            if (UART_struct[UART_1].tx_counter == UART_struct[UART_1].tx_length)    // More data to send?
            {
                while(!U1STAbits.TRMT);
                UART_struct[UART_1].tx_done = UART_TX_COMPLETE;                     // Set the TX done flag
                UART_struct[UART_1].tx_counter = 0;                                 // Clear TX counter
                IEC0bits.U1TXIE = 0;                                                // Disable TX interrupt (no more data to send)
            }               
        }           
    }   
}

//**********************UART2 receive interrupt function**********************//
//Description : UART2 receive interrupt.
//
//Function prototype : _U2RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;      // clear RX interrupt flag
    uint8_t temp=0;
    if (UART_struct[UART_2].rx_counter < UART_struct[UART_2].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_2].rx_buf[UART_struct[UART_2].rx_counter++] = U2RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_2].rx_counter >= UART_struct[UART_2].rx_buf_length)          // All data received?
    {       
        // Empty rxreg from any parasitic data
        if (U2STAbits.URXDA)
        {
            while (U2STAbits.URXDA)
            {
                temp = U2RXREG;
            }
        }                                                 
        UART_struct[UART_2].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_2].rx_counter = 0;                                            // Clear the RX counter
    }  
}

//**********************UART2 transmit interrupt function*********************//
//Description : UART2 transmit interrupt.
//
//Function prototype : _U2TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U2TXInterrupt(void)
{
    IFS1bits.U2TXIF = 0;      
    if (UART_struct[UART_2].tx_length == 1)
    {
        U2TXREG = UART_struct[UART_2].tx_buf[0];
        UART_struct[UART_2].tx_done = UART_TX_COMPLETE;       
        IEC1bits.U2TXIE = 0;                          
    }  

    if (UART_struct[UART_2].tx_length > 1)
    {
        if (UART_struct[UART_2].tx_counter < UART_struct[UART_2].tx_length)
        {
            U2TXREG = UART_struct[UART_2].tx_buf[UART_struct[UART_2].tx_counter++];         
            if (UART_struct[UART_2].tx_counter == UART_struct[UART_2].tx_length)
            {
#ifdef RS485_CLICK_UART2
                while(!U2STAbits.TRMT);
                // MikroBus RS-485 click test ------------------------------------------
                // Put the transceiver in receive mode
                LATHbits.LATH15 = 0;    // RE = 0
                LATDbits.LATD0 = 0;     // DE = 0
                //----------------------------------------------------------------------  
#endif                
                UART_struct[UART_2].tx_done = UART_TX_COMPLETE;     
                UART_struct[UART_2].tx_counter = 0;  
                IEC1bits.U2TXIE = 0;
            }               
        }           
    } 
}

#ifdef UART_DEBUG_ENABLE
//**********************UART3 receive interrupt function**********************//
//Description : UART3 receive interrupt.
//
//Function prototype : _U3RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U3RXInterrupt(void)
{
    IFS5bits.U3RXIF = 0;      // clear RX interrupt flag
    uint8_t temp=0;
    if (UART_struct[UART_3].rx_counter < UART_struct[UART_3].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_3].rx_buf[UART_struct[UART_3].rx_counter++] = U3RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_3].rx_counter >= UART_struct[UART_3].rx_buf_length)          // All data received?
    {
        // Empty rxreg from any parasitic data
        if (U3STAbits.URXDA)
        {
            while (U3STAbits.URXDA)
            {
                temp = U3RXREG;
            }
        } 
        UART_struct[UART_3].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_3].rx_counter = 0;                                            // Clear the RX counter
    }  
}

//**********************UART4 transmit interrupt function*********************//
//Description : UART4 transmit interrupt.
//
//Function prototype : _U4TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U3TXInterrupt(void)
{
    IFS5bits.U3TXIF = 0;      // clear TX interrupt flag
    if (UART_struct[UART_3].tx_length == 1)
    {
        U3TXREG = UART_struct[UART_3].tx_buf[0]; 
        UART_struct[UART_3].tx_done = UART_TX_COMPLETE;          
        IEC5bits.U3TXIE = 0;                        
    }  

    if (UART_struct[UART_3].tx_length > 1)
    {
        if (UART_struct[UART_3].tx_counter < UART_struct[UART_3].tx_length)
        {
            U3TXREG = UART_struct[UART_3].tx_buf[UART_struct[UART_3].tx_counter++];             
            if (UART_struct[UART_3].tx_counter == UART_struct[UART_3].tx_length)
            {
                UART_struct[UART_3].tx_done = UART_TX_COMPLETE;     
                UART_struct[UART_3].tx_counter = 0;   
                IEC5bits.U3TXIE = 0;                    
            }               
        }           
    }   
}
#endif

//**********************UART4 receive interrupt function**********************//
//Description : UART4 receive interrupt.
//
//Function prototype : _U4RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U4RXInterrupt(void)
{
    IFS5bits.U4RXIF = 0;      // clear RX interrupt flag
    uint8_t temp;
    if (UART_struct[UART_4].rx_counter < UART_struct[UART_4].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_4].rx_buf[UART_struct[UART_4].rx_counter++] = U4RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_4].rx_counter >= UART_struct[UART_4].rx_buf_length)          // All data received?
    {
        // Empty rxreg from any parasitic data
        if (U4STAbits.URXDA)
        {
            while (U4STAbits.URXDA)
            {
                temp = U4RXREG;
            }
        }
        UART_struct[UART_4].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_4].rx_counter = 0;                                            // Clear the RX counter
    }
}

//**********************UART4 transmit interrupt function*********************//
//Description : UART4 transmit interrupt.
//
//Function prototype : _U4TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U4TXInterrupt(void)
{
    IFS5bits.U4TXIF = 0;                                // clear TX interrupt flag
    if (UART_struct[UART_4].tx_length == 1)        // Single transmission
    {
        U4TXREG = UART_struct[UART_4].tx_buf[0]; // Transfer data to UART transmit buffer
        UART_struct[UART_4].tx_done = UART_TX_COMPLETE;          // Set the TX done flag
        IEC5bits.U4TXIE = 0;                           // Disable TX interrupt (no more data to send)
    }  

    if (UART_struct[UART_4].tx_length > 1)        // Multiple transmission
    {
        if (UART_struct[UART_4].tx_counter < UART_struct[UART_4].tx_length)         // At least 1 more byte to transfer 
        {
            U4TXREG = UART_struct[UART_4].tx_buf[UART_struct[UART_4].tx_counter++]; // Copy TX data to UART TX buffer            
            if (UART_struct[UART_4].tx_counter == UART_struct[UART_4].tx_length)    // More data to send?
            {
                while(!U4STAbits.TRMT);
                UART_struct[UART_4].tx_done = UART_TX_COMPLETE;                     // Set the TX done flag
                UART_struct[UART_4].tx_counter = 0;                                 // Clear TX counter
                IEC5bits.U4TXIE = 0;                                                // Disable TX interrupt (no more data to send)
            }               
        }           
    }   
}

void __attribute__((__interrupt__, auto_psv)) _U1ErrInterrupt(void)
{
    uint8_t temp = 0;   
    // Must clear the overrun error to keep UART receiving
    U1STAbits.OERR = 0; 
    temp = U1RXREG;
    UART_struct[UART_1].rx_counter = 0;
    IFS4bits.U1EIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U2ErrInterrupt(void)
{
    uint8_t temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U2STAbits.OERR = 0; 
    temp = U2RXREG;
    UART_struct[UART_2].rx_counter = 0;
    IFS4bits.U2EIF = 0;
}

#ifdef UART_DEBUG_ENABLE
void __attribute__((__interrupt__, auto_psv)) _U3ErrInterrupt(void)
{
    uint8_t temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U3STAbits.OERR = 0; 
    temp = U3RXREG;
    UART_struct[UART_3].rx_counter = 0;
    IFS5bits.U3EIF = 0;
}
#endif

void __attribute__((__interrupt__, auto_psv)) _U4ErrInterrupt(void)
{
    uint8_t temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U4STAbits.OERR = 0; 
    temp = U4RXREG;
    UART_struct[UART_4].rx_counter = 0;
    IFS5bits.U4EIF = 0;
}