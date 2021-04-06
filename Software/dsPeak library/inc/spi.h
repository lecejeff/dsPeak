//***************************************************************************//
// File      : spi.h
//
// Functions :  void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre); 
//              void SPI_master_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip);
//              uint8_t SPI_txfer_done (uint8_t channel);
//              uint8_t * SPI_get_rx_buffer (uint8_t channel);
//              uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index);
//              void SPI_master_deassert_cs (uint8_t chip);
//              void SPI_master_assert_cs (uint8_t chip);
//              void SPI_flush_txbuffer (uint8_t channel);
//              void SPI_flush_rxbuffer (uint8_t channel);
//              uint8_t SPI_module_busy (uint8_t channel);
//
// Includes  :  general.h
//           
// Purpose   :  Driver for the dsPIC33E SPI peripheral
//              4x seperate SPI channels on dsPeak :
//              SPI_1 : Riverdi EVE embedded video engine
//              SPI_2 : Flash / uSD Card 
//              SPI_3 : Audio CODEC
//              SPI_4 : MikroBus 1 and 2
//
// Intellitrol                   MPLab X v5.45                        13/01/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
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

#define SPI_TX_IDLE         0
#define SPI_TX_COMPLETE     1

#define SPI_MODULE_FREE     0
#define SPI_MODULE_BUSY     1
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
}STRUCT_SPI;

void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre); 
void SPI_master_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip);
uint8_t SPI_txfer_done (uint8_t channel);
uint8_t * SPI_get_rx_buffer (uint8_t channel);
uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index);
void SPI_master_deassert_cs (uint8_t chip);
void SPI_master_assert_cs (uint8_t chip);
void SPI_flush_txbuffer (uint8_t channel);
void SPI_flush_rxbuffer (uint8_t channel);
uint8_t SPI_module_busy (uint8_t channel);
#endif

