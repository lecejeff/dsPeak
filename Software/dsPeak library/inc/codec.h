//***************************************************************************//
// File      : codec.h
//
// About     : dsPeak features an on-board audio in/out codec based on NXPs
//             SGTL5000 integrated circuit. It has an audio in line, a micro in,
//             an audio out and a speaker out. In addition, this codec has an
//             integrated equalizer / bass-tone enhancer and many other features
//             such as programmable mic gain, volume control, filtering and
//             automatic volume control.
//
// Functions : 
//
// Includes  : general.h
//           
// Purpose   : Driver for the dsPeak SGTL5000 audio codec using DCI as audio
//             interface and SPI as configuration interface 
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#ifndef __CODEC_H_
#define	__CODEC_H_

#include "general.h"
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

// CODEC volume defines
#define DAC_VOLUME_

#define SYS_FS_32kHz        0
#define SYS_FS_44_1kHz      1
#define SYS_FS_48kHz        2
#define SYS_FS_96kHz        3

#define ADC_MUTE            0
#define HEADPHONE_MUTE      1
#define LINEOUT_MUTE        2
#define DAC_MUTE            3

#define SYS_MCLK    12000000    // SYS_MCLK is 12MHz

typedef struct
{
    unsigned int CHIP_DIG_POWER;
    unsigned int CHIP_CLK_CTRL;
    unsigned int CHIP_I2S_CTRL;
    unsigned int CHIP_SSS_CTRL;
    unsigned int CHIP_ADCDAC_CTRL;
    unsigned int CHIP_DAC_VOL;
    unsigned int CHIP_PAD_STRENGTH;
    unsigned int CHIP_ANA_ADC_CTRL;
    unsigned int CHIP_ANA_HP_CTRL;
    unsigned int CHIP_ANA_CTRL;
    unsigned int CHIP_LINREG_CTRL;
    unsigned int CHIP_REF_CTRL;
    unsigned int CHIP_MIC_CTRL;
    unsigned int CHIP_LINE_OUT_CTRL;
    unsigned int CHIP_LINE_OUT_VOL;
    unsigned int CHIP_ANA_POWER;
    unsigned int CHIP_PLL_CTRL;
    unsigned int CHIP_CLK_TOP_CTRL;
    unsigned int CHIP_ANA_STATUS;
    unsigned int CHIP_ANA_TEST1;
    unsigned int CHIP_ANA_TEST2;
    unsigned int CHIP_SHORT_CTRL;
    unsigned int DAP_CONTROL;
    unsigned int DAP_PEQ;
    unsigned int DAP_BASS_ENHANCE;
    unsigned int DAP_BASS_ENHANCE_CTRL;
    unsigned int DAP_AUDIO_EQ;
    unsigned int DAP_SGTL_SURROUND;
    unsigned int DAP_FILTER_COEF_ACCESS;
    unsigned int DAP_COEF_WR_B0_MSB;
    unsigned int DAP_COEF_WR_B0_LSB;
    unsigned int DAP_AUDIO_EQ_BASS_BAND0;
    unsigned int DAP_AUDIO_EQ_BAND1;
    unsigned int DAP_AUDIO_EQ_BAND2;
    unsigned int DAP_AUDIO_EQ_BAND3;
    unsigned int DAP_AUDIO_EQ_TREBLE_BAND4;
    unsigned int DAP_MAIN_CHAN;
    unsigned int DAP_MIX_CHAN;
    unsigned int DAP_AVC_CTRL;
    unsigned int DAP_AVC_THRESHOLD;
    unsigned int DAP_AVC_ATTACK;
    unsigned int DAP_AVC_DECAY;
    unsigned int DAP_COEF_WR_B1_MSB;
    unsigned int DAP_COEF_WR_B1_LSB;
    unsigned int DAP_COEF_WR_B2_MSB;
    unsigned int DAP_COEF_WR_B2_LSB;
    unsigned int DAP_COEF_WR_A1_MSB;
    unsigned int DAP_COEF_WR_A1_LSB;
    unsigned int DAP_COEF_WR_A2_MSB;
    unsigned int DAP_COEF_WR_A2_LSB; 
    unsigned char dac_vol_left;
    unsigned char dac_vol_right;
}STRUCT_CODEC;

void DCI_init (void);
void CODEC_init (unsigned char sys_fs);
unsigned int CODEC_spi_write (unsigned int adr, unsigned int data) ;
unsigned int CODEC_spi_modify_write (unsigned int adr, unsigned int reg, unsigned int mask, unsigned int data);
void CODEC_mute (unsigned char channel);
void CODEC_unmute (unsigned char channel);
void CODEC_mic_config (unsigned char bias_res, unsigned char bias_volt, unsigned char gain);
void CODEC_set_input_route (unsigned char channel);
void CODEC_set_output_route (unsigned char channel);
void CODEC_set_dac_volume (unsigned char dac_vol_right, unsigned char dac_vol_left);
#endif	

