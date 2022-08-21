//****************************************************************************//
// File      :  main.c
//
// Includes  :  general.h
//              Project-related .h files
//
// Purpose   : dsPeak development project
//
// Intellitrol                   MPLab X v5.45                        06/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "dspeak_generic.h"

//----------------------------------------------------------------------------//
// dsPIC33EP512MU814 configuration fuses, do not remove
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
#include "rc_dac.h"
#include "adc.h"
#include "rtcc.h"
#include "timer.h"
#include "uart.h"
#include "codec.h"
#include "pwm.h"
#include "qei.h"
#include "spi_flash.h"
#include "mcontrol.h"
#include "CAN.h"
#include "image_file.h"
#include "ST7735s.h"
#include "dma.h"
#include "i2c.h"
#include "rot_encoder.h"
#include "ividac_driver.h"
// End of dsPeak libraries

// Access to CAN struct members
extern STRUCT_CAN CAN_struct[CAN_QTY];
STRUCT_CAN *CAN1_struct = &CAN_struct[CAN_1];
STRUCT_CAN_MSG CAN_MSG_DSPEAK;

// Access to RCDAC struct members
extern STRUCT_RCDAC RCDAC_struct[RCDAC_QTY];
STRUCT_RCDAC *RCDAC1_struct = &RCDAC_struct[RCDAC_1];
STRUCT_RCDAC *RCDAC2_struct = &RCDAC_struct[RCDAC_2];

// Access to PWM struct members
extern STRUCT_PWM PWM_struct[PWM_QTY];
STRUCT_PWM *PWM1L_struct = &PWM_struct[PWM_1L];
STRUCT_PWM *PWM1H_struct = &PWM_struct[PWM_1H];
STRUCT_PWM *PWM5H_struct = &PWM_struct[PWM_5H];
STRUCT_PWM *PWM6L_struct = &PWM_struct[PWM_6L];
STRUCT_PWM *PWM6H_struct = &PWM_struct[PWM_6H];

// Access to I2C struct members
extern STRUCT_I2C i2c_struct[I2C_QTY];
STRUCT_I2C *I2C1_struct = &i2c_struct[I2C_1];           // MikroBus I2C port, 100kHz
STRUCT_I2C *I2C2_struct = &i2c_struct[I2C_2];           // RC-DAC I2C port, 400kHz

// Access to UART struct members
extern STRUCT_UART UART_struct[UART_QTY];
STRUCT_UART *UART_485_struct = &UART_struct[UART_1];
STRUCT_UART *UART_MKB_struct = &UART_struct[UART_2];
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
STRUCT_BUTTON *BTN1_dsPeak = &BUTTON_struct[BTN_1];
STRUCT_BUTTON *BTN2_dsPeak = &BUTTON_struct[BTN_2];
STRUCT_BUTTON *BTN3_dsPeak = &BUTTON_struct[BTN_3];
STRUCT_BUTTON *BTN4_dsPeak = &BUTTON_struct[BTN_4];

// Access to LED struct members
extern STRUCT_LED LED_struct[BTN_QTY];
STRUCT_LED *LED1_dsPeak = &LED_struct[LED_1];
STRUCT_LED *LED2_dsPeak = &LED_struct[LED_2];
STRUCT_LED *LED3_dsPeak = &LED_struct[LED_3];
STRUCT_LED *LED4_dsPeak = &LED_struct[LED_4];

STRUCT_ADC ADC_struct_AN0, ADC_struct_AN1, ADC_struct_AN2, ADC_struct_AN12, 
            ADC_struct_AN13, ADC_struct_AN14, ADC_struct_AN15;

uint8_t *flash_rx_data;
uint8_t i = 0;

RTCC_time clock;
uint8_t hour, minute, second;
int error_rpm;

uint16_t data_rx_can[4] = {0};
uint8_t can_parse_return;
uint8_t I2C_buf[4] = {0};
uint16_t i2c_sine_counter = 0;

uint8_t record_flag = 0, playback_flag = 0, erase_flag = 0;
uint8_t record_cnt = 0, playback_cnt = 0;

uint8_t *u485_rx_buf1;
uint8_t *u485_rx_buf2;
uint8_t u485_1_data_flag = 0;
uint8_t u485_2_data_flag = 0;
uint8_t counter_485 = 0;

uint8_t new_pid_out1 = 0;
uint16_t new_rpm_ain = 0;
uint32_t tracker = 0, color = 0;
uint16_t val = 0x7FFF;
uint8_t tag = 0;
uint8_t tag_val_write = 1;
uint16_t track_upd = 0;
uint8_t encoder_dir = 0;

uint16_t encoder_rpm = 0;
uint16_t encoder_tour = 0;

uint8_t btn1_debounce = 1;
uint8_t btn4_debounce = 1;

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

uint8_t pwm_cnt = 0, pwm_duty = 0;
uint8_t wiper = 255;

int main() 
{
    dsPeak_init();
    dsPeak_button_init(BTN1_dsPeak, BTN_1, 10);
    dsPeak_button_init(BTN2_dsPeak, BTN_2, 10);
    dsPeak_button_init(BTN3_dsPeak, BTN_3, 10);
    dsPeak_button_init(BTN4_dsPeak, BTN_4, 10);
 
    dsPeak_led_init(LED1_dsPeak, LED_1, LOW);
    dsPeak_led_init(LED2_dsPeak, LED_2, LOW);
    dsPeak_led_init(LED3_dsPeak, LED_3, LOW);
    dsPeak_led_init(LED4_dsPeak, LED_4, LOW);    
    
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    
    LATCbits.LATC2 = 0;
    LATCbits.LATC3 = 0;
    LATCbits.LATC4 = 0;
    
    
    RTCC_init();
    RTCC_write_time(clock);

#ifdef UART_DEBUG_ENABLE
    UART_init(UART_DEBUG_struct, UART_3, 115200, UART_MAX_TX, UART_MAX_RX, DMA_CH6);
    UART_putstr_dma(UART_DEBUG_struct, "dsPeak UART debug port with DMA is enabled\r\n");   
#endif

    //MOTOR_init(PWM1H_struct, PWM1L_struct, MOTOR_1, 30);
    //MOTOR_init(MOTOR_2, 30);  
    
//    SPI_init(SPI_4, SPI_MODE0, PPRE_4_1, SPRE_8_1);     // Max sclk of 9MHz on SPI4 = divide by 8
//#ifdef IVIDAC_RESOLUTION_12BIT
//    IVIDAC_init(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, IVIDAC_RESOLUTION_12BIT, AD5621_OUTPUT_NORMAL, IVIDAC_OUTPUT_DISABLE);  
//    IVIDAC_init(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, IVIDAC_RESOLUTION_12BIT, AD5621_OUTPUT_NORMAL, IVIDAC_OUTPUT_DISABLE);
//#endif
//    
//#ifdef IVIDAC_RESOLUTION_14BIT
//    IVIDAC_init(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, IVIDAC_RESOLUTION_14BIT, AD5641_OUTPUT_NORMAL, IVIDAC_OUTPUT_DISABLE);  
//    IVIDAC_init(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, IVIDAC_RESOLUTION_14BIT, AD5641_OUTPUT_NORMAL, IVIDAC_OUTPUT_DISABLE);    
//#endif
//   
    
    //PWM_init(PWM5H_struct, PWM_5H, PWM_TYPE_SERVO);
    
    //I2C_init(I2C2_struct, I2C_2, I2C_mode_master, 0);
    //RCDAC_init(RCDAC1_struct, PWM6L_struct, I2C2_struct, RCDAC_1, 0);
    //RCDAC_init(RCDAC2_struct, PWM6H_struct, I2C2_struct, RCDAC_2, 0);
    //PWM_change_duty_perc(PWM6L_struct, 50);
    //PWM_change_duty_perc(PWM6H_struct, 50);
    //PWM_init(PWM_6L, PWM_TYPE_SERVO); 

    // TX / RX buf length = 256 (page write) + 4 bytes to provide the address to write
    // CODEC samples are 16bit for each channel, so 32b per stereo sample
    // The flash should save the channel sample one after another
    // 1x page = 256 bytes / (4bytes / sample) = 64 stereo sample / page
    SPI_flash_init(FLASH_struct, SPI_flash, ((CODEC_BLOCK_TRANSFER * 2) + 4), ((CODEC_BLOCK_TRANSFER * 2) + 4), DMA_CH5, DMA_CH4);     
    CODEC_init(CODEC_sgtl5000, SPI_codec, SPI_3, SYS_FS_16kHz, CODEC_BLOCK_TRANSFER, CODEC_BLOCK_TRANSFER, DMA_CH7, DMA_CH8);
    DCI_set_receive_state(CODEC_sgtl5000, DCI_RECEIVE_ENABLE);
    DCI_set_transmit_state(CODEC_sgtl5000, DCI_TRANSMIT_ENABLE);
    
    // Enable dsPeak native RS-485 port @ 460800bps
    UART_init(UART_485_struct, UART_1, 460800, 32, 32, DMA_CH0);
    
#ifdef RS485_CLICK_UART2
    // Enable RS-485 Click6 RS-485 interface on MikroBus port @ 460800bps
    UART_init(UART_MKB_struct, UART_2, 460800, 32, 32, DMA_CH1);
#endif
    
    //CAN_init_message(&CAN_MSG_DSPEAK, 0x0123, 0, 0, 0, 0, 0, 0, 0x0300, 0, 1, 8, 8, CAN_NODE_TYPE_TX_RX);
    //CAN_init(CAN1_struct, CAN_1, 500000, 8, DMA_CH2, DMA_CH3, CAN_NODE_TYPE_TX_RX);
    
//#ifdef BRINGUP_DSPEAK_1    
//    CAN_init_struct(&CAN_native, CAN_1, 500000, 0x0123, 0, 0x0300, 0x0300);
//    CAN_init(&CAN_native);
//    CAN_set_mode(&CAN_native, CAN_MODE_NORMAL);
//    CAN_fill_payload(&CAN_native, can_buf_1, 8);
//#endif
//    
//#ifdef BRINGUP_DSPEAK_2
//    CAN_init_struct(&CAN_native, CAN_1, 500000, 0x0300, 0, 0x0123, 0x0123);
//    CAN_init(&CAN_native);
//    CAN_set_mode(&CAN_native, CAN_MODE_NORMAL);
//    CAN_fill_payload(&CAN_native, can_buf_2, 8);    
//#endif
//    
//    ADC_init_struct(&ADC_struct_AN12, ADC_PORT_1, ADC_CHANNEL_AN12, 
//                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);
//    ADC_init_struct(&ADC_struct_AN13, ADC_PORT_1, ADC_CHANNEL_AN13, 
//                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100); 
//    ADC_init_struct(&ADC_struct_AN14, ADC_PORT_1, ADC_CHANNEL_AN14, 
//                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);
//    ADC_init_struct(&ADC_struct_AN15, ADC_PORT_1, ADC_CHANNEL_AN15, 
//                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100); 
//    
//    ADC_init_struct(&ADC_struct_AN0, ADC_PORT_2, ADC_CHANNEL_AN0, 
//                    ADC_RESOLUTION_10b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);
//    ADC_init_struct(&ADC_struct_AN1, ADC_PORT_2, ADC_CHANNEL_AN1, 
//                    ADC_RESOLUTION_10b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100); 
//    ADC_init_struct(&ADC_struct_AN2, ADC_PORT_2, ADC_CHANNEL_AN2, 
//                    ADC_RESOLUTION_10b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);    
// 
//    ADC_init(&ADC_struct_AN0);
//    ADC_init(&ADC_struct_AN1);
//    ADC_init(&ADC_struct_AN2); 
//    
//    ADC_start(ADC_PORT_1);      // Start ADC1
//    ADC_start(ADC_PORT_2);      // Start ADC2
//    
//    MOTOR_set_rpm(MOTOR_1, speed_rpm_table[0]);
//    MOTOR_set_rpm(MOTOR_2, speed_rpm_table[0]);  

//    PMP_init(PMP_MODE_SRAM);
//    for (i=0; i<10; i++)
//    {
//        PMP_write_single(PMP_MODE_SRAM, mem_adr[i], mem_dat[i]);
//        if (PMP_read_single(PMP_MODE_SRAM, mem_adr[i]) != mem_dat[i])
//        {
//            LATHbits.LATH9 = !LATHbits.LATH9;
//        }   
//        else
//        {
//        }  
//        __delay_ms(100);
//    }
//    
    PMP_init(PMP_MODE_TFT);
    ST7735_init();
    color = RGB888_to_RGB565(0x555500);
    ST7735_Clear(color);
            
    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER1_struct, TIMER_1, TIMER_MODE_16B, TIMER_PRESCALER_256, 5);
    TIMER_init(TIMER2_struct, TIMER_2, TIMER_MODE_16B, TIMER_PRESCALER_256, 5);
    TIMER_init(TIMER3_struct, TIMER_3, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);    
    //TIMER_init(TIMER4_struct, TIMER_4, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    //TIMER_init(TIMER5_struct, TIMER_5, TIMER_MODE_16B, TIMER_PRESCALER_256, QEI_get_fs(QEI_1));
    
    //TIMER_init(TIMER6_struct, TIMER_6, TIMER_MODE_16B, TIMER_PRESCALER_1, 1000000);
    
    //TIMER_init(TIMER7_struct, TIMER_7, TIMER_PRESCALER_256, 10);
    
    // Non-blocking state machine for flash memory
    TIMER_init(TIMER8_struct, TIMER_8, TIMER_MODE_16B, TIMER_PRESCALER_1, 800000);
    
    // Encoder initialization with associated velocity timer   
    //ENCODER_init(10); 
    //TIMER_init(TIMER9_struct, TIMER_9, TIMER_MODE_16B, TIMER_PRESCALER_256, ENCODER_get_fs());
                                  // 
    TIMER_start(TIMER1_struct);
    TIMER_start(TIMER2_struct);
    TIMER_start(TIMER3_struct);   
    //TIMER_start(TIMER4_struct);
    //TIMER_start(TIMER5_struct);
    //TIMER_start(TIMER6_struct);
    //TIMER_start(TIMER7_struct);
    TIMER_start(TIMER8_struct);
    //TIMER_start(TIMER9_struct);

    while (1)
    {                                           
        if (UART_rx_done(UART_485_struct) == UART_RX_COMPLETE)
        {            
            u485_1_data_flag = 1;
            dsPeak_led_write(LED1_dsPeak, LOW);
        } 

        if (UART_rx_done(UART_MKB_struct) == UART_RX_COMPLETE)
        {           
            u485_2_data_flag = 1; 
            dsPeak_led_write(LED2_dsPeak, LOW);
        }
        
        // Handle DCI transfer DMA interrupt
#ifdef DCI0_DMA_ENABLE
        if (DCI_get_interrupt_state(CODEC_sgtl5000, DCI_DMA_TX) == DCI_TRANSMIT_COMPLETE)
        {
            DCI_tx_flag = 1;
            LATCbits.LATC4 = 1;
            CODEC_sgtl5000->DMA_tx_buf_pp ^= 1; 
        }

        // Handle DCI receive DMA interrupt        
        if (DCI_get_interrupt_state(CODEC_sgtl5000, DCI_DMA_RX) == DCI_RECEIVE_COMPLETE)
        {
            DCI_rx_flag = 1;
            LATCbits.LATC3 = 1;
            CODEC_sgtl5000->DMA_rx_buf_pp ^= 1; 
        }
#endif
        
        if (TIMER_get_state(TIMER1_struct, TIMER_INT_STATE) == 1)
        {
            if (u485_1_data_flag == 1)
            {                
                u485_1_data_flag = 0;
                UART_putstr_dma(UART_485_struct, "Message sent from native port!\r\n");
                dsPeak_led_write(LED1_dsPeak, HIGH);
            }
        }       

        if (TIMER_get_state(TIMER2_struct, TIMER_INT_STATE) == 1)
        {
#ifdef RS485_CLICK_UART2               
            if (u485_2_data_flag == 1)
            {                
                u485_2_data_flag = 0;
                UART_putstr_dma(UART_MKB_struct, "Message sent from MikroB port!\r\n");
                dsPeak_led_write(LED2_dsPeak, HIGH);
            }              
#endif      
        }         
        
        // dsPeak on-board button debouncer state machine
        if (TIMER_get_state(TIMER3_struct, TIMER_INT_STATE) == 1)
        { 
            dsPeak_button_debounce(BTN1_dsPeak);
            dsPeak_button_debounce(BTN2_dsPeak);
            dsPeak_button_debounce(BTN3_dsPeak);
            dsPeak_button_debounce(BTN4_dsPeak); 
            
            if (dsPeak_button_get_state(BTN1_dsPeak) == LOW)
            {
                if (BTN1_dsPeak->do_once == 0) 
                {
                    BTN1_dsPeak->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN1 pressed\r\n");
                    UART_putstr_dma(UART_485_struct, "Message sent from native port!\r\n");
                }
            }
            else
            {
                BTN1_dsPeak->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN2_dsPeak) == LOW)
            {
                if (BTN2_dsPeak->do_once == 0) 
                {
                    BTN2_dsPeak->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN2 pressed\r\n");
                    record_flag = 1;            // CODEC record audio flag
                    flash_state_machine = 6;
                }
            }
            else
            {
                BTN2_dsPeak->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN3_dsPeak) == LOW)
            {
                if (BTN3_dsPeak->do_once == 0) 
                {
                    BTN3_dsPeak->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN3 pressed\r\n");
                    playback_flag = 1;
                    flash_state_machine = 6;
                }
            }
            else
            {
                BTN3_dsPeak->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN4_dsPeak) == LOW)
            {
                if (BTN4_dsPeak->do_once == 0) 
                {                   
                    BTN4_dsPeak->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN4 pressed\r\n");
                    erase_flag = 1;
                    flash_state_machine = 6;
                }
            }
            else
            {
                BTN4_dsPeak->do_once = 0;
            }               
        }   
        
        if (TIMER_get_state(TIMER4_struct, TIMER_INT_STATE) == 1)
        {
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
            LATCbits.LATC2 = !LATCbits.LATC2;
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
                dsPeak_led_write(LED4_dsPeak, HIGH);
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
                    dsPeak_led_write(LED4_dsPeak, HIGH);
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
                dsPeak_led_write(LED4_dsPeak, LOW);
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
                    dsPeak_led_write(LED4_dsPeak, HIGH);    // Debug
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
                        uint16_to_byte8(&dci_rx_ptr[0], &spi2_buf[4]);                              // 1x loop 128 iterations                      
                        
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
                dsPeak_led_write(LED4_dsPeak, HIGH);    // Debug
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
                    byte8_to_uint16(&spi_rd_ptr[4], &codec_wr_ptr[0]);           
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
            }
        } 
               
//        if (TIMER_get_state(TIMER9_struct, TIMER_INT_STATE) == 1)
//        {
//            //encoder_rpm = ENCODER_get_velocity();
//        }

//        if (TIMER_get_state(TIMER2_struct, TIMER_INT_STATE) == 1)
//        {           
//#ifdef EVE_SCREEN_ENABLE 
//            if (encoder_dir == 0)
//            {
//                FT8XX_modify_element_string(13, FT_PRIM_TEXT, "Forward ");
//            }
//            else
//            {
//                FT8XX_modify_element_string(13, FT_PRIM_TEXT, "Backward");
//            }            
//            
//            FT8XX_modify_number(&st_Number[0], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN12));
//            FT8XX_modify_number(&st_Number[1], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN13));
//            FT8XX_modify_number(&st_Number[2], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN14));
//            FT8XX_modify_number(&st_Number[3], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN15));
//            FT8XX_modify_number(&st_Number[4], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN0));
//            FT8XX_modify_number(&st_Number[5], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN1));
//            FT8XX_modify_number(&st_Number[6], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN2));
//            FT8XX_modify_number(&st_Number[7], NUMBER_VAL, encoder_rpm);
//            FT8XX_modify_number(&st_Number[8], NUMBER_VAL, encoder_tour);
//            //tracker = FT8XX_rd32(REG_TRACKER);
//                       
//            FT8XX_start_new_dl();					// Start a new display list, reset ring buffer and ring pointer
//            FT8XX_write_dl_long(TAG_MASK(1));
//            FT8XX_write_dl_long(CMD_COLDSTART);
//            
//            FT8XX_write_dl_long(TAG(st_Gradient[0].touch_tag));
//            FT8XX_draw_gradient(&st_Gradient[0]);
//
//            FT8XX_draw_text(&st_Text[0]);
//
//            FT8XX_draw_text(&st_Text[1]);  
//
//            FT8XX_draw_text(&st_Text[3]);
//            FT8XX_draw_text(&st_Text[4]);
//            FT8XX_draw_text(&st_Text[5]);
//            FT8XX_draw_text(&st_Text[6]);
//            FT8XX_draw_text(&st_Text[7]);
//            FT8XX_draw_text(&st_Text[8]);
//            FT8XX_draw_text(&st_Text[9]);
//            FT8XX_draw_text(&st_Text[10]);
//            FT8XX_draw_text(&st_Text[11]);
//            FT8XX_draw_text(&st_Text[12]);
//
//            FT8XX_draw_text(&st_Text[13]);
//            FT8XX_draw_text(&st_Text[14]);   
//            FT8XX_draw_text(&st_Text[15]); 
//
//            FT8XX_draw_number(&st_Number[0]);
//            FT8XX_draw_number(&st_Number[1]);
//            FT8XX_draw_number(&st_Number[2]);
//            FT8XX_draw_number(&st_Number[3]);
//            FT8XX_draw_number(&st_Number[4]);
//            FT8XX_draw_number(&st_Number[5]);
//            FT8XX_draw_number(&st_Number[6]);
//            FT8XX_draw_number(&st_Number[7]);
//            FT8XX_draw_number(&st_Number[8]);
//
//            FT8XX_write_dl_long(BEGIN(LINES));
//            FT8XX_write_dl_long(VERTEX2II(305, 45, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 45, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 220, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 220, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 45, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 220, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 45, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 220, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(395, 25, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(395, 220, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 70, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 70, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 95, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 95, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 120, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 120, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 145, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 145, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 170, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 170, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(305, 195, 0, 0));
//            FT8XX_write_dl_long(VERTEX2II(470, 195, 0, 0));               
//            
//            FT8XX_write_dl_long(TAG(st_Button[0].touch_tag));
//            FT8XX_draw_button(&st_Button[0]);
//
//            FT8XX_draw_text(&st_Text[2]);   
//
//            FT8XX_write_dl_long(COLOR_RGB(85, 170, 0));
//            FT8XX_set_context_fcolor(0xFDFF59);
//            FT8XX_write_dl_long(COLOR_RGB(0, 0, 0));
//
//            FT8XX_CMD_tracker(st_Slider[0].x, st_Slider[0].y, st_Slider[0].w, st_Slider[0].h, st_Slider[0].touch_tag);
//            FT8XX_write_dl_long(TAG(st_Slider[0].touch_tag));
//            FT8XX_draw_slider(&st_Slider[0]);  
//
//            FT8XX_CMD_tracker(st_Dial[0].x, st_Dial[0].y, 1, 1, st_Dial[0].touch_tag);
//            FT8XX_write_dl_long(TAG(st_Dial[0].touch_tag));
//            FT8XX_draw_dial(&st_Dial[0]);
//            FT8XX_write_dl_long(COLOR_RGB(255, 255, 255));
//            
//            if (ENCODER_get_switch_state() == 0)     
//            {
//                
//                FT8XX_set_context_fcolor(0x00FF00);
//                FT8XX_draw_button(&st_Button[1]);
//            }
//            else
//            {
//                FT8XX_set_context_fcolor(0xFF0000);
//                FT8XX_draw_button(&st_Button[1]);
//            }
//            
//            FT8XX_update_screen_dl();         		// Update display list  
//                        
//            tag = FT8XX_read_touch_tag();         
//            tracker = FT8XX_rd32(REG_TRACKER); 
//            
//            if (tag != 0)
//            {
//                
//                if ((tracker & 0x0000FFFF) == st_Slider[0].touch_tag)
//                {
//                    track_upd = ((tracker&0xFFFF0000)>>16);
//                    FT8XX_modify_slider(&st_Slider[0], SLIDER_VAL, track_upd);
//                    track_upd = (track_upd >>9); // Divide by 512 to get a range of 0-128
//                    if (track_upd < 10){track_upd = 10;}
//                    FT8XX_wr8(REG_PWM_DUTY, track_upd);
//                }
//                
//                else if ((tracker & 0x0000FFFF) == st_Dial[0].touch_tag)
//                {
//                    track_upd = ((tracker&0xFFFF0000)>>16);
//                    FT8XX_modify_dial(&st_Dial[0], DIAL_VALUE, track_upd);
//                }
//            }
//            
//            if (tag == st_Button[0].touch_tag)
//            {
//                while(((FT8XX_rd32(REG_TOUCH_DIRECT_XY)) & 0x8000) == 0x0000);
//                FT8XX_touchpanel_calibrate();
//                FT8XX_clear_touch_tag();
//                while(((FT8XX_rd32(REG_TOUCH_DIRECT_XY)) & 0x8000) == 0x0000);
//            }        
//#endif
//        }         
        
//        if (C1RXFUL1bits.RXFUL1 == 1)
//        {
//            C1RXFUL1bits.RXFUL1 = 0;
//            if (CAN_parse_rxmsg(&CAN_native) == 1)
//            {
//                DSPEAK_LED4_STATE = !DSPEAK_LED4_STATE;
//                #ifdef UART_DEBUG_ENABLE
//                UART_putstr(UART_3, "CAN RX data = ");
//                UART_putbuf(UART_3, &CAN_native.can_rx_payload[0], 8);             
//                UART_putc(UART_3, 0x0D);
//                UART_putc(UART_3, 0x0A);
//                #endif
//            }
//        } 

//        if (UART_rx_done(UART_1) == UART_RX_COMPLETE)
//        {
//            u485_rx_buf = UART_get_rx_buffer(UART_1);
//            DSPEAK_LED3_STATE = !DSPEAK_LED3_STATE;
//        }          
        
//        if (TIMER_get_state(TIMER_3, TIMER_INT_STATE) == 1)
//        { 
//            if (++counter_sec >= 60)
//            {    
//                if (CAN_tx_state == 0)
//                {
//                    CAN_tx_return = CAN_send_txmsg(&CAN_native);
//                    #ifdef UART_DEBUG_ENABLE
//                    UART_putstr(UART_3, "CAN tx error counter = ");
//                    UART_putc_ascii(UART_3, retry_counter);
//                    UART_putc(UART_3, 0x0D);
//                    UART_putc(UART_3, 0x0A);
//                    #endif                    
//                    DSPEAK_LED1_STATE = !DSPEAK_LED1_STATE;
//                }
//
//                if (CAN_tx_return == 3)     // Remote node did not acknowledge
//                {
//                    retry_counter = CAN_get_txmsg_errcnt(&CAN_native);
//                    if (retry_counter > CAN_MAXIMUM_TX_RETRY)
//                    {
//                        CAN_tx_state = 1;
//                        C1TR01CONbits.TXREQ0 = 0x0; 
//                    }
//                }
//                              
//                RTCC_read_time();
//                hour = RTCC_get_time_parameter(RTC_HOUR);
//                minute = RTCC_get_time_parameter(RTC_MINUTE);
//                second = RTCC_get_time_parameter(RTC_SECOND);
//#ifdef EVE_SCREEN_ENABLE
//                //FT8XX_modify_clock_hms(&st_Clock[0], hour, minute, second);
//#endif
//                counter_sec = 0; 
//            }
//        }        

        // RS-485 test
//        if (TIMER_get_state(TIMER_4, TIMER_INT_STATE) == 1)
//        { 
//            if (++counter_timer4 > 5)
//            {         
//                counter_timer4 = 0;
//                #ifdef BRINGUP_DSPEAK_1
//                UART_putstr(UART_1, "dsPeak1 sent this message");                 
//                #ifdef UART_DEBUG_ENABLE
//                UART_putstr(UART_3, "RS-485 data rx'd = ");
//                UART_putbuf(UART_3, u485_rx_buf, 25);             
//                UART_putc(UART_3, 0x0D);
//                UART_putc(UART_3, 0x0A);                      
//                #endif
//                DSPEAK_LED2_STATE = !DSPEAK_LED2_STATE;
//                #endif
//
//                #ifdef BRINGUP_DSPEAK_2
//                UART_putstr(UART_1, "dsPeak2 sent this message"); 
//                #ifdef UART_DEBUG_ENABLE
//                UART_putstr(UART_3, "RS-485 data rx'd = ");
//                UART_putbuf(UART_3, u485_rx_buf, UART_get_rx_buffer_length(UART_1));             
//                UART_putc(UART_3, 0x0D);
//                UART_putc(UART_3, 0x0A);                      
//                #endif         
//                DSPEAK_LED2_STATE = !DSPEAK_LED2_STATE;
//                #endif
//            }
//        }
        
        // QEI velocity refresh rate
//        if (TIMER_get_state(TIMER_7, TIMER_INT_STATE) == 1)
//        {           
////            QEI_calculate_velocity(QEI_2);  
////            new_pid_out2 = MOTOR_drive_pid(MOTOR_2);
////            MOTOR_drive_perc(MOTOR_2, MOTOR_get_direction(MOTOR_2), new_pid_out2);
//        }        
//        
        
        //if (TIMER_get_state(TIMER_7, TIMER_INT_STATE) == 1)
        //{
            //IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5621_OUTPUT_NORMAL, dac_out);
            //IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5621_OUTPUT_NORMAL, dac_out);
            //if (dac_out >= 0xFFF){dac_out = 0;}
            //IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5621_OUTPUT_NORMAL, sine_dac[dac_out]);
            //IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5621_OUTPUT_NORMAL, sine_dac[dac_out]);            
            
//#ifdef IVIDAC_RESOLUTION_12BIT
//            if (dac_out >= 512){dac_out = 0;}
//            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5621_OUTPUT_NORMAL, sine_dac_512[dac_out]);
//            IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5621_OUTPUT_NORMAL, sine_dac_512[dac_out]);  
//#endif
            
//#ifdef IVIDAC_RESOLUTION_14BIT
//            if (dac_out >= 256){dac_out = 0;}
//            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5641_OUTPUT_NORMAL, sine_dac_256p_14b[dac_out]);
//            IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, sine_dac_256p_14b[dac_out]); 
//            dac_out++;
//#endif            
//            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5641_OUTPUT_NORMAL, dac_out);
//            IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, dac_out);              
//            dac_out++;   
//            if (++dac_out > 0x3FFF){dac_out = 0;}
//            {
                //dac_out = 0;
//                if (dac_step == 0)
//                {
//                    dac_step = 1;
//                    IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5641_OUTPUT_NORMAL, 0x3FFF);
//                    IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, 0x3FFF);                      
//                }
//                
//                else if (dac_step == 1)
//                {
//                    dac_step = 2;
//                    IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5641_OUTPUT_NORMAL, 0x0000);
//                    IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, 0x0000);                      
//                }
//                
//                else if (dac_step == 2)
//                {
//                    dac_step = 3;
//                    IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5641_OUTPUT_NORMAL, 0x3FFF);
//                    IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, 0x3FFF);                      
//                }
//                
//                else if (dac_step == 3)
//                {
//                    dac_step = 0;
//                    IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS1, AD5641_OUTPUT_NORMAL, 0x0000);
//                    IVIDAC_set_output_raw(&IVIDAC_struct[1], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, 0x0000);                      
//                }                  
//            }
        //}
        
        // QEI velocity refresh rate
//        if (TIMER_get_state(TIMER_8, TIMER_INT_STATE) == 1)
//        {   
//            new_rpm_ain = (uint16_t)(((float)(ADC_get_raw_channel(&ADC_struct_AN2) / 1024.0))*QEI_get_max_rpm(QEI_1));
//            MOTOR_set_rpm(MOTOR_1, new_rpm_ain);
//            
//            QEI_calculate_velocity(QEI_1);  
//            new_pid_out1 = MOTOR_drive_pid(MOTOR_1);
//            MOTOR_drive_perc(MOTOR_1, MOTOR_get_direction(MOTOR_1), new_pid_out1);
//        }

        // SWPWM RGB LED timer
        //if (TIMER_get_state(TIMER_9, TIMER_INT_STATE) == 1)
        //{                   
//            unsigned int cnt = 0;
//            for (; cnt < 8; cnt++)
//            {
//                I2C_buf[0] = 0x90;
//                I2C_buf[1] = 0x08 + cnt; // Channel
//                I2C_buf[2] = ((sine_10b_512pts[i2c_sine_counter] & 0x03C0) >> 6) & 0x0F;
//                I2C_buf[3] = ((sine_10b_512pts[i2c_sine_counter] & 0x003F) << 2) & 0xFC;  
//                I2C_master_write(I2C_port_1, I2C_buf, 4);                
//            }         
//            i2c_sine_counter++;
//            if (i2c_sine_counter == 512){i2c_sine_counter = 0;}      
        //}        
    }
    return 0;
}
