//****************************************************************************//
// File      :  UART.c
//
// Functions :  
//
// Includes  :  UART.h
//              string.h
//
// Purpose   :  Driver for the dsPIC33EP UART core
//              
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
#include "UART.h"
#include "string.h"
#include "DMA.h"

STRUCT_UART UART_struct[UART_QTY];

__eds__ unsigned char uart_dma_tx_buf[UART_MAX_TX] __attribute__((eds,space(dma)));
__eds__ unsigned int uart_dma_rx_buf[UART_MAX_RX] __attribute__((eds,space(dma)));

//void UART_init (unsigned char channel, unsigned long baud, unsigned char buf_length)//
//Description : Function initialize UART channel at specified baudrate with 
//              specified buffer length, which size cannot exceed 256 bytes
//
//Function prototype : void UART_init (unsigned char channel, unsigned long baud, unsigned char buf_length)
//
//Enter params       : unsigned char channel    : UART_x module
//                   : unsigned long baud       : UART_x baudrate
//                   : unsigned char buf_length : UART_x RX and TX buffer length
//
//Exit params        : None
//
//Function call      : UART_init(UART_1, 9600, 256);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
void UART_init (unsigned char channel, unsigned long baud, unsigned char rx_buf_length)
{
    switch (channel)
    {
        case UART_1:
            U1MODEbits.UEN = 0;             // Disable UART if it was previously used
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
            
            UART_struct[channel].UART_tx_done = 0;               // TX not done
            UART_struct[channel].UART_rx_done = 0;               // RX not done
            UART_struct[channel].UART_rx_length = rx_buf_length; // Set receive buffer length
            UART_struct[channel].UART_rx_counter = 0;            // Reset rx data counter
            UART_struct[channel].UART_tx_counter = 0;            // Reset tx data counter
            
            U1BRG = ((FCY / (16 * baud))-1);// Set baudrate
            U1MODE = 0x8A00;                // Reset UART to 8-n-1, TX/RX/RTS/CTS used, RTSMD = 1
            U1STA = 0x0440;                 // Reset status register and enable TX & RX
            IPC16bits.U1EIP = 3;            // Error interrupt priority
            IPC2bits.U1RXIP = 4;            // RX interrupt priority
            IFS4bits.U1EIF = 0;             // Clear error interrupt flag
            IFS0bits.U1RXIF = 0;            // Clear receive interrupt flag
            IEC4bits.U1EIE = 1;             // Enable error interrupt 
            IEC0bits.U1RXIE = 1;            // Enable receive interrupt         
            break;
           
        case UART_2:
            U2MODEbits.UEN = 0;             // Disable UART if it was previously used
            // UART2 input/output pin mapping
            TRISFbits.TRISF1 = 0;           // RF1 configured as an output (UART2_TX)           
            TRISGbits.TRISG1 = 1;           // RG1 configured as an input (UART2_RX)
            
            // MikroBus RS-485 click test --------------------------------------
            // RE pin is MKB1_nCS
            TRISHbits.TRISH15 = 0;  // RE, receiver enable
            TRISDbits.TRISD0 = 0;   // DE, driver enable                       
            LATHbits.LATH15 = 0;    // By default, pull the 485 transceiver in listen mode
            LATDbits.LATD0 = 0;         
            // -----------------------------------------------------------------
            
            // UART2 peripheral pin mapping
            RPOR7bits.RP97R = 0x03;         // RF1 (RP97) assigned to UART2_TX
            RPINR19bits.U2RXR = 113;        // RG1 (RP113) assigned to UART2_RX
                   
            UART_struct[channel].UART_tx_done = 0;               // TX not done
            UART_struct[channel].UART_rx_done = 0;               // RX not done
            UART_struct[channel].UART_rx_length = rx_buf_length; // Set receive buffer length
            UART_struct[channel].UART_rx_counter = 0;            // Reset rx data counter
            UART_struct[channel].UART_tx_counter = 0;            // Reset tx data counter
            
            U2BRG = ((FCY / (16 * baud))-1);// Set baudrate
            U2MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable 
            U2STA  = 0x0440;                // Reset status register and enable TX & RX           
            IPC16bits.U2EIP = 3;            // Error interrupt priority
            IPC7bits.U2RXIP = 4;            // RX interrupt priority
            IFS4bits.U2EIF = 0;             // Clear error interrupt flag
            IFS1bits.U2RXIF = 0;            // Clear receive interrupt flag
            IEC4bits.U2EIE = 1;             // Enable error interrupt
            IEC1bits.U2RXIE = 1;            // Enable receive interrupt
            break;
            
        case UART_3: 
            U3MODEbits.UEN = 0;             // Disable UART if it was previously used
            // UART3 input/output pin mapping
            TRISFbits.TRISF3 = 0;           // RF3 configured as an output (UART3_TX)
            TRISBbits.TRISB8 = 1;           // RB8 configured as an input (UART3_RX)
            
            // UART3 peripheral pin mapping
            RPOR8bits.RP99R = 0x1B;         // RF3 (RP99) assigned to UART3_TX
            RPINR27bits.U3RXR = 40;         // RB8 (RPI40) assigned to UART3_RX
            
            UART_struct[channel].UART_tx_done = 0;               // TX not done
            UART_struct[channel].UART_rx_done = 0;               // RX not done
            UART_struct[channel].UART_rx_length = rx_buf_length; // Set receive buffer length
            UART_struct[channel].UART_rx_counter = 0;            // Reset rx data counter
            UART_struct[channel].UART_tx_counter = 0;            // Reset tx data counter
            
            U3BRG = ((FCY / (16 * baud))-1);// Set baudrate
            U3MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable 
            U3STA  = 0x0440;                // Reset status register and enable TX & RX
            IPC20bits.U3EIP = 3;            // Error interrupt priority
            IPC20bits.U3RXIP = 4;           // RX interrupt priority
            IFS5bits.U3EIF = 0;             // Clear error interrupt flag
            IFS5bits.U3RXIF = 0;            // Clear receive interrupt flag
            IEC5bits.U3EIE = 1;             // Enable error interrupt
            IEC5bits.U3RXIE = 1;            // Enable receive interrupt
            
            DMA_init(DMA_CH0);
            DMA0CON = DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD;
            DMA0REQ = DMAREQ_U3TX;
            DMA0PAD = (volatile unsigned int)&U3TXREG;
            DMA0STAH = __builtin_dmapage(uart_dma_tx_buf);
            DMA0STAL = __builtin_dmaoffset(uart_dma_tx_buf);
            break; 
            
        default:
            break;
    }
    
    // According to UART reference manual, users should add a software delay
    // between the UART enable and first transmission based on the baudrate
    __delay_ms(50);
}   

//********void UART_putc (unsigned char channel, unsigned char data)**********//
//Description : Function transmits a single character on selected channel via
//              interrupt.
//
//Function prototype : void UART_putc (unsigned char channel, unsigned char data)
//
//Enter params       : unsigned char channel : UART_x channel
//                   : unsigned char data : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc(UART_1, 'A');
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
void UART_putc (unsigned char channel, unsigned char data)
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
        while(!U3STAbits.TRMT);
        UART_struct[channel].UART_tx_length = 1;
        UART_struct[channel].UART_tx_data[0] = data;
        IEC5bits.U3TXIE = 1;      
    }    
}  

//******void UART_putc_ascii (unsigned char channel, unsigned char data)******//
//Description : Function converts byte to 2 corresponding ascii characters and 
//              send them through UART
//
//Function prototype : void UART_putc_ascii (unsigned char channel, unsigned char data)
//
//Enter params       : unsigned char channel : UART_x channel
//                   : unsigned char data : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc_ascii(UART_1, 'A');
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
void UART_putc_ascii (unsigned char channel, unsigned char data)
{
    unsigned char buf[2];
    hex_to_ascii(data, &buf[0], &buf[1]);   // Convert byte to ascii values
    UART_putbuf(channel, buf, 2);           // Send both values through UART
}

//**************void UART_putstr (unsigned char channel, char *str)***********//
//Description : Function sends a string of character through UART_x channel
//
//Function prototype : void UART_putstr (unsigned char channel, char *str)
//
//Enter params       : unsigned char channel : UART_x channel
//                   : unsigned char *str : ascii string of character
//
//Exit params        : None
//
//Function call      : UART_putstr(UART_1, "Sending command through UART_x");
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************// 
void UART_putstr (unsigned char channel, char *str)
{
    unsigned char i = 0;
    unsigned char length = strlen(str);
    switch (channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT);                             // Wait for TX shift register to be empty
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *str++;  // Copy data into transmit buffer  
            }               
            UART_struct[channel].UART_tx_length = length;       // Set the transmit length
            UART_struct[channel].UART_tx_done = 0;              // Clear the TX done flag
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
            UART_struct[channel].UART_tx_done = 0;         
            IEC1bits.U2TXIE = 1;              
            break;
            
        case UART_3:
            while(!U3STAbits.TRMT);
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *str++;
            }               
            UART_struct[channel].UART_tx_length = length;      
            UART_struct[channel].UART_tx_done = 0;           
            IEC5bits.U3TXIE = 1;                                  
            break;
            
        default:
            break;
    }
}

//void UART_putbuf (unsigned char channel, unsigned char *buf, unsigned char length)//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel
//
//Function prototype : void UART_putbuf (unsigned char channel, unsigned char *buf, unsigned char length)
//
//Enter params       : unsigned char channel : UART_x channel
//                   : unsigned char *buf : byte buffer
//                   : unsigned char length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_putbuf(UART_1, buf, 256);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************// 
void UART_putbuf (unsigned char channel, unsigned char *buf, unsigned char length)
{
    unsigned char i = 0;
    switch (channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT);                             // Wait for TX shift register to be empty 
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *buf++;  // Copy data into transmit buffer   
            }               
            UART_struct[channel].UART_tx_length = length;       // Set the transmit length  
            UART_struct[channel].UART_tx_done = 0;              // Clear the TX done flag       
            IEC0bits.U1TXIE = 1;                                // Enable TX interrupt which sets the TX interrupt flag                                         
            break;
            
        case UART_2:
            while(!U2STAbits.TRMT);
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *buf++; 
            }               
            UART_struct[channel].UART_tx_length = length;      
            UART_struct[channel].UART_tx_done = 0;                 
            IEC1bits.U2TXIE = 1;                                               
            break;
            
        case UART_3:
            while(!U3STAbits.TRMT); 
            for (; i < length; i++)
            {
                UART_struct[channel].UART_tx_data[i] = *buf++;
            }               
            UART_struct[channel].UART_tx_length = length;      
            UART_struct[channel].UART_tx_done = 0;            
            IEC5bits.U3TXIE = 1;   
            break;
            
        default:
            break;
    }
}

// Non-blocking function
// Uses DMA
void UART_putbuf_dma (unsigned char channel, unsigned char *buf, unsigned char length)
{
    unsigned char i = 0;
    switch (channel)
    {           
        case UART_3:                  
            if (DMA_get_txfer_state(DMA_CH0) == DMA_TXFER_DONE)  // If DMA channel is free, fill buffer and transmit
            {
                if (U3STAbits.TRMT)         
                {
                    for (; i < length; i++)
                    {
                        uart_dma_tx_buf[i] = *buf++;
                    }
                    DMA0CNT = length - 1;                           // 0 = 1 txfer, so substract 1  
                    DMA_enable(DMA_CH0);
                    IEC5bits.U3TXIE = 1;                            // Enable transmission
                    DMA0REQbits.FORCE = 1;
                }
            }
            break;
            
        default:
            break;
    }    
}

//void UART_fill_tx_buffer (unsigned char channel, unsigned char *data, unsigned char length)//
//Description : Function fills the TX buffer and set it's length in the struct
//
//Function prototype : void UART_fill_tx_buffer (unsigned char channel, unsigned char *data, unsigned char length)
//
//Enter params       : unsigned char channel : UART_x channel
//                   : unsigned char *data : buffer
//                   : unsigned char length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_fill_tx_buffer(UART_x, data, length(data));
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
void UART_fill_tx_buffer (unsigned char channel, unsigned char *data, unsigned char length)
{
    unsigned char i = 0;
    for (; i<length; i++)
    {
        UART_struct[channel].UART_tx_data[i] = *data++; // Fill the transmit buffer
    }           
    UART_struct[channel].UART_tx_length = length;       // Write TX buffer length
}

//*****************void UART_send_tx_buffer (unsigned char channel)***************//
//Description : To be used in complement of UART_fill_tx_buffer() to send the 
//              content of the previously filled TX buffer to UART_x channel
//
//Function prototype : void UART_send_tx_buffer (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_send_tx_buffer(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
void UART_send_tx_buffer (unsigned char channel)
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
            while(!U3STAbits.TRMT); 
            IEC5bits.U3TXIE = 1;
            break;
            
        default:
            break;
    }
}

//************unsigned char * UART_get_rx_buffer (unsigned char channel)***********//
//Description : Function returns a pointer to the receive buffer on UART_x channel
//
//Function prototype : unsigned char * UART_get_rx_buffer (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : unsigned char * : pointer to receive buffer
//
//Function call      : ptr = UART_get_rx_buffer(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
//****************************************************************************//
unsigned char * UART_get_rx_buffer (unsigned char channel)
{
    switch(channel)
    {
        case UART_1:
            return &UART_struct[channel].UART_rx_data[0];
        break;
    
        case UART_2:
            return &UART_struct[channel].UART_rx_data[0];
        break;
 
        case UART_3:
            return &UART_struct[channel].UART_rx_data[0];
        break;        
        
        default:
            return 0;
        break;
    }
}

//***********unsigned char UART_rx_done (unsigned char channel)***************//
//Description : Function checks the UART_struct[channel].UART_rx_done bit.
//              If set, a receive operation was completed, the function clears
//              the bit and returns 1. Otherwise, function returns 0
//
//Function prototype : unsigned char UART_rx_done (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : unsigned char : 1(done), 0(rx is not done)
//
//Function call      : UART_receive = UART_rx_done(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021   
//****************************************************************************//
unsigned char UART_rx_done (unsigned char channel)
{
    switch(channel)
    {     
        case UART_1:
            if (UART_struct[channel].UART_rx_done)
            {  
                UART_struct[channel].UART_rx_done = 0;
                return 1;
            }
            else return 0;
        break;

        case UART_2:
            if (UART_struct[channel].UART_rx_done)
            {  
                UART_struct[channel].UART_rx_done = 0;
                return 1;
            }
            else return 0;
        break;
 
        case UART_3:
            if (UART_struct[channel].UART_rx_done)
            {  
                UART_struct[channel].UART_rx_done = 0;
                return 1;
            }
            else return 0;
        break;
        
        default:
            return 0;
        break;
    }     
}

//***********unsigned char UART_tx_done (unsigned char channel)***************//
//Description : Function checks the UART_struct[channel].UART_tx_done bit.
//              If set, a transmit operation was completed, the function clears
//              the bit and returns 1. Otherwise, function returns 0
//
//Function prototype : unsigned char UART_tx_done (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : unsigned char : 1(done), 0(tx is not done)
//
//Function call      : UART_receive = UART_tx_done(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021   
//****************************************************************************//
unsigned char UART_tx_done (unsigned char channel)
{
    switch(channel)
    {     
        case UART_1:
            if (UART_struct[channel].UART_tx_done)
            {  
                UART_struct[channel].UART_tx_done = 0;
                return 1;
            }
            else return 0;
        break;

        case UART_2:
            if (UART_struct[channel].UART_tx_done)
            {  
                UART_struct[channel].UART_tx_done = 0;
                return 1;
            }
            else return 0;
        break;
 
        case UART_3:
            if (UART_struct[channel].UART_tx_done)
            {  
                UART_struct[channel].UART_tx_done = 0;
                return 1;
            }
            else return 0;
        break;
        
        default:
            return 0;
        break;
    }     
}

//**************void UART_rx_interrupt (unsigned char channel)****************//
//Description : Function processes uart reception interrupt
//
//Function prototype : void UART_rx_interrupt (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_rx_interrupt(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021 
//****************************************************************************//
void UART_rx_interrupt (unsigned char channel)
{
    switch(channel)   
    {
        case UART_1:
            if (UART_struct[channel].UART_rx_counter < UART_struct[channel].UART_rx_length)         // Is there data yet to be received
            {
                UART_struct[channel].UART_rx_data[UART_struct[channel].UART_rx_counter] = U1RXREG;  // Yes, copy it from the UART RX register
                UART_struct[channel].UART_rx_counter++;                                             // Increment RX counter
            }

            if (UART_struct[channel].UART_rx_counter >= UART_struct[channel].UART_rx_length)        // ALl data received?
            {
                UART_struct[channel].UART_rx_done = 1;                                              // Yes, set the RX done flag
                UART_struct[channel].UART_rx_counter = 0;                                           // Clear the RX counter
            }
        break;
        
        case UART_2:
            if (UART_struct[channel].UART_rx_counter < UART_struct[channel].UART_rx_length)
            {
                UART_struct[channel].UART_rx_data[UART_struct[channel].UART_rx_counter] = U2RXREG;
                UART_struct[channel].UART_rx_counter++;                         
            }

            if (UART_struct[channel].UART_rx_counter >= UART_struct[channel].UART_rx_length)  
            {
                UART_struct[channel].UART_rx_done = 1;              
                UART_struct[channel].UART_rx_counter = 0;
            }
        break;
  
        case UART_3:
            if (UART_struct[channel].UART_rx_counter < UART_struct[channel].UART_rx_length)     
            {
                UART_struct[channel].UART_rx_data[UART_struct[channel].UART_rx_counter] = U3RXREG;
                UART_struct[channel].UART_rx_counter++;                        
            }

            if (UART_struct[channel].UART_rx_counter >= UART_struct[channel].UART_rx_length)    
            {
                UART_struct[channel].UART_rx_done = 1;              
                UART_struct[channel].UART_rx_counter = 0;
            }
        break;
        
        default:
            break;
    }     
}

//**************void UART_tx_interrupt (unsigned char channel)****************//
//Description : Function processes uart transmit interrupt
//
//Function prototype : void UART_tx_interrupt (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_tx_interrupt(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021 
//****************************************************************************//
void UART_tx_interrupt(unsigned char channel)
{
    switch(channel)
    {
        case UART_1:
            if (UART_struct[channel].UART_tx_length == 1)       // Single transmission
            {
                U1TXREG = UART_struct[channel].UART_tx_data[0]; // Transfer data to UART transmit buffer
                UART_struct[channel].UART_tx_done = 1;          // Set the TX done flag
                IEC0bits.U1TXIE = 0;                            // Disable TX interrupt (no more data to send)
            }  

            if (UART_struct[channel].UART_tx_length > 1)        // Multiple transmission
            {
                if (UART_struct[channel].UART_tx_counter < UART_struct[channel].UART_tx_length)         // 
                {
                    U1TXREG = UART_struct[channel].UART_tx_data[UART_struct[channel].UART_tx_counter];  // Copy TX data to UART TX buffer
                    UART_struct[channel].UART_tx_counter++;                                             // Increment TX counter               
                    if (UART_struct[channel].UART_tx_counter == UART_struct[channel].UART_tx_length)    // More data to send?
                    {
                        UART_struct[channel].UART_tx_done = 1;                                          //  Set the TX done flag
                        UART_struct[channel].UART_tx_counter = 0;                                       // Clear TX counter
                        IEC0bits.U1TXIE = 0;                                                            // Disable TX interrupt (no more data to send)
                    }               
                }           
            }             
            break;
            
        case UART_2:
            if (UART_struct[channel].UART_tx_length == 1)
            {
                U2TXREG = UART_struct[channel].UART_tx_data[0];
                UART_struct[channel].UART_tx_done = 1;       
                IEC1bits.U2TXIE = 0;                          
            }  

            if (UART_struct[channel].UART_tx_length > 1)
            {
                if (UART_struct[channel].UART_tx_counter < UART_struct[channel].UART_tx_length)
                {
                    U2TXREG = UART_struct[channel].UART_tx_data[UART_struct[channel].UART_tx_counter];
                    UART_struct[channel].UART_tx_counter++;                
                    if (UART_struct[channel].UART_tx_counter == UART_struct[channel].UART_tx_length)
                    {
                        UART_struct[channel].UART_tx_done = 1;     
                        UART_struct[channel].UART_tx_counter = 0;  
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
            break;
            
        case UART_3:
            if (UART_struct[channel].UART_tx_length == 1)
            {
                U3TXREG = UART_struct[channel].UART_tx_data[0]; 
                UART_struct[channel].UART_tx_done = 1;          
                IEC5bits.U3TXIE = 0;                        
            }  

            if (UART_struct[channel].UART_tx_length > 1)
            {
                if (UART_struct[channel].UART_tx_counter < UART_struct[channel].UART_tx_length)
                {
                    U3TXREG = UART_struct[channel].UART_tx_data[UART_struct[channel].UART_tx_counter];
                    UART_struct[channel].UART_tx_counter++;                
                    if (UART_struct[channel].UART_tx_counter == UART_struct[channel].UART_tx_length)
                    {
                        UART_struct[channel].UART_tx_done = 1;     
                        UART_struct[channel].UART_tx_counter = 0;   
                        IEC5bits.U3TXIE = 0;                    
                    }               
                }           
            }            
            break;
            
        default:
            break;
    }
}


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;      // clear RX interrupt flag
    UART_rx_interrupt(UART_1);// process interrupt routine
}

void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;      // clear TX interrupt flag
    UART_tx_interrupt(UART_1);// process interrupt routine    
}

void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;      // clear RX interrupt flag
    UART_rx_interrupt(UART_2);// process interrupt routine    
}

void __attribute__((__interrupt__, no_auto_psv)) _U2TXInterrupt(void)
{
    IFS1bits.U2TXIF = 0;      // clear TX interrupt flag
    UART_tx_interrupt(UART_2);// process interrupt routine    
}

void __attribute__((__interrupt__, no_auto_psv)) _U3RXInterrupt(void)
{
    IFS5bits.U3RXIF = 0;      // clear RX interrupt flag
    UART_rx_interrupt(UART_3);// process interrupt routine    
}

void __attribute__((__interrupt__, no_auto_psv)) _U3TXInterrupt(void)
{
    IFS5bits.U3TXIF = 0;      // clear TX interrupt flag
    UART_tx_interrupt(UART_3);// process interrupt routine    
}

void __attribute__((__interrupt__, no_auto_psv)) _U1ErrInterrupt(void)
{
    unsigned char temp = 0;   
    // Must clear the overrun error to keep UART receiving
    U1STAbits.OERR = 0; 
    temp = U1RXREG;
    UART_struct[UART_1].UART_rx_counter = 0;
    IFS4bits.U1EIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _U2ErrInterrupt(void)
{
    unsigned char temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U2STAbits.OERR = 0; 
    temp = U2RXREG;
    UART_struct[UART_2].UART_rx_counter = 0;
    IFS4bits.U2EIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _U3ErrInterrupt(void)
{
    unsigned char temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U3STAbits.OERR = 0; 
    temp = U3RXREG;
    UART_struct[UART_3].UART_rx_counter = 0;
    IFS5bits.U3EIF = 0;
}