//****************************************************************************//
// File      :  main.c
//
// Includes  :  general.h
//              Project-related .h files
//
// Purpose   : dsPeak audio recorder + playback demo project
//
// Intellitrol                   MPLab X v5.45                        24/09/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "dspeak_generic.h"

//----------------------------------------------------------------------------//
// dsPIC33EP256MU814 configuration fuses, do not remove
// Configuration settings can be changed in "Production -> Set configuration bits"
// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Primary oscillator at POR is FRC
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select Bit (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function Bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock-switching enabled, Fail-safe clock monitor disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = ON               // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled )
#pragma config ALTI2C1 = ON             // Alternate I2C pins for I2C1 (ASDA1/ASCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = ON             // Alternate I2C pins for I2C2 (ASDA2/ASCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable Bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Aux Flash may be written)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)
// End of dsPIC33EP512MU814 configuration fuses ------------------------------//

// dsPeak libraries
#include "timer.h"
#include "uart.h"
#include "codec.h"
#include "spi_flash.h"
#include "dma.h"
#include "rot_encoder.h"

// Access to UART struct members
extern STRUCT_UART UART_struct[UART_QTY];
STRUCT_UART *UART_DEBUG_struct = &UART_struct[UART_3];

// Access to TIMER struct members
extern STRUCT_TIMER TIMER_struct[TIMER_QTY];
STRUCT_TIMER *TIMER1_struct = &TIMER_struct[TIMER_1];
STRUCT_TIMER *TIMER2_struct = &TIMER_struct[TIMER_2];
STRUCT_TIMER *TIMER3_struct = &TIMER_struct[TIMER_3];
STRUCT_TIMER *TIMER4_struct = &TIMER_struct[TIMER_4];
STRUCT_TIMER *TIMER5_struct = &TIMER_struct[TIMER_5];
STRUCT_TIMER *TIMER6_struct = &TIMER_struct[TIMER_6];
STRUCT_TIMER *TIMER7_struct = &TIMER_struct[TIMER_7];
STRUCT_TIMER *TIMER8_struct = &TIMER_struct[TIMER_8];
STRUCT_TIMER *TIMER9_struct = &TIMER_struct[TIMER_9];

// Access to SPI struct members
extern STRUCT_SPI SPI_struct[SPI_QTY];
STRUCT_SPI *SPI_codec = &SPI_struct[SPI_3];

// Access to CODEC struct members
extern STRUCT_CODEC CODEC_struct[CODEC_QTY];
STRUCT_CODEC *CODEC_sgtl5000 = &CODEC_struct[DCI_0];

// Access to FLASH struct members
extern STRUCT_FLASH FLASH_struct[FLASH_QTY];
STRUCT_SPI *SPI_flash = &SPI_struct[SPI_2];

// Access to BUTTON struct members
extern STRUCT_BUTTON BUTTON_struct[BTN_QTY];
STRUCT_BUTTON *BTN1_struct = &BUTTON_struct[BTN_1];
STRUCT_BUTTON *BTN2_struct = &BUTTON_struct[BTN_2];
STRUCT_BUTTON *BTN3_struct = &BUTTON_struct[BTN_3];
STRUCT_BUTTON *BTN4_struct = &BUTTON_struct[BTN_4];

// Access to LED struct members
extern STRUCT_LED LED_struct[BTN_QTY];
STRUCT_LED *LED1_struct = &LED_struct[LED_1];
STRUCT_LED *LED2_struct = &LED_struct[LED_2];
STRUCT_LED *LED3_struct = &LED_struct[LED_3];
STRUCT_LED *LED4_struct = &LED_struct[LED_4];

// Access to ENCODER struct members
extern STRUCT_ENCODER ENCODER_struct[ENCODER_QTY];
STRUCT_ENCODER *ENC1_struct = &ENCODER_struct[ENC_1];

uint16_t i = 0;

uint8_t record_flag = 0, playback_flag = 0, erase_flag = 0;
uint8_t record_cnt = 0, playback_cnt = 0;

uint8_t flash_state_machine = 0;

uint8_t spi_release = 0;
uint8_t DCI_tx_flag = 0;
uint8_t DCI_rx_flag = 0;

uint16_t codec_wr_ptr[CODEC_BLOCK_TRANSFER];
uint8_t spi2_buf[260];

uint16_t * dci_rx_ptr;
uint8_t * spi_rd_ptr;
uint32_t flash_wr_adr = 0;  // 32Mbit = 4Mbyte / 4Byte/sample = 1Msample
uint32_t flash_rd_adr = 0;
uint8_t last_txfer = 0;

uint8_t flash_status1_reg = 0;

uint16_t encoder_rpm = 0;
uint8_t encoder_direction = 0;
uint16_t encoder_old_position = 0;
uint16_t encoder_new_position = 0;

uint8_t hp_vol_l = 0x18;
uint8_t hp_vol_r = 0x18;

uint8_t spi_codec_state = 0;

uint8_t codec_volume_flag = 0;
uint8_t codec_spi_tx_flag = 0;

uint8_t buf_test[9] = {0};

int main() 
{
    dsPeak_init(); 
    
    dsPeak_button_init(BTN1_struct, BTN_1, 10);
    dsPeak_button_init(BTN2_struct, BTN_2, 10);
    dsPeak_button_init(BTN3_struct, BTN_3, 10);
    dsPeak_button_init(BTN4_struct, BTN_4, 10);
 
    dsPeak_led_init(LED1_struct, LED_1, LOW);
    dsPeak_led_init(LED2_struct, LED_2, LOW);
    dsPeak_led_init(LED3_struct, LED_3, LOW);
    dsPeak_led_init(LED4_struct, LED_4, LOW);    

#ifdef UART_DEBUG_ENABLE
    UART_init(UART_DEBUG_struct, UART_3, 115200, UART_MAX_TX, UART_MAX_RX, DMA_CH14);
    while(UART_putstr_dma(UART_DEBUG_struct, "dsPeak UART debug port with DMA is enabled\r\n") != 1);   
#endif

    // TX / RX buf length = 256 (page write) + 4 bytes to provide the address to write
    // CODEC samples are 16bit for each channel, so 32b per stereo sample
    // The flash should save the channel sample one after another
    // 1x page = 256 bytes / (4bytes / sample) = 64 stereo sample / page
    SPI_flash_init(FLASH_struct, SPI_flash, ((CODEC_BLOCK_TRANSFER * 2) + 4), ((CODEC_BLOCK_TRANSFER * 2) + 4), DMA_CH2, DMA_CH1);     
    CODEC_init(CODEC_sgtl5000, SPI_codec, SPI_3, SYS_FS_16kHz, CODEC_BLOCK_TRANSFER, CODEC_BLOCK_TRANSFER, DMA_CH3, DMA_CH0);
    
            
    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER1_struct, TIMER_1, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER2_struct, TIMER_2, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER3_struct, TIMER_3, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);    
    //TIMER_init(TIMER5_struct, TIMER_5, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    
    //TIMER_init(TIMER6_struct, TIMER_6, TIMER_MODE_16B, TIMER_PRESCALER_1, 1000000);
    
    //TIMER_init(TIMER7_struct, TIMER_7, TIMER_PRESCALER_256, 10);
    
    // Non-blocking state machine for flash memory
    TIMER_init(TIMER8_struct, TIMER_8, TIMER_MODE_16B, TIMER_PRESCALER_1, 900000);
    
    //Encoder initialization with associated velocity timer   
    ENCODER_init(ENC1_struct, ENC_1, 30); 
    TIMER_init(TIMER4_struct, TIMER_4, TIMER_MODE_16B, TIMER_PRESCALER_256, ENCODER_get_fs(ENC1_struct));

    TIMER_start(TIMER1_struct);
    TIMER_start(TIMER2_struct);
    TIMER_start(TIMER3_struct);   
    TIMER_start(TIMER4_struct);
    //TIMER_start(TIMER5_struct);
    //TIMER_start(TIMER6_struct);
    //TIMER_start(TIMER7_struct);
    TIMER_start(TIMER8_struct);
    //TIMER_start(TIMER9_struct);
    
    while (1)
    {   
        // Handle DCI transfer DMA interrupt
#ifdef DCI0_DMA_ENABLE
        if (DCI_get_interrupt_state(CODEC_sgtl5000, DCI_DMA_TX) == DCI_TRANSMIT_COMPLETE)
        {
            DCI_tx_flag = 1;
            CODEC_sgtl5000->DMA_tx_buf_pp ^= 1; 
        }

        // Handle DCI receive DMA interrupt        
        if (DCI_get_interrupt_state(CODEC_sgtl5000, DCI_DMA_RX) == DCI_RECEIVE_COMPLETE)
        {
            DCI_rx_flag = 1;
            CODEC_sgtl5000->DMA_rx_buf_pp ^= 1; 
        }
#endif       
        
        // dsPeak on-board button debouncer state machine
        if (TIMER_get_state(TIMER3_struct, TIMER_INT_STATE) == 1)
        { 
            dsPeak_button_debounce(BTN1_struct);
            dsPeak_button_debounce(BTN2_struct);
            dsPeak_button_debounce(BTN3_struct);
            dsPeak_button_debounce(BTN4_struct); 
            
            if (dsPeak_button_get_state(BTN1_struct) == LOW)
            {
                if (BTN1_struct->do_once == 0) 
                {
                    BTN1_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN1 pressed\r\n");
                }
            }
            else
            {
                BTN1_struct->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN2_struct) == LOW)
            {
                if (BTN2_struct->do_once == 0) 
                {
                    BTN2_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN2 pressed\r\n");
                    record_flag = 1;            // CODEC record audio flag
                    //flash_state_machine = 6;
                }
            }
            else
            {
                BTN2_struct->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN3_struct) == LOW)
            {
                if (BTN3_struct->do_once == 0) 
                {
                    BTN3_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN3 pressed\r\n");
                    playback_flag = 1;
                    //flash_state_machine = 6;
                }
            }
            else
            {
                BTN3_struct->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN4_struct) == LOW)
            {
                if (BTN4_struct->do_once == 0) 
                {                   
                    BTN4_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN4 pressed\r\n");
                    erase_flag = 1;
                    //flash_state_machine = 6;
                }
            }
            else
            {
                BTN4_struct->do_once = 0;
            }               
        }   
        
        // CODEC HP volume management
        if (TIMER_get_state(TIMER4_struct, TIMER_INT_STATE) == 1)
        {   
            encoder_direction = ENCODER_get_direction(ENC1_struct);
            encoder_old_position = encoder_new_position;
            encoder_new_position = ENCODER_get_position(ENC1_struct);  
            
            if (playback_flag == 1)
            {  
                if (encoder_new_position > encoder_old_position)
                {
                    if (encoder_direction == 0) // FW, decrease volume
                    {                    
                        dsPeak_led_write(LED3_struct, HIGH);
                        if (++hp_vol_l > 0x7F){hp_vol_l = 0x7F;}
                        if (++hp_vol_r > 0x7F){hp_vol_r = 0x7F;}  
                        UART_putstr_dma(UART_DEBUG_struct, "Volume DOWN requested\r\n");
                    }
                    else
                    {
                        dsPeak_led_write(LED3_struct, LOW);
                        if (--hp_vol_l < 1){hp_vol_l = 1;}
                        if (--hp_vol_r < 1){hp_vol_r = 1;}
                        UART_putstr_dma(UART_DEBUG_struct, "Volume UP requested\r\n");
                    }
                    if (SPI_module_busy(CODEC_struct->spi_ref) == SPI_MODULE_FREE)
                    {
                        CODEC_set_hp_volume(CODEC_struct, hp_vol_r, hp_vol_l);
                    }
                }
            }
        }  
//        
        if (TIMER_get_state(TIMER5_struct, TIMER_INT_STATE) == 1)
        {
        } 
        
        if (TIMER_get_state(TIMER6_struct, TIMER_INT_STATE) == 1)
        {
        } 
//        
//        
        if (TIMER_get_state(TIMER7_struct, TIMER_INT_STATE) == 1)
        {           
        }
        
        if (TIMER_get_state(TIMER8_struct, TIMER_INT_STATE) == 1)
        {   
            // SPI FLASH and CODEC state machine to record / playback audio            
            // If spi_release flag is set, call deassert_cs and get_rx_buf functions
            if (spi_release == 1)
            {
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_rx_channel) == DMA_TXFER_DONE)
                {
                    spi_release = 0;
                    SPI_deassert_cs(FLASH_struct->spi_ref);                    
                }
            }                 

            // Query the device busy status
            if (flash_state_machine == 0)
            {   
                dsPeak_led_write(LED4_struct, HIGH);
                // DMA channel is in idle, ready for another transfer
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {
                    spi_release = 1;
                    if (SPI_flash_busy(FLASH_struct) == 1)
                    {                        
                        flash_state_machine = 1;
                    }
                }
            }

            // Query the device busy status
            else if (flash_state_machine == 1)
            {
                // Status register value is at offset 1 (2nd byte received)
                flash_status1_reg = SPI_get_dma_rx_buffer_at_index(FLASH_struct->spi_ref, 1);
                if ((flash_status1_reg & 0x01) == 0)
                {
                    // Busy indicator low, flash is ready
                    flash_state_machine = 2;
                }   
                else
                {
                    // Busy indicator high, keep polling
                    flash_state_machine = 0;
                }
            }

            // Write enable
            else if (flash_state_machine == 2)                     
            {
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {
                    spi_release = 1;
                    if (SPI_flash_write_enable(FLASH_struct) == 1)
                    {
                        flash_state_machine = 3;
                    }
                }
            }

            // FLASH erase all
            else if (flash_state_machine == 3)
            {
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {
                    spi_release = 1;
                    if (SPI_flash_erase(FLASH_struct, CMD_CHIP_ERASE, 0) == 1)
                    {
                        flash_state_machine = 4;
                    }
                }
            }

            // 
            else if (flash_state_machine == 4)
            {
                // DMA channel is in idle, ready for another transfer
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {
                    spi_release = 1;
                    if (SPI_flash_busy(FLASH_struct) == 1)  // Query flash busy flag
                    {                        
                        flash_state_machine = 5;              
                    }
                }                
            }

            else if (flash_state_machine == 5)
            {
                // Status register value is at offset 1 (2nd byte received)
                flash_status1_reg = SPI_get_dma_rx_buffer_at_index(FLASH_struct->spi_ref, 1);               
                if ((flash_status1_reg & 0x01) == 0)
                {
                    // Busy indicator low, flash is ready
                    erase_flag = 0;
                    flash_state_machine = 6;
                    dsPeak_led_write(LED4_struct, HIGH);
                }   
                else
                {
                    // Busy indicator high, keep polling
                    flash_state_machine = 4;
                }                
            }

            // State machine is idle, waiting for user input
            else if (flash_state_machine == 6)
            {    
                dsPeak_led_write(LED4_struct, LOW);
                if (record_flag == 1) 
                {  
                    flash_state_machine = 7;
                    DCI_rx_flag = 0;
                    CODEC_mute(CODEC_sgtl5000, HEADPHONE_MUTE);                    
                }
                
                if (playback_flag == 1)
                {
                    flash_state_machine = 12;
                    DCI_tx_flag = 0;                              
                    CODEC_unmute(CODEC_sgtl5000, HEADPHONE_MUTE);
                }
                
                if (erase_flag == 1) 
                {
                    spi_release = 1;
                    flash_state_machine = 0;
                }
            }

            // At this point the flash has been erased and is ready to be written
            // The record button has been pressed, setting the record flag
            // Store data from CODEC to memory (record mode)
            else if (flash_state_machine == 7)
            {                                
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {
                    dsPeak_led_write(LED4_struct, HIGH);    // Debug
                    spi_release = 1;
                    if (SPI_flash_write_enable(FLASH_struct) == 1)  // Set write enable bit in flash
                    {
                        flash_state_machine = 8;
                    }
                }
            }
            
            // Must validate if WEL = 1 in status register before programming a page
            else if (flash_state_machine == 8)
            {
                // DMA channel is in idle, ready for another transfer
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {
                    spi_release = 1;
                    if (SPI_flash_busy(FLASH_struct) == 1)  // Query flash busy flag
                    {                        
                        flash_state_machine = 9;              
                    }
                }                   
            }
            
            else if (flash_state_machine == 9)
            {    
                // Status register value is at offset 1 (2nd byte received)
                flash_status1_reg = SPI_get_dma_rx_buffer_at_index(FLASH_struct->spi_ref, 1);
                if ((flash_status1_reg & 0x03) == 2)
                {
                    // WEL = 1 and BUSY = 0, flash is ready
                    flash_state_machine = 10;                    
                }
                else
                {
                    // WEL = 0 or BUSY = 1, keep polling
                    flash_state_machine = 7;
                }                       
            }

            // Send data to flash here
            else if (flash_state_machine == 10)
            {          
                // Transfer SPI data from CODEC DMA RX buffer to SPI DMA TX buffer
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {    
                    spi_release = 1;
                    if (DCI_rx_flag == 1)       // DCI RX buffer full
                    {   
                        DCI_rx_flag = 0;                       
                        // Transfer data between DCI RX buffer and SPI TX buffer                       
                        spi2_buf[0] = CMD_PAGE_PROGRAM;
                        spi2_buf[1] = ((flash_wr_adr & 0xFF0000)>>16);
                        spi2_buf[2] = ((flash_wr_adr & 0x00FF00)>>8);
                        spi2_buf[3] = flash_wr_adr&0x0000FF;
                        dci_rx_ptr = DCI_unload_dma_rx_buf(CODEC_sgtl5000, CODEC_BLOCK_TRANSFER);   // 1x loop 128 iterations
                        uint16_to_byte8(&dci_rx_ptr[0], &spi2_buf[4], CODEC_BLOCK_TRANSFER);                              // 1x loop 128 iterations                      
                        
                        if (SPI_load_dma_tx_buffer(FLASH_struct->spi_ref, spi2_buf, 260) == 1)      // 1x loop (260 iterations)
                        {
                            SPI_write_dma(FLASH_struct->spi_ref, FLASH_MEMORY_CS);
                            flash_wr_adr += 256;
                            if (flash_wr_adr > 0x3FFFFF)    // written last memory slot
                            {       
                                flash_state_machine = 11;   // Recording is finished
                            }
                            else
                            {
                                flash_state_machine = 7;    // Continue recording
                            }
                        }                        
                    }                    
                }
            }

            // Recording finished
            else if (flash_state_machine == 11)
            {
                record_flag = 0;   
                flash_wr_adr = 0;                       // Reset write address to 0
                record_cnt = 1;                         // A record was completed
                flash_state_machine = 6;                // Return to record wait state
            }

            // Playback started
            // Fetch data from SPI memory
            else if (flash_state_machine == 12)
            {
                dsPeak_led_write(LED4_struct, HIGH);    // Debug
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_tx_channel) == DMA_TXFER_IDLE)
                {    
                    spi_release = 1;
                    if (SPI_flash_read_page(FLASH_struct, flash_rd_adr) == 1)
                    {
                        flash_rd_adr += 256;
                        if (flash_rd_adr > 0x3FFFFF)
                        {
                            last_txfer = 1;
                        }
                        flash_state_machine = 13;
                    }
                }              
            }

            // Transfer memory data to DCI buffer
            else if (flash_state_machine == 13)
            {
                if (DMA_get_txfer_state(FLASH_struct->spi_ref->DMA_rx_channel) == DMA_TXFER_IDLE)
                {    
                    spi_release = 1;
                    // SPI read is complete, parse buffers
                    spi_rd_ptr = SPI_unload_dma_rx_buffer(FLASH_struct->spi_ref);
                    byte8_to_uint16(&spi_rd_ptr[4], &codec_wr_ptr[0], CODEC_BLOCK_TRANSFER);           
                    flash_state_machine = 14;                         
                }
            }

            else if (flash_state_machine == 14)
            {
                if (DCI_tx_flag == 1)
                {
                    DCI_tx_flag = 0;
                    DCI_fill_dma_tx_buf(CODEC_sgtl5000, codec_wr_ptr, CODEC_BLOCK_TRANSFER);
                    if (last_txfer == 0)
                    {
                        flash_state_machine = 12;
                    }
                    else
                    {
                        flash_state_machine = 15;
                    }                            
                }                
            }

            else if (flash_state_machine == 15)
            {
                CODEC_mute(CODEC_sgtl5000, HEADPHONE_MUTE);
                playback_flag = 0;
                flash_rd_adr = 0;
                playback_cnt = 1;
                last_txfer = 0;
                flash_state_machine = 6;
            }
            
            else
            {
                // Nothing to do here
                flash_state_machine = 6;    // Put state machine to IDLE                
            }
        } 
               
        if (TIMER_get_state(TIMER9_struct, TIMER_INT_STATE) == 1)
        {
        }      
    }
    return 0;
}
