//***************************************************************************//
// File      : spi.h
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
// Includes  :  general.h
//           
// Purpose   :  Driver for the dsPIC33EP SPI peripheral
//              2 seperate SPI channel on Intellitrol
//              SPI_1 - DRV8873 control port, SPI master between dsPIC and peripheral
//              SPI_2 - Intellitrol control port, SPI slave between dsPIC and external master
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#ifndef __spi_h_
#define __spi_h_

#include "general.h"

//******************************************************************************
// Definition of SPI modules
#define SPI_1 0
#define SPI_2 1
#define SPI_3 2
#define SPI_4 3
#define SPI_QTY 4

//******************************************************************************
// Various SPI chip select defines
#define FT8XX_EVE_CS        1
#define SD_CARD_CS          2
#define FLASH_MEMORY_CS     3
#define AUDIO_CODEC_CS      4
#define MIKROBUS1_CS        5
#define MIKROBUS2_CS        6

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define SPI_BUF_LENGTH 32

//******************************************************************************
// SPI CS pin assignation for assert / deassert functions
#define FT8XX_EVE_CS_PIN    LATBbits.LATB11
#define SD_CARD_CS_PIN      LATGbits.LATG9
#define FLASH_MEMORY_CS_PIN LATGbits.LATG0
#define AUDIO_CODEC_CS_PIN  LATDbits.LATD6
#define MIKROBUS1_CS_PIN    LATHbits.LATH15
#define MIKROBUS2_CS_PIN    LATHbits.LATH13

typedef struct
{
    unsigned char spi_chip;
    unsigned char spi_tx_data[SPI_BUF_LENGTH];
    unsigned char spi_rx_data[SPI_BUF_LENGTH];
    unsigned char spi_tx_length;
    unsigned char spi_state;
    unsigned char spi_txfer_done;
    unsigned char spi_rx_cnt;
    unsigned char spi_tx_cnt;
}SPI_struct;

void SPI_init (unsigned char channel, unsigned char mode, unsigned char ppre, unsigned char spre); 
void SPI_master_write (unsigned char channel, unsigned char *data, unsigned char length, unsigned char chip);
unsigned char SPI_txfer_done (unsigned char channel);
unsigned char * SPI_get_rx_buffer (unsigned char channel);
unsigned char SPI_get_rx_buffer_index (unsigned char channel, unsigned char index);
void SPI_master_deassert_cs (unsigned char chip);
void SPI_master_assert_cs (unsigned char chip);
void SPI_flush_buffer (unsigned char channel);
#endif

