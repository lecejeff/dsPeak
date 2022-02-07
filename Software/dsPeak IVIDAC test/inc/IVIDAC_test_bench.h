#ifndef __IVIDAC_TEST_BENCH_H__
#define __IVIDAC_TEST_BENCH_H__

#include "dsPeak_generic.h"
#include "UART.h"
#include "i2c.h"
#include "io_expander.h"

// Define generic I2C address of the on-board I2C EEPROM
#define IVIDAC_I2C_EEPROM_ADR   0xA2

// Define generic I2C addresses of all 3x IO expanders
#define EXPANDER_BANK_0             0
#define EXPANDER_BANK_0_ADDRESS     0x40
#define EXPANDER_BANK_1             1
#define EXPANDER_BANK_1_ADDRESS     0x42

#define IVIDAC_TB_OE_PIN            LATEbits.LATE7
#define IVIDAC_TB_RESET_PIN         LATEbits.LATE8  

// -------------------- IVIDAC debug frames definition -----------------------//
// Decoding of the frame.
// Frame is 6 bytes long : 1 ADR byte, 1x CMD byte, 3x DATA bytes and 1x checksum byte
#define ADR_OFFSET      0
#define COMMAND_OFFSET  1
#define DATA_OFFSET     2
#define DATA0_OFFSET    2
#define DATA1_OFFSET    3
#define DATA2_OFFSET    4
#define DATA_LENGTH     3
#define CHECKSUM_OFFSET 5

// Definition of the commands
#define CMD_READ_BIT_MASK               0x80

#define CMD_DAC_ZERO_CODE               0
#define CMD_DAC_MAX_CODE                1
#define CMD_DAC_CAL_LOW_CODE            2
#define CMD_DAC_CAL_HIGH_CODE           3
#define CMD_DAC_CUSTOM_CODE             4
#define CMD_DAC_OE_STATE                5

#define CMD_TB_SGL_RELAY_BANK0_STATE    16
#define CMD_TB_SGL_RELAY_BANK1_STATE    17
#define CMD_TB_MTL_RELAY_BANK0_STATE    18
#define CMD_TB_MTL_RELAY_BANK1_STATE    19

#define CMD_TB_RELAY_SET_ALL            20
#define CMD_TB_RELAY_RESET_ALL          21

#define CMD_TB_RESET_VARIABLES          100

#define CMD_TB_I2C_EEPROM_INIT          125
#define CMD_TB_I2C_EEPROM_SCAN          126
#define CMD_TB_I2C_EEPROM_DISCOVERED    127

// Definition of relay banks circuitry
#define EXPANDER_output_reset_value     0xFFFF

typedef struct
{
    uint8_t IVIDAC_message_buf[6];
    uint8_t msg_flag;
    uint8_t rx_checksum;
    uint8_t prev_rx_checksum;
    uint8_t tx_checksum;
}STRUCT_IVIDAC_MESSAGE;

void IVIDAC_bench_com_init (STRUCT_IVIDAC_MESSAGE *msg);
uint8_t IVIDAC_parse_uart_rx_buffer (STRUCT_IVIDAC_MESSAGE *msg, uint8_t * rx_buf);
uint8_t IVIDAC_return_rx_checksum (STRUCT_IVIDAC_MESSAGE *msg);

#endif