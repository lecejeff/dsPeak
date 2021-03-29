//***************************************************************************//
// File      : spi.c
//
// Functions :  void SPI_init (uint8_t mode, uint8_t ppre, uint8_t spre, uint8_t channel); 
//              void SPI_master_write (uint8_t *data, uint8_t length, uint8_t chip, uint8_t channel);
//              void SPI_fill_transmit_buffer (uint8_t * data, uint8_t length, uint8_t channel);
//              uint8_t SPI_rx_done (uint8_t channel);
//              uint8_t * SPI_get_rx_buffer (uint8_t channel);
//              void SPI_master_deassert_cs (uint8_t chip);
//              void SPI_master_assert_cs (uint8_t chip);
//              void SPI_slave_initiate (void);
//              void SPI_clear_rx_buffer (uint8_t channel);
//
// Includes  :  spi.h
//           
// Purpose   :  Driver for the dsPIC33EP SPI peripheral
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#include "spi.h"

SPI_struct spi_struct[SPI_QTY];

//void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre)//
//Description : Function initializes SPI module on selected MODE with Prim and
//              secondary prescalers to scale down the clock.
//              Max Fclk (Write only) -> 15MHz
//              Full duplex           -> 8MHz
//
//Function prototype :  void SPI_init (uint8_t mode, uint8_t ppre, uint8_t spre, uint8_t channel) 
//
//Enter params       :  uint8_t mode : SPI MODE (0...3)
//                      uint8_t ppre : Primary clock prescaler
//                      uint8_t spre : Secondary clock prescaler
//                      uint8_t channel : SPI channel
//
//Exit params        : None
//
//Function call      : SPI_init(SPI_MODE0, 5, 3, SPI_2);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre) 
{    
    switch (channel)
    {
        // SPI1 is FTDI EVE interface
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
            // SPI1 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI1CON1bits.PPRE = ppre;
            SPI1CON1bits.SPRE = spre;
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
            TRISFbits.TRISF13 = 0;          // RF13 configured as an output (SPI1_MOSI) 
            TRISAbits.TRISA1 = 1;           // RA1 configured as an input (SPI1_MISO) 
            TRISFbits.TRISF12 = 0;          // RF12 configured as an output (SPI1_SCLK) 
            TRISBbits.TRISB11 = 0;          // RB11 configured as an output (SPI1_nCS)
            FT8XX_EVE_CS_PIN = 1;           // Set #CS to high
            
            // SPI1 peripheral pin assignment       
            RPOR12bits.RP109R = 0x05;       // RF13 (RP109) assigned to SPI1_MOSI 
            RPINR20bits.SDI1R = 17;         // RA1 (RPI17) assigned to SPI1_MISO
            RPINR20bits.SCK1R = 108;        // RF12 (RP108) assigned to SPI1_SCLK input
            RPOR11bits.RP108R = 0x06;       // RF12 (RP108) assigned to SPI1_SCLK output         
            
            IPC2bits.SPI1IP = 4;            // SPI int priority is 4 
            IFS0bits.SPI1EIF = 0;           // Disable SPI error int flag 
            IFS0bits.SPI1IF = 0;            // Disable SPI int flag   
            SPI1STATbits.SPIEN = 1;         // Enable SPI module               
            break;
            
        // SPI2 is SD / FLASH interface  
        case SPI_2:
            IEC2bits.SPI2IE = 0;
            SPI2STATbits.SPIEN = 0;     // Disable SPI module 
            IFS2bits.SPI2EIF = 0;       // Disable SPI error int flag 
            IFS2bits.SPI2IF = 0;        // Disable SPI int flag 
            SPI2CON1bits.DISSCK = 0;    // Internal serial clock is enabled 
            SPI2CON1bits.DISSDO = 0;    // SDOx pin is controlled by the module 
            SPI2CON1bits.MODE16 = 0;    // Communication is byte wide (8bit)   
            SPI2CON1bits.MSTEN = 1;     // SPI master mode enabled 
            SPI2CON2bits.FRMEN = 0;     // Frame mode disabled 
            SPI2CON1bits.SSEN = 0;      // GPIO controls SPI /CS      
            // SPI2 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI2CON1bits.PPRE = ppre;
            SPI2CON1bits.SPRE = spre;
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
            TRISGbits.TRISG6 = 0;           // RG6 configured as an output (SPI2_SCLK)
            TRISGbits.TRISG7 = 1;           // RG7 configured as an input (SPI2_MISO)
            TRISGbits.TRISG8 = 0;           // RGB configured as an output (SPI2_MOSI)
            TRISGbits.TRISG9 = 0;           // RB9 configured as an output (SD_nCS)
            TRISGbits.TRISG0 = 0;           // RG0 configured as an output (FLASH_nCS)       

            IPC8bits.SPI2IP = 4;            // SPI int priority is 4  
            IFS2bits.SPI2EIF = 0;           // Disable SPI error int flag 
            IFS2bits.SPI2IF = 0;            // Disable SPI int flag                 
            SPI2STATbits.SPIEN = 1;         // Enable SPI module                    
            break;  
            
        // SPI3 is the AUDIO CODEC configuration interface    
        case SPI_3:
            IEC5bits.SPI3IE = 0;
            SPI3STATbits.SPIEN = 0;     // Disable SPI module 
            IFS5bits.SPI3EIF = 0;       // Disable SPI error int flag 
            IFS5bits.SPI3IF = 0;        // Disable SPI int flag 
            SPI3CON1bits.DISSCK = 0;    // Internal serial clock is enabled 
            SPI3CON1bits.DISSDO = 0;    // SDOx pin is controlled by the module 
            SPI3CON1bits.MODE16 = 0;    // Communication is byte wide (8bit)   
            SPI3CON1bits.MSTEN = 1;     // SPI master mode enabled 
            SPI3CON2bits.FRMEN = 0;     // Frame mode disabled 
            SPI3CON1bits.SSEN = 0;      // GPIO controls SPI /CS      
            // SPI3 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI3CON1bits.PPRE = ppre;
            SPI3CON1bits.SPRE = spre;
            switch (mode)
            {
                case SPI_MODE0:
                    SPI3CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI3CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    SPI3CON1bits.CKP = 0;   //  
                    break;

                case SPI_MODE1:
                    SPI3CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI3CON1bits.CKP = 0;   //  
                    SPI3CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI3CON1bits.CKE = 1;   // Mode 2 configurations
                    SPI3CON1bits.CKP = 1;   // 
                    SPI3CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI3CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI3CON1bits.CKP = 1; 
                    SPI3CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI3 input/output pin mapping 
            TRISDbits.TRISD5 = 0;           // RD5 configured as an output (SPI3_SCLK)
            TRISDbits.TRISD4 = 0;           // RD4 configured as an output (SPI3_MOSI)
            TRISDbits.TRISD6 = 0;           // RD6 configured as an output (SPI3_nCS) 
            AUDIO_CODEC_CS_PIN = 1;

            // SPI3 peripheral pin assignment       
            RPOR2bits.RP68R = 0x1F;         // RD4 (RP68) assigned to SPI3_MOSI 
            RPINR29bits.SCK3R = 69;         // RD5 (RP69) assigned to SPI3_SCLK input
            RPOR2bits.RP69R = 0x20;         // RD5 (RP69) assigned to SPI3_SCLK output             
            
            IPC22bits.SPI3IP = 4;           // SPI int priority is 4  
            IFS5bits.SPI3EIF = 0;           // Disable SPI error int flag 
            IFS5bits.SPI3IF = 0;            // Disable SPI int flag               
            SPI3STATbits.SPIEN = 1;     // Enable SPI module                   
            break;
          
        // SPI4 is the MikroBus interface     
        case SPI_4:
            IEC7bits.SPI4IE = 0;
            SPI4STATbits.SPIEN = 0;     // Disable SPI module 
            IFS7bits.SPI4EIF = 0;       // Disable SPI error int flag 
            IFS7bits.SPI4IF = 0;        // Disable SPI int flag 
            SPI4CON1bits.DISSCK = 0;    // Internal serial clock is enabled 
            SPI4CON1bits.DISSDO = 0;    // SDOx pin is controlled by the module 
            SPI4CON1bits.MODE16 = 0;    // Communication is byte wide (8bit)   
            SPI4CON1bits.MSTEN = 1;     // SPI master mode enabled 
            SPI4CON2bits.FRMEN = 0;     // Frame mode disabled 
            SPI4CON1bits.SSEN = 0;      // GPIO controls SPI /CS      
            // SPI3 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI4CON1bits.PPRE = ppre;
            SPI4CON1bits.SPRE = spre;
            switch (mode)
            {
                case SPI_MODE0:
                    SPI4CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI4CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    SPI4CON1bits.CKP = 0;   //  
                    break;

                case SPI_MODE1:
                    SPI4CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI4CON1bits.CKP = 0;   //  
                    SPI4CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI4CON1bits.CKE = 1;   // Mode 2 configurations
                    SPI4CON1bits.CKP = 1;   // 
                    SPI4CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI4CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI4CON1bits.CKP = 1; 
                    SPI4CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI4 input/output pin mapping 
            TRISFbits.TRISF0 = 0;           // RF0 configured as an output (SPI4_MOSI)
            TRISDbits.TRISD13 = 1;          // RD13 configured as an input (SPI4_MISO)            
            TRISDbits.TRISD7 = 0;           // RD7 configured as an output (SPI4_SCLK)
            TRISHbits.TRISH15 = 0;          // RH15 configured as an output (MikroBus1_nCS)    
            TRISHbits.TRISH13 = 0;          // RH13 configured as an output (MikroBus2_nCS)   

            // SPI4 peripheral pin assignment       
            RPOR2bits.RP68R = 0x22;         // RF0 (RP68) assigned to SPI4_MOSI 
            RPINR31bits.SDI4R = 77;         // RD13 (RPI77) assigned to SPI4_MISO
            RPINR31bits.SCK4R = 71;         // RD7 (RP71) assigned to SPI4_SCLK input
            RPOR3bits.RP71R = 0x23;         // RD7 (RP71) assigned to SPI4_SCLK output             
            
            IPC30bits.SPI4IP = 4;           // SPI int priority is 4  
            IFS7bits.SPI4EIF = 0;           // Disable SPI error int flag 
            IFS7bits.SPI4IF = 0;            // Disable SPI int flag               
            SPI4STATbits.SPIEN = 1;         // Enable SPI module              
            break;
    }
    
    spi_struct[channel].spi_txfer_done = 0;       // Reset SPI module state machine
    spi_struct[channel].spi_tx_cnt = 0;     // 
    spi_struct[channel].spi_rx_cnt = 0;     // 
    spi_struct[channel].spi_tx_length = 0;  // 
}

//void SPI_write (uint8_t *data, uint8_t length, uint8_t chip, uint8_t channel)//
//Description : Function write specified array of data, of specified length, to
//              specified chip (define it in SPI.h)
//              This function is to be used with a MASTER SPI only
//
//Function prototype : void SPI_write (uint8_t *data, uint8_t length, uint8_t chip, uint8_t channel)
//
//Enter params       :  uint8_t *data  : Array of data to write
//                      uint8_t length : Array length
//                      uint8_t chip : spi chip to select
//                      uint8_t channel : SPI_x channel
//
//Exit params        : None
//
//Function call      : SPI_write(ptr, sizeof(ptr), MCP23017, SPI_1); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_master_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip)
{
    uint8_t i = 0;
    if (channel == SPI_1)                   
    {
        while(IEC0bits.SPI1IE);             // Wait for previous int to be done 
        for (; i<length; i++)            // Fill struct transmit array 
        {
           spi_struct[SPI_1].spi_tx_data[i] = data[i];
        }
        spi_struct[SPI_1].spi_rx_cnt = 0;           // Set receive counter to 0 
        spi_struct[SPI_1].spi_tx_cnt = 0;           // Set transmit counter to 0
        spi_struct[SPI_1].spi_chip = chip;          // Set SPI chip to struct 
        spi_struct[SPI_1].spi_tx_length = length;   // Write length to struct 
        spi_struct[SPI_1].spi_txfer_done = 0;             // SPI bus is busy 
        IEC0bits.SPI1IE = 1;                        // Enable SPI interrupt 
        SPI_master_assert_cs(chip);                 // Assert /CS from specified SPI chip  
        SPI1BUF = spi_struct[SPI_1].spi_tx_data[0]; // Send first byte 
    } 
    
    if (channel == SPI_2)                   
    {
        while(IEC2bits.SPI2IE);             // Wait for previous int to be done 
        for (; i<length; i++)            // Fill struct transmit array 
        {
           spi_struct[SPI_2].spi_tx_data[i] = data[i];
        }
        spi_struct[SPI_2].spi_rx_cnt = 0;           // Set receive counter to 0 
        spi_struct[SPI_2].spi_tx_cnt = 0;           // Set transmit counter to 0
        spi_struct[SPI_2].spi_chip = chip;          // Set SPI chip to struct 
        spi_struct[SPI_2].spi_tx_length = length;   // Write length to struct 
        spi_struct[SPI_2].spi_txfer_done = 0;             // SPI bus is busy 
        IEC2bits.SPI2IE = 1;                        // Enable SPI interrupt 
        SPI_master_assert_cs(chip);                 // Assert /CS from specified SPI chip  
        SPI2BUF = spi_struct[SPI_2].spi_tx_data[0]; // Send first byte 
    }
    
    if (channel == SPI_3)                   
    {
        while(IEC5bits.SPI3IE);             // Wait for previous int to be done 
        for (; i<length; i++)            // Fill struct transmit array 
        {
           spi_struct[SPI_3].spi_tx_data[i] = data[i];
        }
        spi_struct[SPI_3].spi_rx_cnt = 0;           // Set receive counter to 0 
        spi_struct[SPI_3].spi_tx_cnt = 0;           // Set transmit counter to 0
        spi_struct[SPI_3].spi_chip = chip;          // Set SPI chip to struct 
        spi_struct[SPI_3].spi_tx_length = length;   // Write length to struct 
        spi_struct[SPI_3].spi_txfer_done = 0;       // SPI bus is busy 
        IEC5bits.SPI3IE = 1;                        // Enable SPI interrupt 
        SPI_master_assert_cs(chip);                 // Assert /CS from specified SPI chip  
        SPI3BUF = spi_struct[SPI_3].spi_tx_data[0]; // Send first byte 
    }
    
    if (channel == SPI_4)                   
    {
        while(IEC7bits.SPI4IE);             // Wait for previous int to be done 
        for (; i<length; i++)            // Fill struct transmit array 
        {
           spi_struct[SPI_4].spi_tx_data[i] = data[i];
        }
        spi_struct[SPI_4].spi_rx_cnt = 0;           // Set receive counter to 0 
        spi_struct[SPI_4].spi_tx_cnt = 0;           // Set transmit counter to 0
        spi_struct[SPI_4].spi_chip = chip;          // Set SPI chip to struct 
        spi_struct[SPI_4].spi_tx_length = length;   // Write length to struct 
        spi_struct[SPI_4].spi_txfer_done = 0;             // SPI bus is busy 
        IEC7bits.SPI4IE = 1;                        // Enable SPI interrupt 
        SPI_master_assert_cs(chip);                 // Assert /CS from specified SPI chip  
        SPI4BUF = spi_struct[SPI_4].spi_tx_data[0]; // Send first byte 
    }    
}

//*****************void SPI_deassert_cs (uint8_t chip)******************//
//Description : Function deassert spi /CS to specified chip
//
//Function prototype : void SPI_deassert_cs (uint8_t chip)
//
//Enter params       : uint8_t chip : chip to deassert /CS
//
//Exit params        : None
//
//Function call      : SPI_deassert_cs(MCP23017); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_master_deassert_cs (uint8_t chip)
{
   switch (chip)            // Upon switch case 
   {
       case FT8XX_EVE_CS:
           FT8XX_EVE_CS_PIN = 1;
           break;  
           
       case SD_CARD_CS:
           SD_CARD_CS_PIN = 1;
           break;

       case FLASH_MEMORY_CS:
           FLASH_MEMORY_CS_PIN = 1;
           break;

       case AUDIO_CODEC_CS:
           AUDIO_CODEC_CS_PIN = 1;
           break;

       case MIKROBUS1_CS:
           MIKROBUS1_CS_PIN = 1;
           break;

       case MIKROBUS2_CS:
           MIKROBUS2_CS_PIN = 1;
           break;           
   }    
}

//*********************void SPI_assert_cs (uint8_t chip)****************//
//Description : Function assert spi /CS to specified chip
//
//Function prototype : void SPI_assert_cs (uint8_t chip)
//
//Enter params       : uint8_t chip : chip to assert /CS
//
//Exit params        : None
//
//Function call      : SPI_assert_cs(MCP23017); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_master_assert_cs (uint8_t chip)
{
   switch (chip)            // Upon switch case 
   {
       case FT8XX_EVE_CS:
           FT8XX_EVE_CS_PIN = 0;
           break;  
           
       case SD_CARD_CS:
           SD_CARD_CS_PIN = 0;
           break;

       case FLASH_MEMORY_CS:
           FLASH_MEMORY_CS_PIN = 0;
           break;

       case AUDIO_CODEC_CS:
           AUDIO_CODEC_CS_PIN = 0;
           break;

       case MIKROBUS1_CS:
           MIKROBUS1_CS_PIN = 0;
           break;

       case MIKROBUS2_CS:
           MIKROBUS2_CS_PIN = 0;
           break;           
   }    
}

//**********uint8_t * SPI_get_rx_buffer (uint8_t channel)************//
//Description : Function returns a pointer to the 1st element of the spi rx buffer
//
//Function prototype : uint8_t * SPI_get_rx_buffer (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel                   
//
//Exit params        : uint8_t * : pointer to 1st element location
//
//Function call      : uint8_t * = SPI_get_rx_buffer(SPI_2); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
uint8_t * SPI_get_rx_buffer (uint8_t channel)
{
    return(&spi_struct[channel].spi_rx_data[0]);// Return array pointer on 1st element
}

uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index)
{
    return spi_struct[channel].spi_rx_data[index];
}

//*********uint8_t SPI_rx_done (uint8_t channel)************//
//Description : Function check is SPI transfer is over, meaning the SPI bus is free 
//
//Function prototype : uint8_t SPI_rx_done (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel
//
//Exit params        : uint8_t 0 : Bus is BUSY
//                                   1 : Bus is FREE
//
//Function call      : uint8_t = SPI_rx_done(SPI_1); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
uint8_t SPI_txfer_done (uint8_t channel)
{
    if (spi_struct[channel].spi_txfer_done)
    {
        spi_struct[channel].spi_txfer_done = 0;
        return 1;
    }
    else return 0;
}

//************void SPI_clear_rx_buffer (uint8_t channel)****************//
//Description : Function clears the RX buffer of the specified SPI channel
//
//Function prototype : void SPI_clear_rx_buffer (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel
//
//Exit params        : None
//
//Function call      : SPI_clear_rx_buffer(SPI_1); 
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020
//****************************************************************************//
void SPI_flush_buffer (uint8_t channel)
{
    uint8_t i = 0;
    for (; i < SPI_BUF_LENGTH; i++)
    {
        spi_struct[channel].spi_rx_data[i] = 0;
        spi_struct[channel].spi_tx_data[i] = 0;
    }
}

//**************************SPI1 interrupt function***************************//
//Description : SPI interrupt.
//
//Function prototype : _SPI1Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : Called when interrupt happens
//
//Jean-Francois Bilodeau    MPLab X v5.45    03/08/2021
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI1Interrupt(void)
{
    IFS0bits.SPI1IF = 0;                   
    spi_struct[SPI_1].spi_tx_cnt++;
    if (spi_struct[SPI_1].spi_tx_cnt < spi_struct[SPI_1].spi_tx_length)
    {
        spi_struct[SPI_1].spi_rx_data[spi_struct[SPI_1].spi_rx_cnt] = SPI1BUF; 
        spi_struct[SPI_1].spi_rx_cnt++;                           
        SPI1BUF = spi_struct[SPI_1].spi_tx_data[spi_struct[SPI_1].spi_tx_cnt];    
    }
    
    else 
    {               
        spi_struct[SPI_1].spi_rx_data[spi_struct[SPI_1].spi_rx_cnt] = SPI1BUF;
        spi_struct[SPI_1].spi_rx_cnt = 0;        
        spi_struct[SPI_1].spi_tx_cnt = 0;           
        SPI_master_deassert_cs(spi_struct[SPI_1].spi_chip); 
        spi_struct[SPI_1].spi_txfer_done = 1;                
        IEC0bits.SPI1IE = 0;                       
    }
}

//**************************SPI2interrupt function***************************//
//Description : SPI interrupt.
//
//Function prototype : _SPI2Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : Called when interrupt happens
//
//Jean-Francois Bilodeau    MPLab X v5.45    03/08/2021
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI2Interrupt(void)
{
    IFS2bits.SPI2IF = 0;                   
    spi_struct[SPI_2].spi_tx_cnt++;
    if (spi_struct[SPI_2].spi_tx_cnt < spi_struct[SPI_2].spi_tx_length)
    {
        spi_struct[SPI_2].spi_rx_data[spi_struct[SPI_2].spi_rx_cnt] = SPI2BUF;  
        spi_struct[SPI_2].spi_rx_cnt++;                          
        SPI2BUF = spi_struct[SPI_2].spi_tx_data[spi_struct[SPI_2].spi_tx_cnt];     
    }
    
    else 
    {               
        spi_struct[SPI_2].spi_rx_data[spi_struct[SPI_2].spi_rx_cnt] = SPI2BUF;
        spi_struct[SPI_2].spi_rx_cnt = 0;           
        spi_struct[SPI_2].spi_tx_cnt = 0;         
        SPI_master_deassert_cs(spi_struct[SPI_2].spi_chip); 
        spi_struct[SPI_2].spi_txfer_done = 1;                 
        IEC2bits.SPI2IE = 0;                          
    }
}

//**************************SPI3 interrupt function***************************//
//Description : SPI interrupt.
//
//Function prototype : _SPI3Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : Called when interrupt happens
//
//Jean-Francois Bilodeau    MPLab X v5.45    03/08/2021
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI3Interrupt(void)
{
    IFS5bits.SPI3IF = 0;                  
    spi_struct[SPI_3].spi_tx_cnt++;
    if (spi_struct[SPI_3].spi_tx_cnt < spi_struct[SPI_3].spi_tx_length)
    {
        spi_struct[SPI_3].spi_rx_data[spi_struct[SPI_3].spi_rx_cnt] = SPI3BUF;  
        spi_struct[SPI_3].spi_rx_cnt++;                           
        SPI3BUF = spi_struct[SPI_3].spi_tx_data[spi_struct[SPI_3].spi_tx_cnt];  
    }
    
    else
    {               
        spi_struct[SPI_3].spi_rx_data[spi_struct[SPI_3].spi_rx_cnt] = SPI3BUF;
        spi_struct[SPI_3].spi_rx_cnt = 0;         
        spi_struct[SPI_3].spi_tx_cnt = 0;       
        SPI_master_deassert_cs(spi_struct[SPI_3].spi_chip);
        spi_struct[SPI_3].spi_txfer_done = 1;
        IEC5bits.SPI3IE = 0;
    }
}

//**************************SPI4 interrupt function***************************//
//Description : SPI interrupt.
//
//Function prototype : _SPI4Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : Called when interrupt happens
//
//Jean-Francois Bilodeau    MPLab X v5.45    03/08/2021
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI4Interrupt(void)
{
    IFS7bits.SPI4IF = 0;                 
    spi_struct[SPI_4].spi_tx_cnt++;
    if (spi_struct[SPI_4].spi_tx_cnt < spi_struct[SPI_4].spi_tx_length)
    {
        spi_struct[SPI_4].spi_rx_data[spi_struct[SPI_4].spi_rx_cnt] = SPI4BUF;
        spi_struct[SPI_4].spi_rx_cnt++;                   
        SPI4BUF = spi_struct[SPI_4].spi_tx_data[spi_struct[SPI_4].spi_tx_cnt];
    }
    
    else
    {               
        spi_struct[SPI_4].spi_rx_data[spi_struct[SPI_4].spi_rx_cnt] = SPI4BUF;
        spi_struct[SPI_4].spi_rx_cnt = 0;     
        spi_struct[SPI_4].spi_tx_cnt = 0;
        SPI_master_deassert_cs(spi_struct[SPI_4].spi_chip);
        spi_struct[SPI_4].spi_txfer_done = 1;       
        IEC7bits.SPI4IE = 0;      
    }
}