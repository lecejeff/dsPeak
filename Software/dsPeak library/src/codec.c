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
STRUCT_CODEC CODEC_struct[CODEC_QTY]; 

#ifdef DCI0_DMA_ENABLE
__eds__ uint16_t codec_dma_tx_buf_A[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
__eds__ uint16_t codec_dma_rx_buf_A[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
__eds__ uint16_t codec_dma_tx_buf_B[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
__eds__ uint16_t codec_dma_rx_buf_B[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
#endif

void CODEC_init (STRUCT_CODEC *codec, STRUCT_SPI *spi, uint8_t spi_channel, uint8_t sys_fs, uint16_t tx_buf_length, uint16_t rx_buf_length)
{
    uint32_t pll_out_freq = 0;
    uint16_t pll_int_divisor = 0;
    uint16_t pll_frac_divisor = 0;
    
    DCI_init(codec);
    
    codec->SPI_channel = spi_channel;
    codec->spi_ref = spi; 
    
    SPI_init(codec->spi_ref, codec->SPI_channel, SPI_MODE0, 2, 0, tx_buf_length, rx_buf_length);   
                                        // PPRE = 2, primary prescale 1:4
                                        // SPRE = 0, Secondary prescale 1:8
                                        // Fspi = FCY / 32 = 2.175MHz
    
    // Enable 1.8V output to CODEC
    TRISKbits.TRISK1 = 0;               // Set CODEC_1V8_EN to output
    LATKbits.LATK1 = 0;                 // Disable CODEC
    __delay_ms(250);                    // Power-off delay
    LATKbits.LATK1 = 1;                 // Enable 1.8V
    __delay_ms(250);                    // Power-on delay
    
    // Since SPI read is not supported, write default reset values to struct
    // registers to support the modify operation. Reset values taken from datasheet
    codec->CHIP_DIG_POWER = 0;        // Every audio subsystem is disabled
    codec->CHIP_CLK_CTRL = 0x0008;    // SYS_FS = 48kHz               
    codec->CHIP_I2S_CTRL = 0x0010;    // DLEN = 24b
    codec->CHIP_SSS_CTRL = 0x0010;    // DAC_SELECT = I2S_IN
    codec->CHIP_ADCDAC_CTRL = 0x020C; // VOL_RAMP ENABLED, DAC L/R MUTED
    codec->CHIP_DAC_VOL = 0x3C3C;     // DAC VOL L/R = 0dB
    codec->CHIP_PAD_STRENGTH = 0x015F;// I2S PAD STRENGTH = 0x1 = 4.02mA, I2C PAD STRENGTH = 0x3 = 12.05mA
    codec->CHIP_ANA_ADC_CTRL = 0;     // No change in ADC range
    codec->CHIP_ANA_HP_CTRL = 0x1818; // HP VOL L/R = 0dB
    codec->CHIP_ANA_CTRL = 0x0111;    // LineOut = Mute, HP = Mute, ADC = Mute
    codec->CHIP_LINREG_CTRL = 0;      //
    codec->CHIP_REF_CTRL = 0;         //
    codec->CHIP_MIC_CTRL = 0;         //
    codec->CHIP_LINE_OUT_CTRL = 0;    //
    codec->CHIP_LINE_OUT_VOL = 0x0404;// LineOut output level, refer to equation in datasheet
    codec->CHIP_ANA_POWER = 0x7060;   // DAC Stereo, Power-up linreg, ADC stereo, Power-up ref bias
    codec->CHIP_PLL_CTRL = 0x5000;    // PLL INT and FRAC divisors, refer to equation in datasheet
    codec->CHIP_CLK_TOP_CTRL = 0;     //
    codec->CHIP_ANA_STATUS = 0;       //
    codec->CHIP_ANA_TEST1 = 0x01C0;   // HP = ClassAB, VGND center for best antipop performance
    codec->CHIP_ANA_TEST2 = 0;        //
    codec->CHIP_SHORT_CTRL = 0;       //
    codec->DAP_CONTROL = 0;           //
    codec->DAP_PEQ = 0;               //
    codec->DAP_BASS_ENHANCE = 0x0040; // Bass enhance cutoff frequency = 175Hz
    codec->DAP_BASS_ENHANCE_CTRL = 0x051F;    
    codec->DAP_AUDIO_EQ = 0;          //
    codec->DAP_SGTL_SURROUND = 0x0040;// Mid width perception change
    codec->DAP_FILTER_COEF_ACCESS = 0;//
    codec->DAP_COEF_WR_B0_MSB = 0;    //
    codec->DAP_COEF_WR_B0_LSB = 0;    //
    codec->DAP_AUDIO_EQ_BASS_BAND0 = 0x002F;  // Bass/GEG Band 0 to 0dB
    codec->DAP_AUDIO_EQ_BAND1 = 0x002F;       // GEQ Band1 to 0dB
    codec->DAP_AUDIO_EQ_BAND2 = 0x002F;       // GEQ Band2 to 0dB
    codec->DAP_AUDIO_EQ_BAND3 = 0x002F;       // GEQ Band3 to 0dB
    codec->DAP_AUDIO_EQ_TREBLE_BAND4 = 0x002F;// Tone / Treble GEQ4 to 0dB
    codec->DAP_MAIN_CHAN = 0x8000;    // DAP main channel volume = 100%
    codec->DAP_MIX_CHAN = 0;          // DAP Mix channel volume = 100%
    codec->DAP_AVC_CTRL = 0x5100;     // AVC 6dB gain, LBI 25ms response
    codec->DAP_AVC_THRESHOLD = 0x1473;// Threshold set to -12dB
    codec->DAP_AVC_ATTACK = 0x0028;   // Attack rate = 32dB/s
    codec->DAP_AVC_DECAY = 0x0050;    // Decay rate = 4dB/s
    codec->DAP_COEF_WR_B1_MSB = 0;    //
    codec->DAP_COEF_WR_B1_LSB = 0;    //
    codec->DAP_COEF_WR_B2_MSB = 0;    //
    codec->DAP_COEF_WR_B2_LSB = 0;    //
    codec->DAP_COEF_WR_A1_MSB = 0;    //
    codec->DAP_COEF_WR_A1_LSB = 0;    //
    codec->DAP_COEF_WR_A2_MSB = 0;    //
    codec->DAP_COEF_WR_A2_LSB = 0;    //
    
    // Following the datasheet recommendations for initialization sequence   
    // Codec power supply configuration, hardware implementation-dependent
    codec->CHIP_ANA_POWER = CODEC_spi_write(codec, CODEC_CHIP_ANA_POWER, 0x4060);        // Turnoff startup power supplies to save power
    __delay_ms(100);
    codec->CHIP_LINREG_CTRL = CODEC_spi_write(codec, CODEC_CHIP_LINREG_CTRL, 0x0060);    // Configure charge pump to use the VDDIO rail
    codec->CHIP_REF_CTRL = CODEC_spi_write(codec, CODEC_CHIP_REF_CTRL, 0x01F0);          // VAG -> 1.575V, BIAS Nominal, Normal VAG ramp
    codec->CHIP_LINE_OUT_CTRL = CODEC_spi_write(codec, CODEC_CHIP_LINE_OUT_CTRL, 0x0322);// LineOut bias -> 360uA, LineOut VAG -> 1.65V
    codec->CHIP_SHORT_CTRL = CODEC_spi_write(codec, CODEC_CHIP_SHORT_CTRL, 0x4446);      // Enable HP short detect, set trip to 125mA
    codec->CHIP_ANA_CTRL = CODEC_spi_write(codec, CODEC_CHIP_ANA_CTRL, 0x0137);          // Enable Zero-cross detection SEE IF USEFUL OR NOT *****
    codec->CHIP_ANA_POWER = CODEC_spi_write(codec, CODEC_CHIP_ANA_POWER, 0x40FF);        // Power up LineOUt, HP & capless mode, ADC, DAC
    codec->CHIP_DIG_POWER = CODEC_spi_write(codec, CODEC_CHIP_DIG_POWER, 0x0073);        // Power UP I2S, DAP, DAC and ADC
    
    // PLL output frequency is based on the sample clock rate used
    if (sys_fs == SYS_FS_44_1kHz)
    {
        pll_out_freq = 180633600;
    }
    
    else
    {
        pll_out_freq = 196608000;
    }
    pll_int_divisor = (uint16_t)(pll_out_freq / CODEC_SYS_MCLK);
    pll_frac_divisor = (uint16_t)(((pll_out_freq / CODEC_SYS_MCLK)-pll_int_divisor)*2048);
    
    // Write PLL dividers to CODEC
    codec->CHIP_PLL_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_PLL_CTRL, codec->CHIP_PLL_CTRL, 0x07FF, pll_int_divisor<<11);
    codec->CHIP_PLL_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_PLL_CTRL, codec->CHIP_PLL_CTRL, 0xF800, pll_frac_divisor);

    // Codec PLL configuration
    // Power-up the PLL
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFBFF, 1<<10);
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFEFF, 1<<8);
    __delay_us(100);    // PLL power up delay
    
    // Codec System MCLK and Sample Clock
    codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, sys_fs<<2);
    // FOR 8kHz SAMPLING, REMOVE OTHERWISE
    // codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFF3F, 3 << 4);
    //
    codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFFC, 3 << 0);
    
    // Set CODEC to I2S master, set data length to 16 bits
    codec->CHIP_I2S_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_I2S_CTRL, codec->CHIP_I2S_CTRL, 0xFF7F, 1 << 7);
    codec->CHIP_I2S_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_I2S_CTRL, codec->CHIP_I2S_CTRL, 0xFFCF, 3 << 4);

    // Set ADC and DAC to stereo
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xBFFF, 1<<14);
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFFBF, 1<<6);    

    // Route MicIn to ADC
     CODEC_mic_config(codec, MIC_BIAS_RES_2k, MIC_BIAS_VOLT_2V00, MIC_GAIN_30dB);
     codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   // DAC data source is ADC
     codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 0 << 2);   // ADC input is microphone 
    
    // Route ADC to I2Sout
     codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 0 << 0);        // I2S out data source is ADC
    
    // I2S in -> HP Out
     codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 1 << 4);   // DAC data source is I2S in
     codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
     codec->CHIP_ANA_HP_CTRL = CODEC_spi_write(codec, CODEC_CHIP_ANA_HP_CTRL, 0x1818);  
    
    // Route Dac to HP Out
//     codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFBF, 0 << 6);   // Headphone input is DAC
//     codec->CHIP_ANA_HP_CTRL = CODEC_spi_write(codec, CODEC_CHIP_ANA_HP_CTRL, 0x1818);

    // ROute DAC to Line Out
//    codec->CHIP_LINE_OUT_CTRL = CODEC_spi_write(codec, CODEC_CHIP_LINE_OUT_CTRL, 0x0322);   // Out current = 360uA, LO_VAG = 1.65V
//    codec->CHIP_LINE_OUT_VOL = CODEC_spi_write(codec, CODEC_CHIP_LINE_OUT_VOL, 0x0F0F);      // Recommended nominal for VDDIO = 3.3
//    CODEC_unmute(codec, ADC_MUTE);
//    CODEC_unmute(codec, DAC_MUTE);
//    CODEC_unmute(codec, HEADPHONE_MUTE);    
    
    // Unmute all channels
    CODEC_unmute(codec, ADC_MUTE);
    CODEC_unmute(codec, DAC_MUTE);
    CODEC_unmute(codec, HEADPHONE_MUTE);
    CODEC_unmute(codec, LINEOUT_MUTE);
    
    // Enable DCI tx and RX 
    DCI_set_transmit_state(codec, DCI_TRANSMIT_ENABLE);
    DCI_set_receive_state(codec, DCI_RECEIVE_ENABLE);
    
#ifdef DCI0_DMA_ENABLE   
    // Force first DMA transfers to TXBUF0 and TXBUF1
    DMA_force_txfer(codec->DMA_tx_channel);
    while(DMA_get_force_state(codec->DMA_tx_channel) == 1);
    DMA_force_txfer(codec->DMA_tx_channel);
    while(DMA_get_force_state(codec->DMA_tx_channel) == 1);   
#endif   
    DCI_enable(codec);
}

uint16_t CODEC_spi_write (STRUCT_CODEC *codec, uint16_t adr, uint16_t data) 
{
    uint8_t buf[4] = {((adr & 0xFF00)>>8), adr, ((data & 0xFF00)>>8), data};
    // Blocking SPI call
    while (SPI_module_busy(codec->spi_ref) != SPI_MODULE_FREE);
    SPI_load_tx_buffer(codec->spi_ref, buf, 4);
    SPI_master_write(codec->spi_ref, AUDIO_CODEC_CS);
    // End of SPI transaction
    return data;
}

uint16_t CODEC_spi_modify_write (STRUCT_CODEC *codec, uint16_t adr, uint16_t reg, uint16_t mask, uint16_t data)
{
    reg &= mask;
    reg |= data;
    uint8_t buf[4] = {((adr & 0xFF00)>>8), adr, ((reg & 0xFF00)>>8), reg};
    // Blocking SPI call
    while (SPI_module_busy(codec->spi_ref) != SPI_MODULE_FREE);
    SPI_load_tx_buffer(codec->spi_ref, buf, 4);
    SPI_master_write(codec->spi_ref, AUDIO_CODEC_CS); 
    // End of SPI transaction
    return reg;
}

void CODEC_mic_config (STRUCT_CODEC *codec, uint8_t bias_res, uint8_t bias_volt, uint8_t gain)
{
    // Set microphone bias impedance
    codec->CHIP_MIC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_MIC_CTRL, codec->CHIP_MIC_CTRL, 0xFCFF, bias_res << 8);
    // Set microphone bias voltage
    codec->CHIP_MIC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_MIC_CTRL, codec->CHIP_MIC_CTRL, 0xFF8F, bias_volt << 4);
    // Set microphone gain
    CODEC_set_mic_gain(codec, gain);  
}


void CODEC_set_dac_volume (STRUCT_CODEC *codec, uint8_t dac_vol_right, uint8_t dac_vol_left)
{
    // Set DAC volume
    // Min is 0dB, max is -90dB, in 0.5dB step
    // 0 <= dac_vol_L/R <= 180, where 180 * 0.5 = -90dB    
    if ((dac_vol_right >= 0) && (dac_vol_right <= 180))
    {
        codec->dac_vol_right = dac_vol_right + 0x3C;  // 0x3C is 0dB base offset in SGTL5000       
    }
    if ((dac_vol_left >= 0) && (dac_vol_left <= 180))
    {
        codec->dac_vol_left = dac_vol_left + 0x3C;    // 0x3C is 0dB base offset in SGTL5000        
    }    
    codec->CHIP_DAC_VOL = CODEC_spi_write(codec, CODEC_CHIP_DAC_VOL, ((codec->dac_vol_right << 8) | codec->dac_vol_left)); 
}

void CODEC_set_hp_volume (STRUCT_CODEC *codec, uint8_t hp_vol_right, uint8_t hp_vol_left)
{
    // SGTL datasheet, adjustment to HP vol is made by steps of 0.5dB / LSB
    // 0x00 = +12dB
    // 0x18 = 0dB
    // 0x7F - 51.5dB        
    if ((hp_vol_right >= 0) && (hp_vol_right <= 0x7F))
    {
        codec->hp_vol_right = hp_vol_right + 0x18;  // 0x18 is 0dB base offset in SGTL5000       
    }
    if ((hp_vol_left >= 0) && (hp_vol_left <= 0x7F))
    {
        codec->hp_vol_left = hp_vol_left + 0x18;    // 0x18 is 0dB base offset in SGTL5000        
    }    
    codec->CHIP_ANA_HP_CTRL = CODEC_spi_write(codec, CODEC_CHIP_ANA_HP_CTRL, ((codec->hp_vol_right << 8) | codec->hp_vol_left));     
}

void CODEC_set_mic_gain (STRUCT_CODEC *codec, uint8_t gain)
{
    // Set microphone gain
    codec->CHIP_MIC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_MIC_CTRL, codec->CHIP_MIC_CTRL, 0xFFFC, gain << 0);     
}

void CODEC_set_analog_gain (STRUCT_CODEC *codec, uint8_t range, uint8_t gain_right, uint8_t gain_left)
{
    // If range == 0, gain range from 0 to 22.5dB in step of 1.5dB, from 0x0 to 0xF
    // If range == 1, gain range from -6 to 16.5dB in step of 1.5dB, from 0x0 to 0xF
    if ((gain_right >= 0) && (gain_right <= 0xF))
    {
        codec->CHIP_ANA_ADC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_ADC_CTRL, codec->CHIP_ANA_ADC_CTRL, (0xFE0F | (range << 8)), gain_right << 4);
    }
    if ((gain_left >= 0) && (gain_right <= 0xF))
    {
        codec->CHIP_ANA_ADC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_ADC_CTRL, codec->CHIP_ANA_ADC_CTRL, (0xFE0F | (range << 8)), gain_left << 0);
    }
}

void CODEC_mute (STRUCT_CODEC *codec, uint8_t channel)
{
    switch(channel)
    {
        case ADC_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFE, 1 << 0);
            break;
            
        case HEADPHONE_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFEF, 1 << 4);
            break;
            
        case LINEOUT_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFEFF, 1 << 8);
            break;
             
        case DAC_MUTE:
             codec->CHIP_ADCDAC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ADCDAC_CTRL, codec->CHIP_ADCDAC_CTRL, 0xFFF3, 3 << 2);
            break;
            
        default:
            break;
    }
}

void CODEC_unmute (STRUCT_CODEC *codec, uint8_t channel)
{
    switch(channel)
    {
        case ADC_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFE, 0 << 0);
            break;
            
        case HEADPHONE_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFEF, 0 << 4);
            break;
            
        case LINEOUT_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFEFF, 0 << 8);
            break;

        case DAC_MUTE:
             codec->CHIP_ADCDAC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ADCDAC_CTRL, codec->CHIP_ADCDAC_CTRL, 0xFFF3, 0 << 2);
            break;
            
        default:
            break;
    }   
}

uint8_t DCI_fill_tx_buf_dma (STRUCT_CODEC *codec, uint16_t *buf, uint16_t length)
{
#ifdef DCI0_DMA_ENABLE
    uint16_t i=0;
    if (DMA_get_txfer_state(codec->DMA_tx_channel) == DMA_TXFER_DONE)
    {
        if (length > CODEC_BLOCK_TRANSFER){length = CODEC_BLOCK_TRANSFER;}
        for (; i<length; i++)
        {
            if (codec->DMA_tx_buf == 0)
            {
                codec_dma_tx_buf_A[i] = *buf++;
            }
            else
            {
                codec_dma_tx_buf_B[i] = *buf++;
            }
        }
        return 1;
    }
    else
        return 0;
#endif
    return 0;
}

// DCI operates in slave mode on dsPeak
void DCI_init (STRUCT_CODEC *codec)
{
    uint16_t i = 0;
    
    DCI_disable(codec);
   
    DCICON1bits.COFSM = 0x1;// Frame sync set to I2S frame sync mode
    DCICON1bits.CSCKD = 1;  // CSCK pin is an input when DCI module is enabled
    DCICON1bits.COFSD = 1;  // COFS pin is an input when DCI module is enabled
    DCICON1bits.CSCKE = 1;  // Sample incoming data on the rising edge of CSCK
    DCICON2bits.WS = 0xF;   // DCI data word size is 16 bits (standard)
    DCICON3bits.BCG = 0;    // Clear baud-rate generator

#ifndef DCI0_DMA_ENABLE
    // DCI with interrupt, without DMA
    DCICON2bits.COFSG = 1;  // Two data words transfered per I2S frame (Right(16) + Left(16))
    DCICON2bits.BLEN = 1;   // Enable interrupt after 2 data word transmitted    
    RSCONbits.RSE0 = 1;     // Enable receive time slot 0
    RSCONbits.RSE1 = 1;     // Enable receive time slot 1
    TSCONbits.TSE0 = 1;     // Enable transmit time slot 0
    TSCONbits.TSE1 = 1;     // Enable transmit time slot 1    
    for (; i < (2 * CODEC_BLOCK_TRANSFER); i++)
    {
        codec->DCI_receive_buffer[i] = 0;  // Initialize buffer
        codec->DCI_transmit_buffer[i] = 0; // Initialize buffer       
    }
    codec->DCI_receive_counter = 0;   // Initialize counter to 0
    codec->DCI_transmit_counter = 0;  // Initialize counter to 0
#endif

    TRISDbits.TRISD1 = 1;   // RD1 configured as an input (I2S_COFS)
    TRISDbits.TRISD2 = 1;   // RD2 configured as an input (I2S_SCLK)
    TRISDbits.TRISD3 = 0;   // RD3 configured as an output (I2S_DOUT)
    TRISDbits.TRISD12 = 1;  // RD12 configured as an input (I2S_DIN)
    
    RPINR25bits.COFSR = 65; // RD1 (RP65) assigned to I2S_COFS    
    RPINR24bits.CSCKR = 66; // RD2 (RP66) assigned to I2C_SCLK
    RPOR1bits.RP67R = 0x0B; // RD3 (RP67) assigned to I2S_DOUT
    RPINR24bits.CSDIR = 76; // RD12 (RPI76) assigned to I2S_DIN

    codec->DCI_transmit_enable = DCI_TRANSMIT_DISABLE;
    codec->DCI_receive_enable = DCI_RECEIVE_DISABLE;
    codec->interrupt_flag = 0;
    
    // DCI with interrupt, with DMA
#ifdef DCI0_DMA_ENABLE
    for (; i < CODEC_BLOCK_TRANSFER; i++)
    {
        codec_dma_tx_buf_A[i] = 0;    // Initialize buffer
        codec_dma_tx_buf_B[i] = 0;    // Initialize buffer
        codec_dma_rx_buf_A[i] = 0;    // Initialize buffer       
        codec_dma_rx_buf_B[i] = 0;    // Initialize buffer 
    }
    codec->DCI_receive_counter = 0;   // Initialize counter to 0
    codec->DCI_transmit_counter = 0;  // Initialize counter to 0 
    
    // If DMA is enabled, the DCI transfers only 1x frame / tx-rx slot
    DCICON2bits.COFSG = 1;  // Data frame has 2x words (left / right sample)
    DCICON2bits.BLEN = 0;   // Enable interrupt after 1 data word transfered  
    RSCONbits.RSE0 = 1;     // Enable receive time slot 0
    RSCONbits.RSE1 = 1;     // Enable receive time slot 1
    TSCONbits.TSE0 = 1;     // Enable transmit time slot 0
    TSCONbits.TSE1 = 1;     // Enable transmit time slot 1
    
    // DMA_CH7 is DCI TX channel
    DMA_init(DMA_CH7);
    DMA7CON = DMA_SIZE_WORD | DMA_TXFER_WR_PER | DMA_CHMODE_CPPE;
    DMA7REQ = DMAREQ_DCI;
    DMA7PAD = (volatile uint16_t)&TXBUF0;
    DMA7STAH = __builtin_dmaoffset(codec_dma_tx_buf_A);
    DMA7STAL = __builtin_dmaoffset(codec_dma_tx_buf_A);   
    DMA7STBH = __builtin_dmaoffset(codec_dma_tx_buf_B);
    DMA7STBL = __builtin_dmaoffset(codec_dma_tx_buf_B);       
    codec->DMA_tx_channel = DMA_CH7;

    // DMA_CH8 is DCI RX channel
    DMA_init(DMA_CH8);
    DMA8CON = DMA_SIZE_WORD | DMA_TXFER_RD_PER | DMA_CHMODE_CPPE;
    DMA8REQ = DMAREQ_DCI;
    DMA8PAD = (volatile uint16_t)&RXBUF0;
    DMA8STAH = __builtin_dmaoffset(codec_dma_rx_buf_A);
    DMA8STAL = __builtin_dmaoffset(codec_dma_rx_buf_A); 
    DMA8STBH = __builtin_dmaoffset(codec_dma_rx_buf_B);
    DMA8STBL = __builtin_dmaoffset(codec_dma_rx_buf_B);     
    codec->DMA_rx_channel = DMA_CH8;   
    
    codec->DMA_tx_buf = 0;  
    codec->DMA_rx_buf = 0;
       
    DMA_set_txfer_length(codec->DMA_tx_channel, CODEC_BLOCK_TRANSFER - 1); // 0 = 1x transfer
    DMA_set_txfer_length(codec->DMA_rx_channel, CODEC_BLOCK_TRANSFER - 1); // 0 = 1x transfer
    
    DMA_enable(codec->DMA_tx_channel);
    DMA_enable(codec->DMA_rx_channel);
#endif    
}

uint8_t DCI_get_interrupt_state (STRUCT_CODEC *codec, uint8_t tx_rx)
{
    uint16_t i=0;
#ifndef DCI0_DMA_ENABLE
    if (codec->interrupt_flag == 1)
    {
        codec->interrupt_flag = 0;
        return 1;
    }
    else 
        return 0;
#endif
#ifdef DCI0_DMA_ENABLE
    if (tx_rx == DCI_DMA_RX)
    {
        if (codec->DCI_receive_enable == DCI_RECEIVE_ENABLE)
        {
            if (DMA_get_txfer_state(codec->DMA_rx_channel) == DMA_TXFER_DONE)
            {
                codec->DMA_rx_buf ^= 1;
                return 1;
            }
            else
                return 0;
        }
        else 
            return 0;
    }
    
    else if (tx_rx == DCI_DMA_TX)
    {
        if(codec->DCI_transmit_enable == DCI_TRANSMIT_ENABLE)
        {
            if (DMA_get_txfer_state(codec->DMA_tx_channel) == DMA_TXFER_DONE)
            {
                if (codec->DMA_tx_buf == 0) // Tx buffer A was just sent
                {
                    for (i=0; i<CODEC_BLOCK_TRANSFER; i++)
                    {
                        codec_dma_tx_buf_B[i] = codec_dma_rx_buf_B[i];
                    }
                }
                else                        // Tx buffer B was just sent
                { 
                    for (; i<CODEC_BLOCK_TRANSFER; i++)
                    {
                        codec_dma_tx_buf_A[i] = codec_dma_rx_buf_A[i];   
                    }                   
                    codec->DMA_tx_buf ^= 1;
                }
                return 1;
            }
            else
                return 0;
        }
        else
            return 0;
    }
    else
        return 0;
#endif
}

void DCI_set_transmit_state (STRUCT_CODEC *codec, uint8_t state)
{
    codec->DCI_transmit_enable = state;
}

void DCI_set_receive_state (STRUCT_CODEC *codec, uint8_t state)
{
    codec->DCI_receive_enable = state;
}

void DCI_enable (STRUCT_CODEC *codec)
{
    codec->DCI_enable_state = 1;
#ifndef DCI0_DMA_ENABLE
    IEC3bits.DCIIE = 1;     // Enable DCI interrupt (only when DMA not enabled)
    IPC15bits.DCIIP = 4;    // Make the DCI interrupt higher priority than nominal      
#endif
    DCICON1bits.DCIEN = 1;  // Enable DCI module    
}

void DCI_disable (STRUCT_CODEC *codec)
{
    codec->DCI_enable_state = 0;
    IEC3bits.DCIIE = 0;     // Disable DCI interrupt  
    IFS3bits.DCIIF = 0;     // Clear DCI interrupt flag   
    DCICON1bits.DCIEN = 0;  // Disable DCI module   
}

void __attribute__((__interrupt__, auto_psv)) _DCIInterrupt(void)
{
    IFS3bits.DCIIF = 0;      // clear DCI interrupt flag
    
    // Without DMA
#ifndef DCI0_DMA_ENABLE
    // I2S direct loopback (I2Sin -> I2Sout)
    TXBUF0 = RXBUF0;
    TXBUF1 = RXBUF1;     
    
//    CODEC_struct[DCI_0].DCI_receive_buffer[CODEC_struct[DCI_0].DCI_receive_counter++] = RXBUF0;
//    CODEC_struct[DCI_0].DCI_receive_buffer[CODEC_struct[DCI_0].DCI_receive_counter++] = RXBUF1;  
//    if (CODEC_struct[DCI_0].DCI_receive_counter >= CODEC_BLOCK_TRANSFER)
//    {
//        CODEC_struct[DCI_0].DCI_receive_counter = 0;
//    }
//    
//    if (CODEC_struct[DCI_0].DCI_transmit_enable == 1)
//    {
//        TXBUF0 = CODEC_struct[DCI_0].DCI_transmit_buffer[CODEC_struct[DCI_0].DCI_transmit_counter++];
//        TXBUF1 = CODEC_struct[DCI_0].DCI_transmit_buffer[CODEC_struct[DCI_0].DCI_transmit_counter++];   
//        if (CODEC_struct[DCI_0].DCI_transmit_counter >= CODEC_BLOCK_TRANSFER)
//        {
//            CODEC_struct[DCI_0].DCI_transmit_counter = 0;
//            CODEC_struct[DCI_0].DCI_transmit_enable = 0;
//        }
//    }
#endif
    CODEC_struct[DCI_0].interrupt_flag = 1;
}