//***************************************************************************//
// File      : codec.c
//
// Functions : 
//
// Includes  : codec.h
//           
// Purpose   : Driver for the dsPeak SGTL5000 audio codec using DCI as audio
//             interface and SPI as configuration interface 
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#include "codec.h"
#include "spi.h"

STRUCT_CODEC CODEC_struct; 

const int sine_wave_255_65k[256] = {
0x8000,0x8327,0x864e,0x8973,0x8c98,0x8fba,0x92da,0x95f7,
0x9911,0x9c27,0x9f38,0xa244,0xa54c,0xa84d,0xab48,0xae3c,
0xb12a,0xb40f,0xb6ed,0xb9c2,0xbc8e,0xbf50,0xc209,0xc4b7,
0xc75b,0xc9f4,0xcc81,0xcf02,0xd177,0xd3e0,0xd63b,0xd889,
0xdac9,0xdcfb,0xdf1e,0xe133,0xe339,0xe52f,0xe716,0xe8ec,
0xeab3,0xec68,0xee0d,0xefa1,0xf123,0xf294,0xf3f3,0xf540,
0xf67a,0xf7a3,0xf8b8,0xf9bb,0xfaab,0xfb88,0xfc52,0xfd08,
0xfdab,0xfe3b,0xfeb7,0xff1f,0xff73,0xffb4,0xffe1,0xfff9,
0xfffe,0xffef,0xffcd,0xff96,0xff4b,0xfeed,0xfe7b,0xfdf6,
0xfd5c,0xfcb0,0xfbef,0xfb1c,0xfa36,0xf93c,0xf830,0xf711,
0xf5df,0xf49b,0xf345,0xf1de,0xf064,0xeed9,0xed3d,0xeb8f,
0xe9d2,0xe803,0xe625,0xe436,0xe238,0xe02b,0xde0f,0xdbe4,
0xd9ab,0xd763,0xd50f,0xd2ad,0xd03e,0xcdc3,0xcb3c,0xc8a9,
0xc60b,0xc361,0xc0ae,0xbdf0,0xbb29,0xb858,0xb57f,0xb29d,
0xafb4,0xacc3,0xa9cb,0xa6cd,0xa3c9,0xa0bf,0x9db0,0x9a9c,
0x9785,0x9469,0x914b,0x8e29,0x8b06,0x87e1,0x84ba,0x8193,
0x7e6c,0x7b45,0x781e,0x74f9,0x71d6,0x6eb4,0x6b96,0x687a,
0x6563,0x624f,0x5f40,0x5c36,0x5932,0x5634,0x533c,0x504b,
0x4d62,0x4a80,0x47a7,0x44d6,0x420f,0x3f51,0x3c9e,0x39f4,
0x3756,0x34c3,0x323c,0x2fc1,0x2d52,0x2af0,0x289c,0x2654,
0x241b,0x21f0,0x1fd4,0x1dc7,0x1bc9,0x19da,0x17fc,0x162d,
0x1470,0x12c2,0x1126,0xf9b,0xe21,0xcba,0xb64,0xa20,
0x8ee,0x7cf,0x6c3,0x5c9,0x4e3,0x410,0x34f,0x2a3,
0x209,0x184,0x112,0xb4,0x69,0x32,0x10,0x1,
0x6,0x1e,0x4b,0x8c,0xe0,0x148,0x1c4,0x254,
0x2f7,0x3ad,0x477,0x554,0x644,0x747,0x85c,0x985,
0xabf,0xc0c,0xd6b,0xedc,0x105e,0x11f2,0x1397,0x154c,
0x1713,0x18e9,0x1ad0,0x1cc6,0x1ecc,0x20e1,0x2304,0x2536,
0x2776,0x29c4,0x2c1f,0x2e88,0x30fd,0x337e,0x360b,0x38a4,
0x3b48,0x3df6,0x40af,0x4371,0x463d,0x4912,0x4bf0,0x4ed5,
0x51c3,0x54b7,0x57b2,0x5ab3,0x5dbb,0x60c7,0x63d8,0x66ee,
0x6a08,0x6d25,0x7045,0x7367,0x768c,0x79b1,0x7cd8,0x8000};

void CODEC_init (unsigned char sys_fs)
{
    unsigned long pll_out_freq = 0;
    unsigned int pll_int_divisor = 0;
    unsigned int pll_frac_divisor = 0;
    
    SPI_init(SPI_3, SPI_MODE0, 2, 0);   // PPRE = 3, primary prescale 1:4
                                        // SPRE = 0, Secondary prescale 1:8
                                        // Fspi = FCY / 32 = 2.175MHz
        
    // Since SPI read is not supported, write default reset values to struct
    // registers to support the modify operation. Reset values taken from datasheet
    CODEC_struct.CHIP_DIG_POWER = 0;        // Every audio subsystem is disabled
    CODEC_struct.CHIP_CLK_CTRL = 0x0008;    // SYS_FS = 48kHz               
    CODEC_struct.CHIP_I2S_CTRL = 0x0010;    // DLEN = 24b
    CODEC_struct.CHIP_SSS_CTRL = 0x0010;    // DAC_SELECT = I2S_IN
    CODEC_struct.CHIP_ADCDAC_CTRL = 0x020C; // VOL_RAMP ENABLED, DAC L/R MUTED
    CODEC_struct.CHIP_DAC_VOL = 0x3C3C;     // DAC VOL L/R = 0dB
    CODEC_struct.CHIP_PAD_STRENGTH = 0x015F;// I2S PAD STRENGTH = 0x1 = 4.02mA, I2C PAD STRENGTH = 0x3 = 12.05mA
    CODEC_struct.CHIP_ANA_ADC_CTRL = 0;     // No change in ADC range
    CODEC_struct.CHIP_ANA_HP_CTRL = 0x1818; // HP VOL L/R = 0dB
    CODEC_struct.CHIP_ANA_CTRL = 0x0111;    // LineOut = Mute, HP = Mute, ADC = Mute
    CODEC_struct.CHIP_LINREG_CTRL = 0;      //
    CODEC_struct.CHIP_REF_CTRL = 0;         //
    CODEC_struct.CHIP_MIC_CTRL = 0;         //
    CODEC_struct.CHIP_LINE_OUT_CTRL = 0;    //
    CODEC_struct.CHIP_LINE_OUT_VOL = 0x0404;// LineOut output level, refer to equation in datasheet
    CODEC_struct.CHIP_ANA_POWER = 0x7060;   // DAC Stereo, Power-up linreg, ADC stereo, Power-up ref bias
    CODEC_struct.CHIP_PLL_CTRL = 0x5000;    // PLL INT and FRAC divisors, refer to equation in datasheet
    CODEC_struct.CHIP_CLK_TOP_CTRL = 0;     //
    CODEC_struct.CHIP_ANA_STATUS = 0;       //
    CODEC_struct.CHIP_ANA_TEST1 = 0x01C0;   // HP = ClassAB, VGND center for best antipop performance
    CODEC_struct.CHIP_ANA_TEST2 = 0;        //
    CODEC_struct.CHIP_SHORT_CTRL = 0;       //
    CODEC_struct.DAP_CONTROL = 0;           //
    CODEC_struct.DAP_PEQ = 0;               //
    CODEC_struct.DAP_BASS_ENHANCE = 0x0040; // Bass enhance cutoff frequency = 175Hz
    CODEC_struct.DAP_BASS_ENHANCE_CTRL = 0x051F;    
    CODEC_struct.DAP_AUDIO_EQ = 0;          //
    CODEC_struct.DAP_SGTL_SURROUND = 0x0040;// Mid width perception change
    CODEC_struct.DAP_FILTER_COEF_ACCESS = 0;//
    CODEC_struct.DAP_COEF_WR_B0_MSB = 0;    //
    CODEC_struct.DAP_COEF_WR_B0_LSB = 0;    //
    CODEC_struct.DAP_AUDIO_EQ_BASS_BAND0 = 0x002F;  // Bass/GEG Band 0 to 0dB
    CODEC_struct.DAP_AUDIO_EQ_BAND1 = 0x002F;       // GEQ Band1 to 0dB
    CODEC_struct.DAP_AUDIO_EQ_BAND2 = 0x002F;       // GEQ Band2 to 0dB
    CODEC_struct.DAP_AUDIO_EQ_BAND3 = 0x002F;       // GEQ Band3 to 0dB
    CODEC_struct.DAP_AUDIO_EQ_TREBLE_BAND4 = 0x002F;// Tone / Treble GEQ4 to 0dB
    CODEC_struct.DAP_MAIN_CHAN = 0x8000;    // DAP main channel volume = 100%
    CODEC_struct.DAP_MIX_CHAN = 0;          // DAP Mix channel volume = 100%
    CODEC_struct.DAP_AVC_CTRL = 0x5100;     // AVC 6dB gain, LBI 25ms response
    CODEC_struct.DAP_AVC_THRESHOLD = 0x1473;// Threshold set to -12dB
    CODEC_struct.DAP_AVC_ATTACK = 0x0028;   // Attack rate = 32dB/s
    CODEC_struct.DAP_AVC_DECAY = 0x0050;    // DEcay rate = 4dB/s
    CODEC_struct.DAP_COEF_WR_B1_MSB = 0;    //
    CODEC_struct.DAP_COEF_WR_B1_LSB = 0;    //
    CODEC_struct.DAP_COEF_WR_B2_MSB = 0;    //
    CODEC_struct.DAP_COEF_WR_B2_LSB = 0;    //
    CODEC_struct.DAP_COEF_WR_A1_MSB = 0;    //
    CODEC_struct.DAP_COEF_WR_A1_LSB = 0;    //
    CODEC_struct.DAP_COEF_WR_A2_MSB = 0;    //
    CODEC_struct.DAP_COEF_WR_A2_LSB = 0;    //
    
    // Following the datasheet recommendations for initialization sequence   
    // Codec power supply configuration, hardware implementation-dependent
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_write(CODEC_CHIP_ANA_POWER, 0x4060);        // Turnoff startup power supplies to save power
    __delay_ms(100);
    CODEC_struct.CHIP_LINREG_CTRL = CODEC_spi_write(CODEC_CHIP_LINREG_CTRL, 0x0060);    // Configure charge pump to use the VDDIO rail
    CODEC_struct.CHIP_REF_CTRL = CODEC_spi_write(CODEC_CHIP_REF_CTRL, 0x01F0);          // VAG -> 1.575V, BIAS Nominal, Normal VAG ramp
    CODEC_struct.CHIP_LINE_OUT_CTRL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_CTRL, 0x0322);// LineOut bias -> 360uA, LineOut VAG -> 1.65V
    CODEC_struct.CHIP_SHORT_CTRL = CODEC_spi_write(CODEC_CHIP_SHORT_CTRL, 0x4446);      // Enable HP short detect, set trip to 125mA
    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_CTRL, 0x0137);          // Enable Zero-cross detection SEE IF USEFUL OR NOT *****
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_write(CODEC_CHIP_ANA_POWER, 0x40FF);        // Power up LineOUt, HP & capless mode, ADC, DAC
    CODEC_struct.CHIP_DIG_POWER = CODEC_spi_write(CODEC_CHIP_DIG_POWER, 0x0073);        // Power UP I2S, DAP, DAC and ADC
    
    // PLL output frequency is based on the sample clock rate used
    if (sys_fs == SYS_FS_44_1kHz)
    {
        pll_out_freq = 180633600;
    }
    
    else
    {
        pll_out_freq = 196608000;
    }
    pll_int_divisor = (unsigned int)(pll_out_freq / SYS_MCLK);
    pll_frac_divisor = (unsigned int)(((pll_out_freq / SYS_MCLK)-pll_int_divisor)*2048);
    
    // Write PLL dividers to CODEC
    CODEC_struct.CHIP_PLL_CTRL = CODEC_spi_modify_write(CODEC_CHIP_PLL_CTRL, CODEC_struct.CHIP_PLL_CTRL, 0x07FF, pll_int_divisor<<11);
    CODEC_struct.CHIP_PLL_CTRL = CODEC_spi_modify_write(CODEC_CHIP_PLL_CTRL, CODEC_struct.CHIP_PLL_CTRL, 0xFC00, pll_frac_divisor);

    // Codec PLL configuration
    // Power-up the PLL
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xFBFF, 1<<10);
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xFEFF, 1<<8);
    
    // Codec System MCLK and Sample Clock
    CODEC_struct.CHIP_CLK_CTRL = CODEC_spi_modify_write(CODEC_CHIP_CLK_CTRL, CODEC_struct.CHIP_CLK_CTRL, 0xFFF3, sys_fs<<2);
    CODEC_struct.CHIP_CLK_CTRL = CODEC_spi_modify_write(CODEC_CHIP_CLK_CTRL, CODEC_struct.CHIP_CLK_CTRL, 0xFFFC, 3);
    
    // Set CODEC to I2S master, set data length to 16 bits
    CODEC_struct.CHIP_I2S_CTRL = CODEC_spi_modify_write(CODEC_CHIP_I2S_CTRL, CODEC_struct.CHIP_I2S_CTRL, 0xFF7F, 1 << 7);
    CODEC_struct.CHIP_I2S_CTRL = CODEC_spi_modify_write(CODEC_CHIP_I2S_CTRL, CODEC_struct.CHIP_I2S_CTRL, 0xFFCF, 3 << 4);

    // Set ADC and DAC to stereo
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xBFFF, 1<<14);
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xFFBF, 1<<6);    
    
    // Route MicIn to ADC
//    CODEC_mic_config(MIC_BIAS_RES_2k, MIC_BIAS_VOLT_2V00, MIC_GAIN_30dB);
//    CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   // DAC data source is ADC
//    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFB, 0 << 2);   // ADC input is microphone 
//    // Route Dac to HP Out
//     CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
//     CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818);
//     CODEC_unmute(ADC_MUTE);
//     CODEC_unmute(DAC_MUTE);
//     CODEC_unmute(HEADPHONE_MUTE);

    // ROute DAC to Line Out
//    CODEC_struct.CHIP_LINE_OUT_CTRL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_CTRL, 0x0322);   // Out current = 360uA, LO_VAG = 1.65V
//    CODEC_struct.CHIP_LINE_OUT_VOL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_VOL, 0x0F0F);      // Recommended nominal for VDDIO = 3.3
//    CODEC_unmute(ADC_MUTE);
//    CODEC_unmute(DAC_MUTE);
//    CODEC_unmute(HEADPHONE_MUTE);    
       
    // Route LineIn to ADC
    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFB, 1 << 2);
    CODEC_struct.CHIP_ANA_ADC_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_ADC_CTRL, 0x0000);  // Gain of 0dB on ADC
    // Route ADC to DAC
    CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   // DAC data source is ADC
    CODEC_struct.CHIP_DAC_VOL = CODEC_spi_write(CODEC_CHIP_DAC_VOL, 0x3C3C);    // 0dB gain on DAC
    // Route DAC output to HP
    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
    CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818); 
    CODEC_unmute(ADC_MUTE);
    CODEC_unmute(DAC_MUTE);
    CODEC_unmute(HEADPHONE_MUTE);     
    

    
    //CODEC_mute(HEADPHONE_MUTE);  
    // Route LineIn to HPOut (preliminary CODEC test)
    //CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 1 << 6);
    // Set HP out left and right volume
    //CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818);
    //CODEC_unmute(HEADPHONE_MUTE);
}

unsigned int CODEC_spi_write (unsigned int adr, unsigned int data) 
{
    unsigned char buf[4] = {((adr & 0xFF00)>>8), adr, ((data & 0xFF00)>>8), data};
    SPI_master_write(SPI_3, buf, 4, AUDIO_CODEC_CS);
    return data;
}

unsigned int CODEC_spi_modify_write (unsigned int adr, unsigned int reg, unsigned int mask, unsigned int data)
{
    reg &= mask;
    reg |= data;
    unsigned char buf[4] = {((adr & 0xFF00)>>8), adr, ((reg & 0xFF00)>>8), reg};
    SPI_master_write(SPI_3, buf, 4, AUDIO_CODEC_CS); 
    return reg;
}

void CODEC_mic_config (unsigned char bias_res, unsigned char bias_volt, unsigned char gain)
{
    // Set microphone bias impedance
    CODEC_struct.CHIP_MIC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_MIC_CTRL, CODEC_struct.CHIP_MIC_CTRL, 0xFCFF, bias_res << 8);
    // Set microphone bias voltage
    CODEC_struct.CHIP_MIC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_MIC_CTRL, CODEC_struct.CHIP_MIC_CTRL, 0xFF8F, bias_volt << 4);
    // Set microphone gain
    CODEC_struct.CHIP_MIC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_MIC_CTRL, CODEC_struct.CHIP_MIC_CTRL, 0xFFFC, gain << 0);    
}

// Set DAC volume
// Min is 0dB, max is -90dB, in 0.5dB step
// 0 <= dac_vol_right <= 180, where 180 * 0.5 = -90dB
void CODEC_set_dac_volume (unsigned char dac_vol_right, unsigned char dac_vol_left)
{
    if ((dac_vol_right >= 0) && (dac_vol_right <= 180))
    {
        CODEC_struct.dac_vol_right = dac_vol_right + 0x3C;  // 0x3C is 0dB base offset in SGTL5000       
    }
    if ((dac_vol_left >= 0) && (dac_vol_left <= 180))
    {
        CODEC_struct.dac_vol_left = dac_vol_left + 0x3C;    // 0x3C is 0dB base offset in SGTL5000        
    }    
    CODEC_struct.CHIP_DAC_VOL = CODEC_spi_write(CODEC_CHIP_DAC_VOL, ((CODEC_struct.dac_vol_right << 8) | CODEC_struct.dac_vol_left)); 
}

void CODEC_mute (unsigned char channel)
{
    switch(channel)
    {
        case ADC_MUTE:
            CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFE, 1 << 0);
            break;
            
        case HEADPHONE_MUTE:
            CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFEF, 1 << 4);
            break;
            
        case LINEOUT_MUTE:
            CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFEFF, 1 << 8);
            break;
             
        case DAC_MUTE:
             CODEC_struct.CHIP_ADCDAC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ADCDAC_CTRL, CODEC_struct.CHIP_ADCDAC_CTRL, 0xFFF3, 3 << 2);
            break;
            
        default:
            break;
    }
}

void CODEC_unmute (unsigned char channel)
{
    switch(channel)
    {
        case ADC_MUTE:
            CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFE, 0 << 0);
            break;
            
        case HEADPHONE_MUTE:
            CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFEF, 0 << 4);
            break;
            
        case LINEOUT_MUTE:
            CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFEFF, 0 << 8);
            break;

        case DAC_MUTE:
             CODEC_struct.CHIP_ADCDAC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ADCDAC_CTRL, CODEC_struct.CHIP_ADCDAC_CTRL, 0xFFF3, 0 << 2);
            break;
            
        default:
            break;
    }   
}

void DCI_init (unsigned char sys_fs)
{
    DCICON1bits.DCIEN = 0;  // Disable DCI module if it was in use
    IFS3bits.DCIIF = 0;     // Clear DCI interrupt flag
    IEC3bits.DCIIE = 0;     // Disable DCI interrupt
    
    DCICON1bits.COFSM = 0x1;// Frame sync set to I2S frame sync mode
    DCICON1bits.CSCKD = 1;  // CSCK pin is an input when DCI module is enabled
    DCICON1bits.COFSD = 1;  // COFS pin is an input when DCI module is enabled
    DCICON2bits.WS = 0xF;   // DCI data word size is 16 bits
    
    RSCONbits.RSE0 = 1;     // Enable receive time slot 0
    TSCONbits.TSE0 = 1;     // Enable transmit time slot 0
    
    TRISDbits.TRISD1 = 1;   // RD1 configured as an input (I2S_COFS)
    TRISDbits.TRISD2 = 1;   // RD2 configured as an input (I2S_SCLK)
    TRISDbits.TRISD3 = 0;   // RD3 configured as an output (I2S_DOUT)
    TRISDbits.TRISD12 = 1;  // RD12 configured as an input (I2S_DIN)
    
    RPINR25bits.COFSR = 65; // RD1 (RP65) assigned to I2S_COFS    
    RPINR24bits.CSCKR = 66; // RD2 (RP66) assigned to I2C_SCLK
    RPOR1bits.RP67R = 0x0B; // RD3 (RP67) assigned to I2S_DOUT
    RPINR24bits.CSDIR = 76; // RD12 (RPI76) assigned to I2S_DIN
   
    IPC15bits.DCIIP = 6;    // Make the DCI interrupt higher priority than nominal
    DCICON1bits.DCIEN = 1;  // Enable DCI module
}

void __attribute__((__interrupt__, no_auto_psv)) _DCIInterrupt(void)
{
    static unsigned char sine_counter = 0;
    unsigned int dumb;
    IFS3bits.DCIIF = 0;      // clear DCI interrupt flag
    TXBUF0 = sine_wave_255_65k[sine_counter];
    dumb = RXBUF0;
    if (++sine_counter > 255){sine_counter = 0;}
    //UART_rx_interrupt(UART_1);// process interrupt routine
}