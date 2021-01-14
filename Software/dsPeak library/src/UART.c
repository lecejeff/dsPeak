//****************************************************************************//
// File      :  UART.c
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
// Includes  :  UART.h
//              string.h
//
// Purpose   :  Driver for the dsPIC33EP UART core
//              
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
#include "UART.h"
#include "string.h"

STRUCT_UART UART_struct[UART_QTY];

//*********void UART_init (unsigned char channel, unsigned long baud)*********//
//Description : Function initialize UART channel at specified baudrate
//
//Function prototype : void UART_init (unsigned char channel, unsigned long baud)
//
//Enter params       : unsigned char channel : UART_x module
//                   : unsigned long baud : UART_x baudrate
//
//Exit params        : None
//
//Function call      : UART_init(UART_1, 9600);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020  
//****************************************************************************//
void UART_init (unsigned char channel, unsigned long baud)
{
    switch (channel)
    {
        case UART_1:
            TRISBbits.TRISB7 = 0;        //UART1_TX is an output on RB7(RP39)
            RPOR2bits.RP39R = 1;         //RB7(RP39) is UART1_tx
            TRISBbits.TRISB8 = 1;        //UART1_RX is an input on RB8(RP40)
            RPINR18bits.U1RXR = 0x28;    //RB8(RP40) is UART1_RX
            UART_struct[UART_1].UART_rx_done = 0;   //RX not done
            UART_struct[UART_1].UART_rx_good = 0;
            UART_struct[UART_1].UART_rx_length = UART_MAX_RX;   //Set RX receive buffer
            UART_struct[UART_1].data_counter = 0;
            U1BRG = ((FCY / (16 * baud))-1);
            U1MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable 
            U1STA  = 0x0440;                // Reset status register and enable TX & RX
            IEC0bits.U1RXIE = 1;
            // UART 1 Error interrupt
            IPC2bits.U1RXIP = 4;    // RX interrupt priority
            IPC16bits.U1EIP = 3;    // Error interrupt priority
            IFS4bits.U1EIF = 0;     // Clear U1 error flag
            IFS0bits.U1RXIF = 0;
            IEC4bits.U1EIE = 1;     // Enable error interrupts            
            break;
           
        case UART_2:
            TRISCbits.TRISC9 = 0;           //UART2_TX is an output on RC9(RP57)
            RPOR7bits.RP57R = 0x03;         //RC9(RP68) is UART2_TX
            TRISBbits.TRISB15 = 1;          //UART2_RX is an input on RB15(RPI47)
            RPINR19bits.U2RXR = 0x2F;       //RB15(RPI47) is UART2_RX
                   
            UART_struct[UART_2].UART_rx_good = 0;
            UART_struct[UART_2].UART_rx_done = 0;  //RX not done
            UART_struct[UART_2].UART_rx_length = UART_MAX_RX;   //Set RX receive buffer
            UART_struct[UART_2].data_counter = 0;
            U2BRG = ((FCY / (16 * baud))-1);
            U2MODE = 0x8000;                // Reset UART to 8-n-1, alt pins, and enable 
            U2STA  = 0x0440;                // Reset status register and enable TX & RX
            IPC16bits.U2EIP = 3;    // Error interrupt priority
            IPC7bits.U2RXIP = 4;    // RX interrupt priority
            IFS4bits.U2EIF = 0;     // Clear U1 error flag
            IFS1bits.U2RXIF = 0;
            IEC4bits.U2EIE = 1;     // Enable error interrupts
            IEC1bits.U2RXIE = 1;
            break;
    }
    __delay_ms(50);
}   

//********void UART_putc (unsigned char channel, unsigned char data)**********//
//Description : Function transmits a single character on selected channel
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
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void UART_putc (unsigned char channel, unsigned char data)
{ 
    if (channel == UART_1)
    {
        while(!U1STAbits.TRMT); //Wait for prev tx to be done
        U1TXREG = data;         //Tx char over serial port       
    }

    if (channel == UART_2)
    {
        while(!U2STAbits.TRMT); //Wait for prev tx to be done
        U2TXREG = data;         //Tx char over serial port         
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
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020  
//****************************************************************************//
void UART_putc_ascii (unsigned char channel, unsigned char data)
{
    unsigned char byteh, bytel;
    hex_to_ascii(data, &byteh, &bytel); //Convert byte to ascii values
    UART_putc(channel, byteh);                   //Send high nibble
    UART_putc(channel, bytel);                   //send low nibble  
}

//****void UART_respond_to_request (unsigned char a, unsigned char b, unsigned char c, 
//        unsigned char d, unsigned char e, unsigned char channel)****//
//Description : Function sends 6 bytes of data to computer. Calculates the 
//              checksum and sends 6 bytes when called.
//
//Function prototype : void UART_respond_to_request (unsigned char a, unsigned char b, 
//                     unsigned char c, unsigned char d, unsigned char e, unsigned char channel)
//
//Enter params       : unsigned char a    : usually the platform address (R_#W) 
//                   : unsigned char b    : usually platform module
//                   : unsigned char c    : data 1
//                   : unsigned char d    : data 2
//                   : unsigned char e    : data 3
//                   : unsigned char channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_respond_to_request(0, 1, 2, 3, 4, UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020  
//****************************************************************************//
void UART_respond_to_request (unsigned char a, unsigned char b, unsigned char c, 
        unsigned char d, unsigned char e, unsigned char channel)
{
    unsigned char check = 0;
    check = ((a+b+c+d+e)&0x00FF);
    UART_putc(channel, a);
    UART_putc(channel, b);
    UART_putc(channel, c);
    UART_putc(channel, d);
    UART_putc(channel, e);
    UART_putc(channel, check);
}

//void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL)//
//Description : Function converts a HEX byte to 2 ascii characters
//
//Function prototype : void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL)
//
//Enter params       : unsigned char ucByte : hex byte to convert
//                   : unsigned char *ucByteH : ASCII character representing MSB
//                   : unsigned char *ucByteL : ASCII character representing LSB
//
//Exit params        : None
//
//Function call      : hex_to_ascii('B', &ucByteH, &ucByteL);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL)
{
    *ucByteH = ucByte >> 4;	    //ByteH = Byte d飡l頴 vers droite(val haute)
    *ucByteL = (ucByte & 0x0F);	//ByteL = Byte masqu頥n val basse   
    if ((*ucByteH < 10) && (*ucByteH >= 0)) //Si ByteH inclus entre 0  et 9
    {
        *ucByteH = *ucByteH + 0x30;			//Additionne 30, carac ASCII chiffre
    }
    
    else if ((*ucByteH >= 0x0A) && (*ucByteH <= 0x0F)) //Site ByteH entre 0A et 0F
    {
        *ucByteH = (*ucByteH + 0x37);		//Additionne 37, carac ASCII Lettre
    }
    
    if ((*ucByteL < 10) && (*ucByteL >= 0))	//Si ByteL inclus entre 0  et 9
    {
        *ucByteL = (*ucByteL + 0x30);		//Additionne 30, carac ASCII chiffre
    }
    
    else if ((*ucByteL >= 0x0A) && (*ucByteL <= 0x0F)) //Site ByteL entre 0A et 0F
    {
        *ucByteL = (*ucByteL + 0x37);		//Additionne 37, carac ASCII Lettre
    }   
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
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************// 
void UART_putstr (unsigned char channel, char *str)
{
    unsigned char i = 0;
    unsigned char length = strlen(str);
    for (; i <= length; i++)
    {
       UART_putc(channel, str[i]);      //Increm size amount variable
    }
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
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void UART_putstr_ascii (unsigned char channel, char *str)
{
    unsigned char i = 0;
    unsigned char length = strlen(str);
    for (; i <= length; i++)
    {
       UART_putc_ascii(channel, str[i]);      //Increm size amount variable
    }
}

//void UART_fill_transmit_buffer (unsigned char channel, unsigned char *data, unsigned char length)//
//Description : Function fills the transmit buffer before proceeding with UART transaction
//
//Function prototype : void UART_fill_transmit_buffer (unsigned char channel, unsigned char *data, unsigned char length)
//
//Enter params       : unsigned char channel : UART_x channel
//                   : unsigned char *data : ascii string of character
//                   : unsigned char length : length of data to transmit
//
//Exit params        : None
//
//Function call      : UART_fill_transmit_buffer(UART_1, data, sizeof(data));
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void UART_fill_transmit_buffer (unsigned char channel, unsigned char *data, unsigned char length)
{
    unsigned char i = 0;
    for (i=0; i<length; i++)
    {
        UART_struct[channel].UART_tx_data[i] = *data;
        data++;
    }    
}

//*****************void UART_send_tx_buffer (unsigned char channel)***************//
//Description : Function sends the content of the UART_struct[channel] over UART
//
//Function prototype : void UART_send_tx_buffer (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_send_tx_buffer(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void UART_send_tx_buffer (unsigned char channel)
{
    unsigned char i = 0;
    for (i=0; i<UART_MAX_TX; i++)
    {
        UART_putc(channel, UART_struct[channel].UART_tx_data[i]);
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
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020   
//****************************************************************************//
unsigned char * UART_get_rx_buffer (unsigned char channel)
{
    if (channel == UART_1)
    {
        return &UART_struct[UART_1].UART_process_data[0];
    }
    
    else if (channel == UART_2)
    {
        return &UART_struct[UART_2].UART_rx_data[0];
    }
    
    else
        return 0;
}

//*************void UART_clear_rx_buffer (unsigned char channel)**************//
//Description : Function resets the whole rx buffer to 0
//
//Function prototype : void UART_clear_rx_buffer (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : None
//
//Function call      : UART_clear_rx_buffer(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020   
//****************************************************************************//
void UART_clear_rx_buffer (unsigned char channel)
{
    unsigned char i = 0;
    for (i=0; i<UART_MAX_TX; i++)
    {
        UART_struct[channel].UART_process_data[i] = 0;
    }
}

//***********unsigned char UART_rx_done (unsigned char channel)***************//
//Description : Function scans received data and calculates checksum. If the 
//              calculated checksum is not the same as the checksum in the last 
//              index of the receive buffer, function returns 1. If the
//              calculated checksum is the same as the one in the last index,
//
//Function prototype : unsigned char UART_rx_done (unsigned char channel)
//
//Enter params       : unsigned char channel : UART_x channel
//
//Exit params        : unsigned char : 1(done), 0(rx is not done)
//
//Function call      : UART_receive = UART_rx_done(UART_1);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
unsigned char UART_rx_done (unsigned char channel)
{
    if (channel == UART_1)
    {      
        if (UART_struct[UART_1].UART_rx_done)
        {  
            UART_struct[UART_1].UART_rx_done = 0;
            return 1;
        }
        else return 0;
    }
    
    else if (channel == UART_2)
    {      
        if (UART_struct[UART_2].UART_rx_done)
        {  
            UART_struct[UART_2].UART_rx_done = 0;
            return 1;
        }
        else return 0;
    } 
    else return 0;
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
//Jean-Francois Bilodeau    MPLab X v5.00    09/10/2018   
//****************************************************************************//
void UART_rx_interrupt (unsigned char channel)
{
    if (channel == UART_1) 
    {
        if (UART_struct[UART_1].data_counter < UART_struct[UART_1].UART_rx_length)      //Data to be rx'd not done
        {
            UART_struct[UART_1].UART_rx_data[UART_struct[UART_1].data_counter] = U1RXREG;//Get data from buffer
            UART_struct[UART_1].data_counter++;                             //Increm amount variable
            UART_struct[UART_1].UART_rx_done = 0;             //RX is not done
        }
       
        if (UART_struct[UART_1].data_counter >= UART_struct[UART_1].UART_rx_length)     //Data all received
        {
            UART_struct[UART_1].UART_rx_done = 1;             //RX is done      
            UART_struct[UART_1].data_counter = 0;
        }
    }

    else if (channel == UART_2) 
    {
        if (UART_struct[UART_2].data_counter < UART_struct[UART_2].UART_rx_length)      //Data to be rx'd not done
        {
            UART_struct[UART_2].UART_rx_data[UART_struct[UART_2].data_counter] = U2RXREG;//Get data from buffer
            UART_struct[UART_2].data_counter++;                             //Increm amount variable
            UART_struct[UART_2].UART_rx_done = 0;             //RX is not done
        }

        if (UART_struct[UART_2].data_counter >= UART_struct[UART_2].UART_rx_length)     //Data all received
        {
            UART_struct[UART_2].UART_rx_done = 1;             //RX is done      
            UART_struct[UART_2].data_counter = 0;
        }
    }     
}


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;      // clear RX interrupt flag
    UART_rx_interrupt(UART_1);// process interrupt routine
}

void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;      // clear RX interrupt flag
    UART_rx_interrupt(UART_2);// process interrupt routine    
}


void __attribute__((__interrupt__, no_auto_psv)) _U1ErrInterrupt(void)
{
    unsigned char temp = 0;   
    // Must clear the overrun error to keep UART receiving
    U1STAbits.OERR = 0; 
    temp = U1RXREG;
    UART_struct[UART_1].data_counter = 0;
    IFS4bits.U1EIF = 0;     // Clear U1 error flag
}

void __attribute__((__interrupt__, no_auto_psv)) _U2ErrInterrupt(void)
{
    unsigned char temp = 0;   
    /* Must clear the overrun error to keep UART receiving */
    U2STAbits.OERR = 0; 
    temp = U2RXREG;
    UART_struct[UART_2].data_counter = 0;
    IFS4bits.U2EIF = 0;     // Clear U1 error flag
}
