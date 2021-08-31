//***************************************************************************//
// File      : codec.h
//
// About     : dsPeak features an on-board audio in/out codec based on NXPs
//             SGTL5000 integrated circuit. It has an audio in line, a micro in,
//             an audio out and a speaker out. In addition, this codec has an
//             integrated equalizer / bass-tone enhancer and many other features
//             such as programmable mic gain / volume control, filtering and
//             automatic volume control.
//
// Functions : 
//
// Includes  : dspeak_generic.h
//           
// Purpose   : Driver for the dsPeak SGTL5000 audio codec using DCI as audio
//             interface and SPI as configuration interface 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
//****************************************************************************//

#ifndef __CODEC_H_
#define	__CODEC_H_

#include "dspeak_generic.h"
#include "spi.h"

#define CODEC_CHIP_ID                   0x0000
#define CODEC_CHIP_DIG_POWER            0x0002
#define CODEC_CHIP_CLK_CTRL             0x0004
#define CODEC_CHIP_I2S_CTRL             0x0006
#define CODEC_CHIP_SSS_CTRL             0x000A
#define CODEC_CHIP_ADCDAC_CTRL          0x000E
#define CODEC_CHIP_DAC_VOL              0x0010
#define CODEC_CHIP_PAD_STRENGTH         0x0014
#define CODEC_CHIP_ANA_ADC_CTRL         0x0020
#define CODEC_CHIP_ANA_HP_CTRL          0x0022
#define CODEC_CHIP_ANA_CTRL             0x0024
#define CODEC_CHIP_LINREG_CTRL          0x0026
#define CODEC_CHIP_REF_CTRL             0x0028
#define CODEC_CHIP_MIC_CTRL             0x002A
#define CODEC_CHIP_LINE_OUT_CTRL        0x002C
#define CODEC_CHIP_LINE_OUT_VOL         0x002E
#define CODEC_CHIP_ANA_POWER            0x0030
#define CODEC_CHIP_PLL_CTRL             0x0032
#define CODEC_CHIP_CLK_TOP_CTRL         0x0034
#define CODEC_CHIP_ANA_STATUS           0x0036
#define CODEC_CHIP_ANA_TEST1            0x0038
#define CODEC_CHIP_ANA_TEST2            0x003A
#define CODEC_CHIP_SHORT_CTRL           0x003C
#define CODEC_DAP_CONTROL               0x0100
#define CODEC_DAP_PEQ                   0x0102
#define CODEC_DAP_BASS_ENHANCE          0x0104
#define CODEC_DAP_BASS_ENHANCE_CTRL     0x0106
#define CODEC_DAP_AUDIO_EQ              0x0108
#define CODEC_DAP_SGTL_SURROUND         0x010A
#define CODEC_DAP_FILTER_COEF_ACCESS    0x010C
#define CODEC_DAP_COEF_WR_B0_MSB        0x010E
#define CODEC_DAP_COEF_WR_B0_LSB        0x0110
#define CODEC_DAP_AUDIO_EQ_BASS_BAND0   0x0116
#define CODEC_DAP_AUDIO_EQ_BAND1        0x0118
#define CODEC_DAP_AUDIO_EQ_BAND2        0x011A
#define CODEC_DAP_AUDIO_EQ_BAND3        0x011C
#define CODEC_DAP_AUDIO_EQ_TREBLE_BAND4 0x011E
#define CODEC_DAP_MAIN_CHAN             0x0120
#define CODEC_DAP_MIX_CHAN              0x0122
#define CODEC_DAP_AVC_CTRL              0x0124
#define CODEC_DAP_AVC_THRESHOLD         0x0126
#define CODEC_DAP_AVC_ATTACK            0x0128
#define CODEC_DAP_AVC_DECAY             0x012A
#define CODEC_DAP_COEF_WR_B1_MSB        0x012C
#define CODEC_DAP_COEF_WR_B1_LSB        0x012E
#define CODEC_DAP_COEF_WR_B2_MSB        0x0130
#define CODEC_DAP_COEF_WR_B2_LSB        0x0132
#define CODEC_DAP_COEF_WR_A1_MSB        0x0134
#define CODEC_DAP_COEF_WR_A1_LSB        0x0136
#define CODEC_DAP_COEF_WR_A2_MSB        0x0138
#define CODEC_DAP_COEF_WR_A2_LSB        0x013A

// Microphone specific defines
#define MIC_BIAS_RES_OFF    0   // Microphone bias output impedance, match with     
#define MIC_BIAS_RES_2k     1   // microphone output impedance
#define MIC_BIAS_RES_4k     2   //
#define MIC_BIAS_RES_8k     3   //

#define MIC_BIAS_VOLT_1V25  0   // Microphone bias voltage, should be under
#define MIC_BIAS_VOLT_1V50  1   // VDDA - 0.2V for adequate PSRR
#define MIC_BIAS_VOLT_1V75  2   //
#define MIC_BIAS_VOLT_2V00  3   //
#define MIC_BIAS_VOLT_2V25  4   //
#define MIC_BIAS_VOLT_2V50  5   //
#define MIC_BIAS_VOLT_2V75  6   //
#define MIC_BIAS_VOLT_3V00  7   //

#define MIC_GAIN_0dB        0   // Microphone amplifier gain, setting a gain 
#define MIC_GAIN_20dB       1   // different than 0dB decreases THD
#define MIC_GAIN_30dB       2   //
#define MIC_GAIN_40dB       3   //
//----------------------------

#define CODEC_INPUT_LINEIN  0
#define CODEC_INPUT_I2S     1
#define CODEC_INPUT_MIC
#define CODEC_INPUT_

// CODEC volume defines
#define ADC_VOL_RANGE_DEFAULT   0
#define ADC_VOL_RANGE_RED6DB    1

#define SYS_FS_32kHz        0
#define SYS_FS_44_1kHz      1
#define SYS_FS_48kHz        2
#define SYS_FS_96kHz        3
#define SYS_FS_8kHz         4
#define SYS_FS_11_025kHz    5
#define SYS_FS_12kHz        6
#define SYS_FS_16kHz        7
#define SYS_FS_22_05kHz     8
#define SYS_FS_24kHz        9

#define ADC_MUTE            0
#define HEADPHONE_MUTE      1
#define LINEOUT_MUTE        2
#define DAC_MUTE            3

#define SYS_MCLK    12000000    // SYS_MCLK is 12MHz

// I2C BLOCK TRANSFER define
// A single transfer is 2x words (4x bytes), one per channel (left and right)
#define CODEC_BLOCK_TRANSFER 256    

typedef struct
{
    uint16_t CHIP_DIG_POWER;
    uint16_t CHIP_CLK_CTRL;
    uint16_t CHIP_I2S_CTRL;
    uint16_t CHIP_SSS_CTRL;
    uint16_t CHIP_ADCDAC_CTRL;
    uint16_t CHIP_DAC_VOL;
    uint16_t CHIP_PAD_STRENGTH;
    uint16_t CHIP_ANA_ADC_CTRL;
    uint16_t CHIP_ANA_HP_CTRL;
    uint16_t CHIP_ANA_CTRL;
    uint16_t CHIP_LINREG_CTRL;
    uint16_t CHIP_REF_CTRL;
    uint16_t CHIP_MIC_CTRL;
    uint16_t CHIP_LINE_OUT_CTRL;
    uint16_t CHIP_LINE_OUT_VOL;
    uint16_t CHIP_ANA_POWER;
    uint16_t CHIP_PLL_CTRL;
    uint16_t CHIP_CLK_TOP_CTRL;
    uint16_t CHIP_ANA_STATUS;
    uint16_t CHIP_ANA_TEST1;
    uint16_t CHIP_ANA_TEST2;
    uint16_t CHIP_SHORT_CTRL;
    uint16_t DAP_CONTROL;
    uint16_t DAP_PEQ;
    uint16_t DAP_BASS_ENHANCE;
    uint16_t DAP_BASS_ENHANCE_CTRL;
    uint16_t DAP_AUDIO_EQ;
    uint16_t DAP_SGTL_SURROUND;
    uint16_t DAP_FILTER_COEF_ACCESS;
    uint16_t DAP_COEF_WR_B0_MSB;
    uint16_t DAP_COEF_WR_B0_LSB;
    uint16_t DAP_AUDIO_EQ_BASS_BAND0;
    uint16_t DAP_AUDIO_EQ_BAND1;
    uint16_t DAP_AUDIO_EQ_BAND2;
    uint16_t DAP_AUDIO_EQ_BAND3;
    uint16_t DAP_AUDIO_EQ_TREBLE_BAND4;
    uint16_t DAP_MAIN_CHAN;
    uint16_t DAP_MIX_CHAN;
    uint16_t DAP_AVC_CTRL;
    uint16_t DAP_AVC_THRESHOLD;
    uint16_t DAP_AVC_ATTACK;
    uint16_t DAP_AVC_DECAY;
    uint16_t DAP_COEF_WR_B1_MSB;
    uint16_t DAP_COEF_WR_B1_LSB;
    uint16_t DAP_COEF_WR_B2_MSB;
    uint16_t DAP_COEF_WR_B2_LSB;
    uint16_t DAP_COEF_WR_A1_MSB;
    uint16_t DAP_COEF_WR_A1_LSB;
    uint16_t DAP_COEF_WR_A2_MSB;
    uint16_t DAP_COEF_WR_A2_LSB; 
    uint8_t dac_vol_left;
    uint8_t dac_vol_right;
    uint8_t hp_vol_left;
    uint8_t hp_vol_right;
    
    uint16_t DCI_receive_buffer[CODEC_BLOCK_TRANSFER];
    uint16_t DCI_transmit_buffer[CODEC_BLOCK_TRANSFER];
    uint16_t DCI_transmit_counter;
    uint16_t DCI_receive_counter;
    uint8_t DCI_transmit_enable;
}STRUCT_CODEC;

void DCI_init (void);
void CODEC_init (uint8_t sys_fs);
uint16_t CODEC_spi_write (uint16_t adr, uint16_t data);
uint16_t CODEC_spi_modify_write (uint16_t adr, uint16_t reg, uint16_t mask, uint16_t data);
void CODEC_mute (uint8_t channel);
void CODEC_unmute (uint8_t channel);
void CODEC_mic_config (uint8_t bias_res, uint8_t bias_volt, uint8_t gain);
void CODEC_set_input_route (uint8_t in_channel, uint8_t out_channel, uint8_t gain);
void CODEC_set_output_route (uint8_t in_channel, uint8_t out_channel, uint8_t gain);
void CODEC_set_mic_gain (uint8_t gain);
void CODEC_set_analog_gain (uint8_t range, uint8_t gain_right, uint8_t gain_left);
void CODEC_set_dac_volume (uint8_t dac_vol_right, uint8_t dac_vol_left);
void CODEC_set_hp_volume (uint8_t hp_vol_right, uint8_t hp_vol_left);
uint8_t DCI_get_interrupt_state (void);
#endif	

