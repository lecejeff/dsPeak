//****************************************************************************//
// File      :  UART.c
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
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "UART.h"
#include "string.h"
#include "DMA.h"

STRUCT_UART UART_struct[UART_QTY];

// Define UART_x channel DMA buffers (either transmit, receive or both)
__eds__ uint8_t uart1_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
__eds__ uint8_t uart2_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));

#ifdef UART_DEBUG_ENABLE
__eds__ uint8_t uart3_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
#endif

//__eds__ uint16_t uart1_dma_rx_buf[UART_MAX_RX] __attribute__((eds,space(dma)));
//__eds__ uint16_t uart2_dma_rx_buf[UART_MAX_RX] __attribute__((eds,space(dma)));
//__eds__ uint16_t uart3_dma_rx_buf[UART_MAX_RX] __attribute__((eds,space(dma)));

//***void UART_init (uint8_t channel, uint32_t baud, uint8_t rx_buf_length)***//
//Description : Function initialize UART channel at specified baudrate with 
//              specified buffer length, which size cannot exceed 256 bytes
//
//Function prototype : void UART_init (uint8_t channel, uint32_t baud, uint8_t rx_buf_length)
//
//Enter params       : uint8_t channel    : UART_x module
//                   : uint32_t baud       : UART_x baudrate
//                   : uint8_t rx_buf_length : UART_x RX buffer length
//
//Exit params        : None
//
//Function call      : UART_init(UART_1, 9600, 256);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_init (uint8_t channel, uint32_t baud, uint8_t rx_buf_length)
{
    switch (channel)
    {
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
                U1MODE = 0x8A08;                // Reset UART to 8-n-1, alt pins, and enable                      
            }
            else
            {
                U1BRG = (uint16_t)((FCY / (16 * baud))-1);// Set baudrate using normal speed formula
                U1MODE = 0x8A00;                // Reset UART to 8-n-1, alt pins, and enable                
            }
            
            U1STA = 0x0440;                 // Reset status register and enable TX & RX
            IPC16bits.U1EIP = 3;            // Error interrupt priority
            IPC2bits.U1RXIP = 4;            // RX interrupt priority
            IEC4bits.U1EIE = 1;             // Enable error interrupt 
            IEC0bits.U1RXIE = 1;            // Enable receive interrupt  
//            DMA_init(DMA_CH0);
//            DMA0CON = DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD;
//            DMA0REQ = DMAREQ_U3TX;
//            DMA0PAD = (volatile uint16_t)&U3TXREG;
//            DMA0STAH = __builtin_dmapage(uart1_dma_tx_buf);
//            DMA0STAL = __builtin_dmaoffset(uart1_dma_tx_buf);
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

            // UART2 peripheral pin mapping
            RPOR7bits.RP97R = 0x03;         // RF1 (RP97) assigned to UART2_TX
            RPINR19bits.U2RXR = 113;        // RG1 (RP113) assigned to UART2_RX
            
            // MikroBus RS-485 click test --------------------------------------
            // RE pin is MKB1_nCS
            TRISHbits.TRISH15 = 0;  // RE, receiver enable
            TRISDbits.TRISD0 = 0;   // DE, driver enable                       
            LATHbits.LATH15 = 0;    // By default, pull the 485 transceiver in listen mode
            LATDbits.LATD0 = 0;         
            // -----------------------------------------------------------------
            
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
            
            U2STA  = 0x0440;                // Reset status register and enable TX & RX           
            IPC16bits.U2EIP = 3;            // Error interrupt priority
            IPC7bits.U2RXIP = 4;            // RX interrupt priority
            IEC4bits.U2EIE = 1;             // Enable error interrupt
            IEC1bits.U2RXIE = 1;            // Enable receive interrupt
//            DMA_init(DMA_CH0);
//            DMA0CON = DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD;
//            DMA0REQ = DMAREQ_U3TX;
//            DMA0PAD = (volatile uint16_t)&U3TXREG;
//            DMA0STAH = __builtin_dmapage(uart2_dma_tx_buf);
//            DMA0STAL = __builtin_dmaoffset(uart2_dma_tx_buf);            
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
            IEC5bits.U3EIE = 1;             // Enable error interrupt
            IEC5bits.U3RXIE = 1;            // Enable receive interrupt
            
//            DMA_init(DMA_CH0);
//            DMA0CON = DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD;
//            DMA0REQ = DMAREQ_U3TX;
//            DMA0PAD = (volatile uint16_t)&U3TXREG;
//            DMA0STAH = __builtin_dmapage(uart3_dma_tx_buf);
//            DMA0STAL = __builtin_dmaoffset(uart3_dma_tx_buf);
#endif
            break; 
            
        default:
            break;
    }
      
    if (rx_buf_length > UART_MAX_RX)
    {
        UART_struct[channel].UART_rx_length = UART_MAX_RX;
    }
    else
    {
        UART_struct[channel].UART_rx_length = rx_buf_length;
    }
    UART_struct[channel].UART_tx_done = 0;               // TX not done
    UART_struct[channel].UART_rx_done = 0;               // RX not done
    UART_struct[channel].UART_rx_counter = 0;            // Reset rx data counter
    UART_struct[channel].UART_tx_counter = 0;            // Reset tx data counter    
    // According to UART reference manual, users should add a software delay
    // between the UART enable and first transmission based on the baudrate
    __delay_ms(25);
}   

uint8_t UART_get_rx_buffer_length (uint8_t channel)
{
    return UART_struct[channel].UART_rx_length;
}

//**************void UART_putc (uint8_t channel, uint8_t data)****************//
//Description : Function transmits a single character on selected channel via
//              interrupt.
//
//Function prototype : void UART_putc (uint8_t channel, uint8_t data)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t data : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc(UART_1, 'A');
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putc (uint8_t channel, uint8_t data)
{ 
    if (channel == UART_1)
    {
        while(!U1STAbits.TRMT);                     // Wait for TX shift reg to be empty    
        UART_struct[channel].UART_tx_length = 1;    // Set TX length
        UART_struct[channel].UART_tx_data[0] = data;// Fill buffer
        IEC0bits.U1TXIE = 1;                        // Enable interrupt, which sets the TX interrupt flag
    }

    if (channel == UART_2)
    {
        while(!U2STAbits.TRMT);
        UART_struct[channel].UART_tx_length = 1;
        UART_struct[channel].UART_tx_data[0] = data;
        IEC1bits.U2TXIE = 1;        
    }
    
    if (channel == UART_3)
    {
#ifdef UART_DEBUG_ENABLE
        while(!U3STAbits.TRMT);
        UART_struct[channel].UART_tx_length = 1;
        UART_struct[channel].UART_tx_data[0] = data;
        IEC5bits.U3TXIE = 1;   
#endif
    }    
}  

//************void UART_putc_ascii (uint8_t channel, uint8_t data)************//
//Description : Function converts byte to 2 corresponding ascii characters and 
//              send them through UART
//
//Function prototype : void UART_putc_ascii (uint8_t channel, uint8_t data)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t data : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc_ascii(UART_1, 'A');
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putc_ascii (uint8_t channel, uint8_t data)
{
    uint8_t buf[2];
    hex_to_ascii(data, &buf[0], &buf[1]);   // Convert byte to ascii values
    UART_putbuf(channel, buf, 2);           // Send both values through UART
}

//**************void UART_putstr (uint8_t channel, char *str)*****************//
//Description : Function sends a string of character through UART_x channel
//
//Function prototype : void UART_putstr (uint8_t channel, char *str)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t *str : ascii string of character
//
//Exit params        : None
//
//Function call      : UART_putstr(UART_1, "Sending command through UART_x");
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
// Need to add a MAX_TX_OVERFLOW error mechanism to this function
void UART_putstr (uint8_t channel, const char *str)
{
    uint8_t i = 0;
    uint8_t length = strlen(str);
    switch (channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT);                             // Wait for TX shift register to be empty
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *str++;  // Copy data into transmit buffer  
            }               
            UART_struct[channel].UART_tx_length = length;       // Set the transmit length
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;   // Clear the TX done flag
            IEC0bits.U1TXIE = 1;                                // Enable TX interrupt which sets the TX interrupt flag                 
            break;
            
        case UART_2:
            while(!U2STAbits.TRMT);
            // MikroBus RS-485 click test ------------------------------------------
            // Put the transceiver in transmit mode
            LATHbits.LATH15 = 1;    // RE = 1
            LATDbits.LATD0 = 1;     // DE = 1
            //----------------------------------------------------------------------               
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *str++;
            }               
            UART_struct[channel].UART_tx_length = length;
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;         
            IEC1bits.U2TXIE = 1;              
            break;
            
        case UART_3:           
#ifdef UART_DEBUG_ENABLE
            while(!U3STAbits.TRMT);
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *str++;
            }               
            UART_struct[channel].UART_tx_length = length;      
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;           
            IEC5bits.U3TXIE = 1;  
#endif
            break;
            
        default:
            break;
    }
}

//******void UART_putbuf (uint8_t channel, uint8_t *buf, uint8_t length)******//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel
//
//Function prototype : void UART_putbuf (uint8_t channel, uint8_t *buf, uint8_t length)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t *buf : byte buffer
//                   : uint8_t length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_putbuf(UART_1, buf, 256);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putbuf (uint8_t channel, uint8_t *buf, uint8_t length)
{
    uint8_t i = 0;
    switch (channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT);                             // Wait for TX shift register to be empty 
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *buf++;  // Copy data into transmit buffer   
            }               
            UART_struct[channel].UART_tx_length = length;       // Set the transmit length  
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;   // Clear the TX done flag       
            IEC0bits.U1TXIE = 1;                                // Enable TX interrupt which sets the TX interrupt flag                                         
            break;
            
        case UART_2:
            while(!U2STAbits.TRMT);
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *buf++; 
            }               
            UART_struct[channel].UART_tx_length = length;      
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;                 
            IEC1bits.U2TXIE = 1;                                               
            break;
            
        case UART_3:
#ifdef UART_DEBUG_ENABLE
            while(!U3STAbits.TRMT); 
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *buf++;
            }               
            UART_struct[channel].UART_tx_length = length;      
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;            
            IEC5bits.U3TXIE = 1;   
#endif
            break;
            
        default:
            break;
    }
}


//***void UART_putbuf_dma (uint8_t channel, uint8_t *buf, uint8_t length)*****//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel using DMA
//
//Function prototype : void UART_putbuf_dma (uint8_t channel, uint8_t *buf, uint8_t length)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t *buf : byte buffer
//                   : uint8_t length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_putbuf_dma(UART_1, buf, 256);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putbuf_dma (uint8_t channel, uint8_t *buf, uint8_t length)
{
    uint8_t i = 0;
    switch (channel)
    {
        case UART_1:                  
//            if (DMA_get_txfer_state(DMA_CH0) == DMA_TXFER_DONE)  // If DMA channel is free, fill buffer and transmit
//            {
//                if (U1STAbits.TRMT)         
//                {
//                    for (; i < length; i++)
//                    {
//                        uart1_dma_tx_buf[i] = *buf++;
//                    }
//                    DMA0CNT = length - 1;                           // 0 = 1 txfer, so substract 1  
//                    DMA_enable(DMA_CH0);
//                    IEC0bits.U1TXIE = 1;                            // Enable transmission
//                    DMA0REQbits.FORCE = 1;
//                }
//            }
            break;
            
        case UART_2:                  
//            if (DMA_get_txfer_state(DMA_CH0) == DMA_TXFER_DONE)  // If DMA channel is free, fill buffer and transmit
//            {
//                if (U2STAbits.TRMT)         
//                {
//                    for (; i < length; i++)
//                    {
//                        uart2_dma_tx_buf[i] = *buf++;
//                    }
//                    DMA0CNT = length - 1;                           // 0 = 1 txfer, so substract 1  
//                    DMA_enable(DMA_CH0);
//                    IEC1bits.U2TXIE = 1;                            // Enable transmission
//                    DMA0REQbits.FORCE = 1;
//                }
//            }
            break;        
        
        case UART_3:    
#ifdef UART_DEBUG_ENABLE
            if (DMA_get_txfer_state(DMA_CH0) == DMA_TXFER_DONE)  // If DMA channel is free, fill buffer and transmit
            {
                if (U3STAbits.TRMT)         
                {
                    for (; i < length; i++)
                    {
                        uart3_dma_tx_buf[i] = *buf++;
                    }
                    DMA0CNT = length - 1;                           // 0 = 1 txfer, so substract 1  
                    DMA_enable(DMA_CH0);
                    IEC5bits.U3TXIE = 1;                            // Enable transmission
                    DMA0REQbits.FORCE = 1;
                }
            }
#endif
            break;
            
        default:
            break;
    }    
}

//*void UART_fill_tx_buffer (uint8_t channel, uint8_t *data, uint8_t length)**//
//Description : Function fills the TX buffer and set it's length in the struct
//
//Function prototype : void UART_fill_tx_buffer (uint8_t channel, uint8_t *data, uint8_t length)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t *data : buffer
//                   : uint8_t length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_fill_tx_buffer(UART_x, data, length(data));
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_fill_tx_buffer (uint8_t channel, uint8_t *data, uint8_t length)
{
    uint8_t i = 0;
    for (; i<length; i++)
    {
        UART_struct[channel].UART_tx_data[i] = *data++; // Fill the transmit buffer
    }           
    UART_struct[channel].UART_tx_length = length;       // Write TX buffer length
    UART_struct[channel].UART_tx_done = UART_TX_IDLE;   // Set transfer state to IDLE
}

//*****************void UART_send_tx_buffer (uint8_t channel)***************//
//Description : To be used in complement of UART_fill_tx_buffer() to send the 
//              content of the previously filled TX buffer to UART_x channel
//
//Function prototype : void UART_send_tx_buffer (uint8_t channel)
//
//Enter params       : uint8_t channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_send_tx_buffer(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_send_tx_buffer (uint8_t channel)
{
    switch(channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT); // Wait for TX shift reg to be empty   
            IEC0bits.U1TXIE = 1;    // Enable TX interrupt, which set the TX interrupt flag
            break;
            
        case UART_2:
            while(!U2STAbits.TRMT);
            IEC1bits.U2TXIE = 1;
            break;
            
        case UART_3:
#ifdef UART_DEBUG_ENABLE
            while(!U3STAbits.TRMT); 
            IEC5bits.U3TXIE = 1;
#endif
            break;
            
        default:
            break;
    }
}

//**************uint8_t * UART_get_rx_buffer (uint8_t channel)****************//
//Description : Function returns a pointer to the first element of the 
//              receive buffer on UART_x channel
//
//Function prototype : uint8_t * UART_get_rx_buffer (uint8_t channel)
//
//Enter params       : uint8_t channel : UART_x channel
//
//Exit params        : uint8_t * : pointer to receive buffer
//
//Function call      : ptr = UART_get_rx_buffer(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t * UART_get_rx_buffer (uint8_t channel)
{
    switch (channel)
    {
        case UART_1:
            return &UART_struct[channel].UART_rx_data[0];
            break;
            
        case UART_2:
            return &UART_struct[channel].UART_rx_data[0];
            break;
            
        case UART_3:
#ifdef UART_DEBUG_ENABLE
            return &UART_struct[channel].UART_rx_data[0];
#endif
            return 0;
            break;

        default:
            return 0;
        break;
    }
}

//*****************uint8_t UART_rx_done (uint8_t channel)*********************//
//Description : Function checks the UART_struct[channel].UART_rx_done bit.
//              If set, a receive operation was completed, the function clears
//              the bit and returns 1. Otherwise, function returns 0
//
//Function prototype : uint8_t UART_rx_done (uint8_t channel)
//
//Enter params       : uint8_t channel : UART_x channel
//
//Exit params        : uint8_t : 1(done), 0(rx is not done)
//
//Function call      : UART_receive = UART_rx_done(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_rx_done (uint8_t channel)
{
    if ((channel == UART_1) || (channel == UART_2) || (channel == UART_3))
    {
        if (UART_struct[channel].UART_rx_done == UART_RX_COMPLETE)
        {  
            UART_struct[channel].UART_rx_done = UART_RX_IDLE;
            return UART_RX_COMPLETE;
        }
        else return UART_RX_IDLE;        
    }
    else
        return UART_RX_IDLE;
}

//*****************uint8_t UART_tx_done (uint8_t channel)*********************//
//Description : Function checks the UART_struct[channel].UART_tx_done bit.
//              If set, a transmit operation was completed, the function clears
//              the bit and returns 1. Otherwise, function returns 0
//
//Function prototype : uint8_t UART_tx_done (uint8_t channel)
//
//Enter params       : uint8_t channel : UART_x channel
//
//Exit params        : uint8_t : 1(done), 0(tx is not done)
//
//Function call      : UART_receive = UART_tx_done(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_tx_done (uint8_t channel)
{
    if ((channel == UART_1) || (channel == UART_2) || (channel == UART_3))
    {
        if (UART_struct[channel].UART_tx_done == UART_TX_COMPLETE)
        {  
            UART_struct[channel].UART_tx_done = UART_TX_IDLE;
            return UART_TX_COMPLETE;
        }
        else return UART_TX_IDLE;        
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
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;      // clear RX interrupt flag
    if (UART_struct[UART_1].UART_rx_counter < UART_struct[UART_1].UART_rx_length)           // Waiting for more data?
    {
        UART_struct[UART_1].UART_rx_data[UART_struct[UART_1].UART_rx_counter++] = U1RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_1].UART_rx_counter >= UART_struct[UART_1].UART_rx_length)          // All data received?
    {
        UART_struct[UART_1].UART_rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_1].UART_rx_counter = 0;                                            // Clear the RX counter
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
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;                                // clear TX interrupt flag
    if (UART_struct[UART_1].UART_tx_length == 1)        // Single transmission
    {
        U1TXREG = UART_struct[UART_1].UART_tx_data[0]; // Transfer data to UART transmit buffer
        UART_struct[UART_1].UART_tx_done = 1;          // Set the TX done flag
        IEC0bits.U1TXIE = 0;                           // Disable TX interrupt (no more data to send)
    }  

    if (UART_struct[UART_1].UART_tx_length > 1)        // Multiple transmission
    {
        if (UART_struct[UART_1].UART_tx_counter < UART_struct[UART_1].UART_tx_length)           // At least 1 more byte to transfer 
        {
            U1TXREG = UART_struct[UART_1].UART_tx_data[UART_struct[UART_1].UART_tx_counter++];  // Copy TX data to UART TX buffer            
            if (UART_struct[UART_1].UART_tx_counter == UART_struct[UART_1].UART_tx_length)      // More data to send?
            {
                UART_struct[UART_1].UART_tx_done = UART_TX_COMPLETE;                            // Set the TX done flag
                UART_struct[UART_1].UART_tx_counter = 0;                                        // Clear TX counter
                IEC0bits.U1TXIE = 0;                                                            // Disable TX interrupt (no more data to send)
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
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;      // clear RX interrupt flag
    if (UART_struct[UART_2].UART_rx_counter < UART_struct[UART_2].UART_rx_length)           // Waiting for more data?
    {
        UART_struct[UART_2].UART_rx_data[UART_struct[UART_2].UART_rx_counter++] = U2RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_2].UART_rx_counter >= UART_struct[UART_2].UART_rx_length)          // All data received?
    {
        UART_struct[UART_2].UART_rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_2].UART_rx_counter = 0;                                            // Clear the RX counter
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
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _U2TXInterrupt(void)
{
    IFS1bits.U2TXIF = 0;      
    if (UART_struct[UART_2].UART_tx_length == 1)
    {
        U2TXREG = UART_struct[UART_2].UART_tx_data[0];
        UART_struct[UART_2].UART_tx_done = 1;       
        IEC1bits.U2TXIE = 0;                          
    }  

    if (UART_struct[UART_2].UART_tx_length > 1)
    {
        if (UART_struct[UART_2].UART_tx_counter < UART_struct[UART_2].UART_tx_length)
        {
            U2TXREG = UART_struct[UART_2].UART_tx_data[UART_struct[UART_2].UART_tx_counter++];         
            if (UART_struct[UART_2].UART_tx_counter == UART_struct[UART_2].UART_tx_length)
            {
                UART_struct[UART_2].UART_tx_done = 1;     
                UART_struct[UART_2].UART_tx_counter = 0;  
                IEC1bits.U2TXIE = 0; 
                while(!U2STAbits.TRMT);
                // MikroBus RS-485 click test ------------------------------------------
                // Put the transceiver in receive mode
                LATHbits.LATH15 = 0;    // RE = 0
                LATDbits.LATD0 = 0;     // DE = 0
                //----------------------------------------------------------------------                        
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
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _U3RXInterrupt(void)
{
    IFS5bits.U3RXIF = 0;      // clear RX interrupt flag
    if (UART_struct[UART_3].UART_rx_counter < UART_struct[UART_3].UART_rx_length)           // Waiting for more data?
    {
        UART_struct[UART_3].UART_rx_data[UART_struct[UART_3].UART_rx_counter++] = U3RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_3].UART_rx_counter >= UART_struct[UART_3].UART_rx_length)          // All data received?
    {
        UART_struct[UART_3].UART_rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_3].UART_rx_counter = 0;                                            // Clear the RX counter
    }  
}

//**********************UART3 transmit interrupt function*********************//
//Description : UART3 transmit interrupt.
//
//Function prototype : _U3TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _U3TXInterrupt(void)
{
    IFS5bits.U3TXIF = 0;      // clear TX interrupt flag
    if (UART_struct[UART_3].UART_tx_length == 1)
    {
        U3TXREG = UART_struct[UART_3].UART_tx_data[0]; 
        UART_struct[UART_3].UART_tx_done = 1;          
        IEC5bits.U3TXIE = 0;                        
    }  

    if (UART_struct[UART_3].UART_tx_length > 1)
    {
        if (UART_struct[UART_3].UART_tx_counter < UART_struct[UART_3].UART_tx_length)
        {
            U3TXREG = UART_struct[UART_3].UART_tx_data[UART_struct[UART_3].UART_tx_counter++];             
            if (UART_struct[UART_3].UART_tx_counter == UART_struct[UART_3].UART_tx_length)
            {
                UART_struct[UART_3].UART_tx_done = 1;     
                UART_struct[UART_3].UART_tx_counter = 0;   
                IEC5bits.U3TXIE = 0;                    
            }               
        }           
    }   
}
#endif

void __attribute__((__interrupt__, no_auto_psv)) _U1ErrInterrupt(void)
{
    uint8_t temp = 0;   
    // Must clear the overrun error to keep UART receiving
    U1STAbits.OERR = 0; 
    temp = U1RXREG;
    UART_struct[UART_1].UART_rx_counter = 0;
    IFS4bits.U1EIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _U2ErrInterrupt(void)
{
    uint8_t temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U2STAbits.OERR = 0; 
    temp = U2RXREG;
    UART_struct[UART_2].UART_rx_counter = 0;
    IFS4bits.U2EIF = 0;
}

#ifdef UART_DEBUG_ENABLE
void __attribute__((__interrupt__, no_auto_psv)) _U3ErrInterrupt(void)
{
    uint8_t temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U3STAbits.OERR = 0; 
    temp = U3RXREG;
    UART_struct[UART_3].UART_rx_counter = 0;
    IFS5bits.U3EIF = 0;
}
#endif