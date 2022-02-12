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
uint8_t codec_int_flag = 0;
uint16_t rx_ch_left = 0;
uint16_t rx_ch_right = 0;
extern uint16_t tx_ch_left;
extern uint8_t first_read;
extern uint16_t tx_ch_right;
uint8_t data_tx_ready = 0;

STRUCT_SPI *CODEC_spi;

void CODEC_init (uint8_t sys_fs)
{
    uint32_t pll_out_freq = 0;
    uint16_t pll_int_divisor = 0;
    uint16_t pll_frac_divisor = 0;
    DCI_init();
    SPI_init(CODEC_spi, SPI_3, SPI_MODE0, 2, 0, SPI_BUF_LENGTH, SPI_BUF_LENGTH);   
                                        // PPRE = 2, primary prescale 1:4
                                        // SPRE = 0, Secondary prescale 1:8
                                        // Fspi = FCY / 32 = 2.175MHz
    
    // Enable 1.8V output to CODEC
    TRISKbits.TRISK1 = 0;               // Set CODEC_1V8_EN to output
    LATKbits.LATK1 = 0;                 // Disable CODEC
    __delay_ms(250);                     // Power-off delay
    LATKbits.LATK1 = 1;                 // Enable 1.8V
    __delay_ms(250);                     // Power-on delay
    
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
    pll_int_divisor = (uint16_t)(pll_out_freq / SYS_MCLK);
    pll_frac_divisor = (uint16_t)(((pll_out_freq / SYS_MCLK)-pll_int_divisor)*2048);
    
    // Write PLL dividers to CODEC
    CODEC_struct.CHIP_PLL_CTRL = CODEC_spi_modify_write(CODEC_CHIP_PLL_CTRL, CODEC_struct.CHIP_PLL_CTRL, 0x07FF, pll_int_divisor<<11);
    CODEC_struct.CHIP_PLL_CTRL = CODEC_spi_modify_write(CODEC_CHIP_PLL_CTRL, CODEC_struct.CHIP_PLL_CTRL, 0xF800, pll_frac_divisor);

    // Codec PLL configuration
    // Power-up the PLL
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xFBFF, 1<<10);
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xFEFF, 1<<8);
    __delay_us(100);    // PLL power up delay
    
    // Codec System MCLK and Sample Clock
    CODEC_struct.CHIP_CLK_CTRL = CODEC_spi_modify_write(CODEC_CHIP_CLK_CTRL, CODEC_struct.CHIP_CLK_CTRL, 0xFFF3, sys_fs<<2);
    // FOR 8kHz SAMPLING, REMOVE OTHERWISE
    // CODEC_struct.CHIP_CLK_CTRL = CODEC_spi_modify_write(CODEC_CHIP_CLK_CTRL, CODEC_struct.CHIP_CLK_CTRL, 0xFF3F, 3 << 4);
    //
    CODEC_struct.CHIP_CLK_CTRL = CODEC_spi_modify_write(CODEC_CHIP_CLK_CTRL, CODEC_struct.CHIP_CLK_CTRL, 0xFFFC, 3 << 0);
    
    // Set CODEC to I2S master, set data length to 16 bits
    CODEC_struct.CHIP_I2S_CTRL = CODEC_spi_modify_write(CODEC_CHIP_I2S_CTRL, CODEC_struct.CHIP_I2S_CTRL, 0xFF7F, 1 << 7);
    CODEC_struct.CHIP_I2S_CTRL = CODEC_spi_modify_write(CODEC_CHIP_I2S_CTRL, CODEC_struct.CHIP_I2S_CTRL, 0xFFCF, 3 << 4);

    // Set ADC and DAC to stereo
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xBFFF, 1<<14);
    CODEC_struct.CHIP_ANA_POWER = CODEC_spi_modify_write(CODEC_CHIP_ANA_POWER, CODEC_struct.CHIP_ANA_POWER, 0xFFBF, 1<<6);    
   
    // Route DAC to HPout
    // CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
    // CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818);    
    
    // Route ADC to I2Sout
    // CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFFC, 0 << 0);        // I2S out data source is ADC
    
    // I2S in -> HP Out
    // CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFCF, 1 << 4);   // DAC data source is I2S in
    // CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
    // CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818);  
    
    // Route MicIn to ADC
    // CODEC_mic_config(MIC_BIAS_RES_2k, MIC_BIAS_VOLT_2V00, MIC_GAIN_30dB);
    // CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   // DAC data source is ADC
    // CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFB, 0 << 2);   // ADC input is microphone 
    // Route Dac to HP Out
    // CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
    // CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818);

    // ROute DAC to Line Out
//    CODEC_struct.CHIP_LINE_OUT_CTRL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_CTRL, 0x0322);   // Out current = 360uA, LO_VAG = 1.65V
//    CODEC_struct.CHIP_LINE_OUT_VOL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_VOL, 0x0F0F);      // Recommended nominal for VDDIO = 3.3
//    CODEC_unmute(ADC_MUTE);
//    CODEC_unmute(DAC_MUTE);
//    CODEC_unmute(HEADPHONE_MUTE);    

#ifdef BRINGUP_DSPEAK_1    
    // Route MicIn to ADC
    CODEC_mic_config(MIC_BIAS_RES_2k, MIC_BIAS_VOLT_1V25, MIC_GAIN_40dB);
    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFB, 0 << 2);   // ADC input is microphone    
    // Set DAC input to ADC
    CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   // DAC data source is ADC
    CODEC_struct.CHIP_DAC_VOL = CODEC_spi_write(CODEC_CHIP_DAC_VOL, 0x3C3C);                // 0dB gain on DAC    
    // Route DAC to LineOut
    CODEC_struct.CHIP_LINE_OUT_CTRL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_CTRL, 0x0322);    // Out current = 360uA, LO_VAG = 1.65V
    CODEC_struct.CHIP_LINE_OUT_VOL = CODEC_spi_write(CODEC_CHIP_LINE_OUT_VOL, 0x0F0F);      // Recommended nominal for VDDIO = 3.3   
#endif    
       
#ifdef BRINGUP_DSPEAK_2
    // Route LineIn to ADC
    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFFB, 1 << 2);
    CODEC_struct.CHIP_ANA_ADC_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_ADC_CTRL, 0x0000);  // Gain of 0dB on ADC
//    // Route ADC to DAC
    CODEC_struct.CHIP_SSS_CTRL = CODEC_spi_modify_write(CODEC_CHIP_SSS_CTRL, CODEC_struct.CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   // DAC data source is ADC
    CODEC_struct.CHIP_DAC_VOL = CODEC_spi_write(CODEC_CHIP_DAC_VOL, 0x3C3C);    // 0dB gain on DAC
//    // Route DAC output to HP
    CODEC_struct.CHIP_ANA_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_CTRL, CODEC_struct.CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
    CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, 0x1818); 
#endif  
    
    // Unmute all channels
    CODEC_unmute(ADC_MUTE);
    CODEC_unmute(DAC_MUTE);
    CODEC_unmute(HEADPHONE_MUTE);
    CODEC_unmute(LINEOUT_MUTE);
}

uint16_t CODEC_spi_write (uint16_t adr, uint16_t data) 
{
    uint8_t buf[4] = {((adr & 0xFF00)>>8), adr, ((data & 0xFF00)>>8), data};
    SPI_load_tx_buffer(CODEC_spi, buf, 4);
    SPI_master_write(CODEC_spi, AUDIO_CODEC_CS);
    return data;
}

uint16_t CODEC_spi_modify_write (uint16_t adr, uint16_t reg, uint16_t mask, uint16_t data)
{
    reg &= mask;
    reg |= data;
    uint8_t buf[4] = {((adr & 0xFF00)>>8), adr, ((reg & 0xFF00)>>8), reg};
    SPI_load_tx_buffer(CODEC_spi, buf, 4);
    SPI_master_write(CODEC_spi, AUDIO_CODEC_CS); 
    return reg;
}

void CODEC_mic_config (uint8_t bias_res, uint8_t bias_volt, uint8_t gain)
{
    // Set microphone bias impedance
    CODEC_struct.CHIP_MIC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_MIC_CTRL, CODEC_struct.CHIP_MIC_CTRL, 0xFCFF, bias_res << 8);
    // Set microphone bias voltage
    CODEC_struct.CHIP_MIC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_MIC_CTRL, CODEC_struct.CHIP_MIC_CTRL, 0xFF8F, bias_volt << 4);
    // Set microphone gain
    CODEC_set_mic_gain(gain);  
}


void CODEC_set_dac_volume (uint8_t dac_vol_right, uint8_t dac_vol_left)
{
    // Set DAC volume
    // Min is 0dB, max is -90dB, in 0.5dB step
    // 0 <= dac_vol_L/R <= 180, where 180 * 0.5 = -90dB    
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

void CODEC_set_hp_volume (uint8_t hp_vol_right, uint8_t hp_vol_left)
{
    // SGTL datasheet, adjustment to HP vol is made by steps of 0.5dB / LSB
    // 0x00 = +12dB
    // 0x18 = 0dB
    // 0x7F - 51.5dB        
    if ((hp_vol_right >= 0) && (hp_vol_right <= 0x7F))
    {
        CODEC_struct.hp_vol_right = hp_vol_right + 0x18;  // 0x18 is 0dB base offset in SGTL5000       
    }
    if ((hp_vol_left >= 0) && (hp_vol_left <= 0x7F))
    {
        CODEC_struct.hp_vol_left = hp_vol_left + 0x18;    // 0x18 is 0dB base offset in SGTL5000        
    }    
    CODEC_struct.CHIP_ANA_HP_CTRL = CODEC_spi_write(CODEC_CHIP_ANA_HP_CTRL, ((CODEC_struct.hp_vol_right << 8) | CODEC_struct.hp_vol_left));     
}

void CODEC_set_mic_gain (uint8_t gain)
{
    // Set microphone gain
    CODEC_struct.CHIP_MIC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_MIC_CTRL, CODEC_struct.CHIP_MIC_CTRL, 0xFFFC, gain << 0);     
}

void CODEC_set_analog_gain (uint8_t range, uint8_t gain_right, uint8_t gain_left)
{
    // If range == 0, gain range from 0 to 22.5dB in step of 1.5dB, from 0x0 to 0xF
    // If range == 1, gain range from -6 to 16.5dB in step of 1.5dB, from 0x0 to 0xF
    if ((gain_right >= 0) && (gain_right <= 0xF))
    {
        CODEC_struct.CHIP_ANA_ADC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_ADC_CTRL, CODEC_struct.CHIP_ANA_ADC_CTRL, (0xFE0F | (range << 8)), gain_right << 4);
    }
    if ((gain_left >= 0) && (gain_right <= 0xF))
    {
        CODEC_struct.CHIP_ANA_ADC_CTRL = CODEC_spi_modify_write(CODEC_CHIP_ANA_ADC_CTRL, CODEC_struct.CHIP_ANA_ADC_CTRL, (0xFE0F | (range << 8)), gain_left << 0);
    }
}

void CODEC_mute (uint8_t channel)
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

void CODEC_unmute (uint8_t channel)
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


void DCI_init (void)
{
    uint16_t i = 0;
    DCICON1bits.DCIEN = 0;  // Disable DCI module if it was in use
    IFS3bits.DCIIF = 0;     // Clear DCI interrupt flag
    IEC3bits.DCIIE = 0;     // Disable DCI interrupt
    
    DCICON1bits.COFSM = 0x1;// Frame sync set to I2S frame sync mode
    DCICON1bits.CSCKD = 1;  // CSCK pin is an input when DCI module is enabled
    DCICON1bits.COFSD = 1;  // COFS pin is an input when DCI module is enabled
    DCICON1bits.CSCKE = 1;  // Sample incoming data on the rising edge of CSCK
    DCICON2bits.WS = 0xF;   // DCI data word size is 16 bits (standard)
    DCICON2bits.COFSG = 1;  // Two data words transfered per I2S frame (Right(16) + Left(16))
    DCICON2bits.BLEN = 1;   // Enable interrupt after 2 data word transmitted
    DCICON3bits.BCG = 0;    // Clear baud-rate generator
    
    RSCONbits.RSE0 = 1;     // Enable receive time slot 0
    RSCONbits.RSE1 = 1;     // Enable receive time slot 1
    TSCONbits.TSE0 = 1;     // Enable transmit time slot 0
    TSCONbits.TSE1 = 1;     // Enable transmit time slot 1
    
    TRISDbits.TRISD1 = 1;   // RD1 configured as an input (I2S_COFS)
    TRISDbits.TRISD2 = 1;   // RD2 configured as an input (I2S_SCLK)
    TRISDbits.TRISD3 = 0;   // RD3 configured as an output (I2S_DOUT)
    TRISDbits.TRISD12 = 1;  // RD12 configured as an input (I2S_DIN)
    
    RPINR25bits.COFSR = 65; // RD1 (RP65) assigned to I2S_COFS    
    RPINR24bits.CSCKR = 66; // RD2 (RP66) assigned to I2C_SCLK
    RPOR1bits.RP67R = 0x0B; // RD3 (RP67) assigned to I2S_DOUT
    RPINR24bits.CSDIR = 76; // RD12 (RPI76) assigned to I2S_DIN
    
    for (; i < (2 * CODEC_BLOCK_TRANSFER); i++)
    {
        CODEC_struct.DCI_receive_buffer[i] = 0;  // Initialize buffer
        CODEC_struct.DCI_transmit_buffer[i] = 0; // Initialize buffer       
    }
    CODEC_struct.DCI_receive_counter = 0;   // Initialize counter to 0
    CODEC_struct.DCI_transmit_counter = 0;  // Initialize counter to 0
    
    IPC15bits.DCIIP = 4;    // Make the DCI interrupt higher priority than nominal
    IEC3bits.DCIIE = 1;     // Enable DCI interrupt
    DCICON1bits.DCIEN = 1;  // Enable DCI module
}

uint8_t DCI_get_interrupt_state (void)
{
    if (codec_int_flag == 1)
    {
        codec_int_flag = 0;
        return 1;
    }
    else 
        return 0;
}

void DCI_transmit_enable (void)
{
    CODEC_struct.DCI_transmit_enable = 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _DCIInterrupt(void)
{
    IFS3bits.DCIIF = 0;      // clear DCI interrupt flag
    
    CODEC_struct.DCI_receive_buffer[CODEC_struct.DCI_receive_counter++] = RXBUF0;
    CODEC_struct.DCI_receive_buffer[CODEC_struct.DCI_receive_counter++] = RXBUF1;  
    if (CODEC_struct.DCI_receive_counter >= CODEC_BLOCK_TRANSFER)
    {
        CODEC_struct.DCI_receive_counter = 0;
    }
    
    if (CODEC_struct.DCI_transmit_enable == 1)
    {
        TXBUF0 = CODEC_struct.DCI_transmit_buffer[CODEC_struct.DCI_transmit_counter++];
        TXBUF1 = CODEC_struct.DCI_transmit_buffer[CODEC_struct.DCI_transmit_counter++];   
        if (CODEC_struct.DCI_transmit_counter >= CODEC_BLOCK_TRANSFER)
        {
            CODEC_struct.DCI_transmit_counter = 0;
            CODEC_struct.DCI_transmit_enable = 0;
        }
    }
    codec_int_flag = 1;
}