//***************************************************************************//
// File      : spi.h
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
    uint8_t spi_chip;
    uint8_t spi_tx_data[SPI_BUF_LENGTH];
    uint8_t spi_rx_data[SPI_BUF_LENGTH];
    uint8_t spi_tx_length;
    uint8_t spi_state;
    uint8_t spi_txfer_done;
    uint8_t spi_rx_cnt;
    uint8_t spi_tx_cnt;
}SPI_struct;

void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre); 
void SPI_master_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip);
uint8_t SPI_txfer_done (uint8_t channel);
uint8_t * SPI_get_rx_buffer (uint8_t channel);
uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index);
void SPI_master_deassert_cs (uint8_t chip);
void SPI_master_assert_cs (uint8_t chip);
void SPI_flush_buffer (uint8_t channel);
#endif

