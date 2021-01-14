//***************************************************************************//
// File      : spi.c
//
// Functions :  void SPI_init (unsigned char mode, unsigned char ppre, unsigned char spre, unsigned char channel); 
//              void SPI_master_write (unsigned char *data, unsigned char length, unsigned char chip, unsigned char channel);
//              void SPI_fill_transmit_buffer (unsigned char * data, unsigned char length, unsigned char channel);
//              unsigned char SPI_rx_done (unsigned char channel);
//              unsigned char * SPI_get_rx_buffer (unsigned char channel);
//              void SPI_master_deassert_cs (unsigned char chip);
//              void SPI_master_assert_cs (unsigned char chip);
//              void SPI_slave_initiate (void);
//              void SPI_clear_rx_buffer (unsigned char channel);
//
// Includes  :  spi.h
//           
// Purpose   :  Driver for the dsPIC33EP SPI peripheral
//              2 seperate SPI channel on Intellitrol
//              SPI_1 - DRV8873 control port, SPI master between dsPIC and peripheral
//              SPI_2 - Intellitrol control port, SPI slave between dsPIC and external master
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#include "spi.h"

SPI_struct spi_struct[SPI_QTY];

//void SPI_init (unsigned char channel, unsigned char mode, unsigned char ppre, unsigned char spre)//
//Description : Function initializes SPI module on selected MODE with Prim and
//              secondary prescalers to scale down the clock.
//              Max Fclk (Write only) -> 15MHz
//              Full duplex           -> 8MHz
//
//Function prototype :  void SPI_init (unsigned char mode, unsigned char ppre, unsigned char spre, unsigned char channel) 
//
//Enter params       :  unsigned char mode : SPI MODE (0...3)
//                      unsigned char ppre : Primary clock prescaler
//                      unsigned char spre : Secondary clock prescaler
//                      unsigned char channel : SPI channel
//
//Exit params        : None
//
//Function call      : SPI_init(SPI_MODE0, 5, 3, SPI_2);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_init (unsigned char channel, unsigned char mode, unsigned char ppre, unsigned char spre) 
{    
    switch (channel)
    {
        // SPI1 is DRV8873 control port, SPI master
        case SPI_1:
            IEC0bits.SPI1IE = 0;
            SPI1STATbits.SPIEN = 0;     // Disable SPI module 
            IFS0bits.SPI1EIF = 0;       // Disable SPI error int flag 
            IFS0bits.SPI1IF = 0;        // Disable SPI int flag 
            SPI1CON1bits.DISSCK = 0;    // Internal serial clock is enabled 
            SPI1CON1bits.DISSDO = 0;    // SDOx pin is controlled by the module 
            SPI1CON1bits.MODE16 = 0;    // Communication is byte wide (8bit)   
            SPI1CON1bits.MSTEN = 1;     // SPI master mode enabled 
            SPI1CON2bits.FRMEN = 0;     // Frame mode disabled 
            SPI1CON1bits.SSEN = 0;      // GPIO controls SPI /CS       
            switch (mode)
            {
                case SPI_MODE0:
                    SPI1CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI1CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    SPI1CON1bits.CKP = 0;   //  
                    break;

                case SPI_MODE1:
                    SPI1CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI1CON1bits.CKP = 0;   //  
                    SPI1CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI1CON1bits.CKE = 1;   // Mode 2 configurations  
                    SPI1CON1bits.CKP = 1;   //  
                    SPI1CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI1CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI1CON1bits.CKP = 1;   //  
                    SPI1CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI1 input/output pin mapping  
            TRISAbits.TRISA4 = 0;   // RA4 is output (MOSI) 
            TRISAbits.TRISA9 = 1;   // RA9 is input (MISO) 
            TRISCbits.TRISC3 = 0;   // RC3 is output (SCLK) 
            TRISBbits.TRISB0 = 0;   // RB0 is output (#CS)
            
            IPC2bits.SPI1IP = 4;    // SPI int priority is 4 
            IFS0bits.SPI1EIF = 0;   // Disable SPI error int flag 
            IFS0bits.SPI1IF = 0;    // Disable SPI int flag   
            spi_struct[channel].spi_done = 0;       // Reset state machine variables
            spi_struct[channel].spi_tx_cnt = 0;     // 
            spi_struct[channel].spi_rx_cnt = 0;     // 
            spi_struct[channel].spi_tx_length = 0;  // 
            
            SPI1STATbits.SPIEN = 1; // Enable SPI module               
            break;
            
        // SPI2 is intellitrol SPI interface, slave port    
        case SPI_2:
            SPI2STATbits.SPIEN = 0;     // Disable SPI module 
            SPI2CON1bits.DISSCK = 0;    // Internal serial clock is enabled 
            SPI2CON1bits.DISSDO = 0;    // SDOx pin is controlled by the module 
            SPI2CON1bits.MODE16 = 0;    // Communication is byte wide (8bit)   
            SPI2CON1bits.MSTEN = 0;     // SPI slave mode
            SPI2CON2bits.FRMEN = 0;     // Frame mode disabled 
            SPI2CON1bits.SSEN = 1;      // Module controls SPI /CS        

            switch (mode)
            {
                case SPI_MODE0:
                    SPI2CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI2CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    SPI2CON1bits.CKP = 0;   //  
                    break;

                case SPI_MODE1:
                    SPI2CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI2CON1bits.CKP = 0;   //  
                    SPI2CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI2CON1bits.CKE = 1;   // Mode 2 configurations
                    SPI2CON1bits.CKP = 1;   // 
                    SPI2CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI2CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI2CON1bits.CKP = 1; 
                    SPI2CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI2 input/output pin mapping 
            TRISBbits.TRISB13 = 1;      // RB13 is #CS input on intellitrol
            TRISBbits.TRISB10 = 0;      // RB10 is output (Drive out to master in) 
            TRISBbits.TRISB12 = 1;      // RB12 is input (Master out to drive in) 
            TRISBbits.TRISB11 = 1;      // RB11 is input (SCLK) 

            RPINR23bits.SS2R = 0x2D;    // RB13 (RPI45) is Intellitrol #CS
            RPINR22bits.SDI2R = 0x2C;   // RB12 (RPI44) is SPI2_MISO
            RPOR4bits.RP42R = 0x08;     // RB10(RP42) is SPI2_MOSI 
            RPINR22bits.SCK2R = 0x2B;   // SPI2_SCLK is an input on RP43
            RPOR4bits.RP43R = 0x09;     // SPI2_SCLK both input and output
            
            CNENBbits.CNIEB13 = 1;      // Enable change notice interrupt on CS            

            IFS2bits.SPI2EIF = 0;       // Disable SPI error int flag 
            IFS2bits.SPI2IF = 0;        // Disable SPI int flag               
            IPC8bits.SPI2IP = 4;        // SPI int priority is 4  
            IEC2bits.SPI2IE = 1;
            IEC2bits.SPI2EIE = 1;
            spi_struct[channel].spi_done = 0;                   // no SPI transaction completed
            spi_struct[channel].spi_tx_cnt = 0;                 // no SPI transaction done
            spi_struct[channel].spi_rx_cnt = 0;                 // no SPI transaction done
            spi_struct[channel].spi_tx_length = SPI_MSG_LENGTH; // no SPI transaction done
            
            SPI2STATbits.SPIEN = 1;     // Enable SPI module                    
            break;        
    }   
}

//void SPI_write (unsigned char *data, unsigned char length, unsigned char chip, unsigned char channel)//
//Description : Function write specified array of data, of specified length, to
//              specified chip (define it in SPI.h)
//              This function is to be used with a MASTER SPI only
//
//Function prototype : void SPI_write (unsigned char *data, unsigned char length, unsigned char chip, unsigned char channel)
//
//Enter params       :  unsigned char *data  : Array of data to write
//                      unsigned char length : Array length
//                      unsigned char chip : spi chip to select
//                      unsigned char channel : SPI_x channel
//
//Exit params        : None
//
//Function call      : SPI_write(ptr, sizeof(ptr), MCP23017, SPI_1); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_master_write (unsigned char channel, unsigned char *data, unsigned char length, unsigned char chip)
{
    unsigned char i = 0;
    if (channel == SPI_1)                   
    {
        while(IEC0bits.SPI1IE);             // Wait for previous int to be done 
        for (i=0; i<length; i++)            // Fill struct transmit array 
        {
           spi_struct[SPI_1].spi_tx_data[i] = data[i];
        }
        spi_struct[SPI_1].spi_rx_cnt = 0;           // Set receive counter to 0 
        spi_struct[SPI_1].spi_tx_cnt = 0;           // Set transmit counter to 0
        spi_struct[SPI_1].spi_chip = chip;          // Set SPI chip to struct 
        spi_struct[SPI_1].spi_tx_length = length;   // Write length to struct 
        spi_struct[SPI_1].spi_done = 0;             // SPI bus is busy 
        IEC0bits.SPI1IE = 1;                        // Enable SPI interrupt 
        SPI_master_assert_cs(chip);                 // Assert /CS from specified SPI chip  
        SPI1BUF = spi_struct[SPI_1].spi_tx_data[0]; // Send first byte 
    }  
}

//void SPI_fill_transmit_buffer (unsigned char channel, unsigned char * data, unsigned char length)//
//Description : Function fills the transmit buffer for a slave SPI port
//              When a #CS is detected on the SPI port, the transfer is started
//              automatically between the slave and the master
//              This function is to be used with a SLAVE SPI only
//
//Function prototype : void SPI_fill_transmit_buffer (unsigned char * data, unsigned char length, unsigned char channel)
//
//Enter params       :  unsigned char *data  : Array of data to write
//                      unsigned char length : Array length
//                      unsigned char channel : spi chip to select
//
//Exit params        : None
//
//Function call      : SPI_fill_transmit_buffer(ptr, sizeof(ptr), SPI_2); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_fill_transmit_buffer (unsigned char channel, unsigned char * data, unsigned char length)
{
    unsigned char i;
    for (i = 0; i < length; i++)
    {
        spi_struct[SPI_2].spi_tx_data[i] = *data;
        data++;
    }
}

//*********************void SPI_slave_initiate (void)*************************//
//Description : Function initiates a slave SPI transfer by writing a byte to 
//              the SPI buffer.
//              This function is to be used with a SLAVE SPI only
//              This function shall be called when a #CS event is detected on
//              the slave
//
//Function prototype : void SPI_slave_initiate (void)
//
//Enter params       :  None
//
//Exit params        :  None
//
//Function call      :  SPI_slave_initiate(); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_slave_initiate (void)
{
    SPI2BUF = spi_struct[SPI_2].spi_tx_data[0];
    spi_struct[SPI_2].spi_tx_cnt = 1;
}

//*****************void SPI_deassert_cs (unsigned char chip)******************//
//Description : Function deassert spi /CS to specified chip
//
//Function prototype : void SPI_deassert_cs (unsigned char chip)
//
//Enter params       : unsigned char chip : chip to deassert /CS
//
//Exit params        : None
//
//Function call      : SPI_deassert_cs(MCP23017); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_master_deassert_cs (unsigned char chip)
{
   switch (chip)            // Upon switch case 
   {
       case DRV8873_CHIP:
           DRV8873_CS = 1;
           break;
           
       case EXTSPI2_CHIP:
           EXTSPI2_CS = 1;
           break;
   }
}

//*********************void SPI_assert_cs (unsigned char chip)****************//
//Description : Function assert spi /CS to specified chip
//
//Function prototype : void SPI_assert_cs (unsigned char chip)
//
//Enter params       : unsigned char chip : chip to assert /CS
//
//Exit params        : None
//
//Function call      : SPI_assert_cs(MCP23017); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_master_assert_cs (unsigned char chip)
{
   switch (chip)            // Upon switch case 
   {
       case DRV8873_CHIP:
           DRV8873_CS = 0;
           break;  
           
       case EXTSPI2_CHIP:
           EXTSPI2_CS = 0;
           break;
   }    
}

//**********unsigned char * SPI_get_rx_buffer (unsigned char channel)************//
//Description : Function returns a pointer to the 1st element of the spi rx buffer
//
//Function prototype : unsigned char * SPI_get_rx_buffer (unsigned char channel)
//
//Enter params       : unsigned char channel : SPI_x channel                   
//
//Exit params        : unsigned char * : pointer to 1st element location
//
//Function call      : unsigned char * = SPI_get_rx_buffer(SPI_2); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
unsigned char * SPI_get_rx_buffer (unsigned char channel)
{
    return(&spi_struct[channel].spi_rx_data[0]);// Return array pointer on 1st element
}

//*********unsigned char SPI_rx_done (unsigned char channel)************//
//Description : Function check is SPI transfer is over, meaning the SPI bus is free 
//
//Function prototype : unsigned char SPI_rx_done (unsigned char channel)
//
//Enter params       : unsigned char channel : SPI_x channel
//
//Exit params        : unsigned char 0 : Bus is BUSY
//                                   1 : Bus is FREE
//
//Function call      : unsigned char = SPI_rx_done(SPI_1); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
unsigned char SPI_rx_done (unsigned char channel)
{
    if (spi_struct[channel].spi_done)
    {
        spi_struct[channel].spi_done = 0;
        return 1;
    }
    else return 0;
}

//************void SPI_clear_rx_buffer (unsigned char channel)****************//
//Description : Function clears the RX buffer of the specified SPI channel
//
//Function prototype : void SPI_clear_rx_buffer (unsigned char channel)
//
//Enter params       : unsigned char channel : SPI_x channel
//
//Exit params        : None
//
//Function call      : SPI_clear_rx_buffer(SPI_1); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_clear_rx_buffer (unsigned char channel)
{
    unsigned char i = 0;
    for (i=0; i < SPI_MSG_LENGTH; i++)
    {
        spi_struct[channel].spi_rx_data[i] = 0;
    }
}

//**************************SPI1 interrupt function***************************//
//Description : SPI master interrupt.
//
//Function prototype : _SPI1Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : Called when interrupt happens
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI1Interrupt(void)
{
    IFS0bits.SPI1IF = 0;                   // clear interrupt flag 
    spi_struct[SPI_1].spi_tx_cnt++;
    if (spi_struct[SPI_1].spi_tx_cnt < spi_struct[SPI_1].spi_tx_length)// if write_cnt < write_length 
    {
        spi_struct[SPI_1].spi_rx_data[spi_struct[SPI_1].spi_rx_cnt] = SPI1BUF;  // Get Data 
        spi_struct[SPI_1].spi_rx_cnt++;                           // Increm RD cnter 
        SPI1BUF = spi_struct[SPI_1].spi_tx_data[spi_struct[SPI_1].spi_tx_cnt];      // Write new data  
        spi_struct[SPI_1].spi_tx_cnt++;                                       // Increm WR cnter 
    }
    
    else // If write_cnt == length, all data was transmitted 
    {               
        spi_struct[SPI_1].spi_rx_data[spi_struct[SPI_1].spi_rx_cnt] = SPI1BUF;// Get last Data 
        spi_struct[SPI_1].spi_rx_cnt = 0;            // Clear RD cnter   
        spi_struct[SPI_1].spi_tx_cnt = 0;            // Clear WR_CNT 
        SPI_master_deassert_cs(spi_struct[SPI_1].spi_chip); // Deassert CS   
        spi_struct[SPI_1].spi_done = 1;                 // SPI transaction over
        IEC0bits.SPI1IE = 0;                            // Disable SPI interrupt  
    }
}

//**************************SPI2 interrupt function***************************//
//Description : SPI slave interrupt
//
//Function prototype : _SPI2Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : Called when interrupt happens
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI2Interrupt(void)
{
    IFS2bits.SPI2IF = 0;                    // clear interrupt flag    
    if (spi_struct[SPI_2].spi_tx_cnt < DRIVE_DATA_LENGTH)// if write_cnt < write_length 
    {
        spi_struct[SPI_2].spi_rx_data[spi_struct[SPI_2].spi_rx_cnt] = SPI2BUF;  // Get Data 
        spi_struct[SPI_2].spi_rx_cnt++;                     // Increm RD cnter 
        SPI2BUF = spi_struct[SPI_2].spi_tx_data[spi_struct[SPI_2].spi_tx_cnt];// Write new data  
        spi_struct[SPI_2].spi_tx_cnt++;                                  // Increm WR cnter 
    }
    
    else // If write_cnt == length, all data was transmitted 
    {               
        spi_struct[SPI_2].spi_rx_data[spi_struct[SPI_2].spi_rx_cnt] = SPI2BUF; // Get last Data 
        spi_struct[SPI_2].spi_rx_cnt = 0;             // Clear RD cnter   
        spi_struct[SPI_2].spi_tx_cnt = 0;             // Clear WR_CNT  
        spi_struct[SPI_2].spi_done = 1;               // SPI transaction over 
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI2ErrInterrupt(void)
{
    IFS2bits.SPI2EIF = 0;
    SPI2STATbits.SPIROV = 0;
}