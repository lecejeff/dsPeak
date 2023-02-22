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
#include "ft8xx.h"

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
STRUCT_PWM *PWM2L_struct = &PWM_struct[PWM_2L];
STRUCT_PWM *PWM2H_struct = &PWM_struct[PWM_2H];
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
STRUCT_SPI *EVE_spi = &SPI_struct[SPI_1];       // Assign EVE_spi to SPI_1
STRUCT_SPI *SPI_codec = &SPI_struct[SPI_3];     // Assign CODEC to SPI_3

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

// Access to RTC struct member
extern STRUCT_RTCC RTCC_struct[RTC_QTY];
STRUCT_RTCC *RTC1_struct = &RTCC_struct[RTC_1];

// Access to Bridgetek EVE struct member
extern STRUCT_BT8XX BT8XX_struct[BT8XX_QTY];
STRUCT_BT8XX *eve = &BT8XX_struct[BT8XX_1];

// Debug variables related to functions under development
char new_pid_out1 = 0, new_pid_out2 = 0;
uint8_t state = 0;
uint16_t counter_5sec  = 0;
uint16_t speed_rpm_table[5] = {30, 95, 60, 18, 55};
uint8_t motor_debug_buf[18] = {0};
int error_rpm;
uint16_t setpoint_rpm;
uint16_t actual_rpm;

uint16_t i = 0;

uint8_t *u485_rx_buf1;
uint8_t *u485_rx_buf2;
uint8_t u485_1_data_flag = 0;
uint8_t u485_2_data_flag = 0;
uint8_t counter_485_native = 0;
uint8_t u485_2_rx = 0;

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

uint16_t color;

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

uint8_t tag = 0;
uint16_t track_upd = 0;
uint32_t tracker = 0;
uint8_t track_counter = 0;
uint8_t key = 0;

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
    
    RTCC_init(RTC1_struct, RTC_1);   
    RTCC_write_time(RTC1_struct);

#ifdef UART_DEBUG_ENABLE
    UART_init(UART_DEBUG_struct, UART_3, 115200, UART_MAX_TX, UART_MAX_RX, DMA_CH14);
    while(UART_putstr_dma(UART_DEBUG_struct, "dsPeak UART debug port with DMA is enabled\r\n") != 1);   
#endif
    
#ifdef EVE_SCREEN_ENABLE
    FT8XX_init (eve, EVE_spi, SPI_1, DMA_CH4, DMA_CH5);
    
    // Initialize DisplayList
    FT8XX_CMD_gradient(0, 0, 0, 0xFF0000, 480, 272, 0x5500FF);
    FT8XX_set_touch_tag(FT_PRIM_GRADIENT, 0, 1);
    
    FT8XX_CMD_text(0, 10, 7, 18, 0, "dsPeak bring-up Rev1_00-0B");
    FT8XX_CMD_text(1, 10, 25, 18, 0, "Designed by : Intellitrol");
    FT8XX_CMD_text(2, 390, 242, 18, 0, "Brightness"); 
    FT8XX_CMD_text(3, 314, 8, 18, 0, "Keypad input"); 
       
    FT8XX_CMD_slider(0, 20, 245, 350, 10, 0, 65535, 65535);
    FT8XX_set_touch_tag(FT_PRIM_SLIDER, 0, 2);
    
    FT8XX_CMD_keys(0, 13, 65, 234, 31, 27, OPT_FLAT | OPT_CENTERX, "1234567890");
    FT8XX_set_touch_tag(FT_PRIM_KEYS, 0, 3);
    
    FT8XX_CMD_keys(1, 13, 100, 234, 31, 27, OPT_FLAT | OPT_CENTERX, "QWERTYUIOP");
    FT8XX_set_touch_tag(FT_PRIM_KEYS, 1, 4);
    
    FT8XX_CMD_keys(2, 13, 135, 234, 31, 27, OPT_FLAT | OPT_CENTERX, "ASDFGHJKL");
    FT8XX_set_touch_tag(FT_PRIM_KEYS, 2, 5);
    
    FT8XX_CMD_keys(3, 13, 170, 234, 31, 27, OPT_FLAT | OPT_CENTERX, "ZXCVBNM");
    FT8XX_set_touch_tag(FT_PRIM_KEYS, 3, 6);
    // End of DisplayList initialization
    
    // DisplayList write to BT8XX
    FT8XX_start_new_dl(eve);					// Start a new display list, reset ring buffer and ring pointer
    FT8XX_write_dl_long(eve, TAG_MASK(1));
    FT8XX_write_dl_long(eve, CMD_COLDSTART);
    
    FT8XX_write_dl_long(eve, TAG(st_Gradient[0].touch_tag));
    FT8XX_draw_gradient(eve, &st_Gradient[0]);
    
    FT8XX_draw_text(eve, &st_Text[0]);    
    FT8XX_draw_text(eve, &st_Text[1]);    
    FT8XX_draw_text(eve, &st_Text[2]);    
    FT8XX_draw_text(eve, &st_Text[3]);     
    
    FT8XX_write_dl_long(eve, COLOR_RGB(85, 170, 0));
    FT8XX_set_context_fcolor(eve, 0xFDFF59);
    FT8XX_write_dl_long(eve, COLOR_RGB(0, 0, 0));
    FT8XX_write_dl_long(eve, TAG(st_Slider[0].touch_tag));
    FT8XX_draw_slider(eve, &st_Slider[0]); 
    
    FT8XX_write_dl_long(eve, TAG(st_Keys[0].touch_tag));
    FT8XX_draw_keys(eve, &st_Keys[0]);
    FT8XX_write_dl_long(eve, TAG(st_Keys[1].touch_tag));
    FT8XX_draw_keys(eve, &st_Keys[1]);
    FT8XX_write_dl_long(eve, TAG(st_Keys[2].touch_tag));
    FT8XX_draw_keys(eve, &st_Keys[2]);
    FT8XX_write_dl_long(eve, TAG(st_Keys[3].touch_tag));
    FT8XX_draw_keys(eve, &st_Keys[3]);
    
    FT8XX_write_dl_long(eve, BEGIN(RECTS));
    FT8XX_write_dl_long(eve, COLOR_RGB(0, 0, 0));
    FT8XX_write_dl_long(eve, VERTEX2II(250, 30, 0, 0));
    FT8XX_write_dl_long(eve, VERTEX2II(460, 230, 0, 0));
    FT8XX_write_dl_long(eve, END());
    
    FT8XX_update_screen_dl(eve);         		// Update display list 
#endif

    MOTOR_init(PWM1H_struct, PWM1L_struct, MOTOR_1, 30);
    MOTOR_init(PWM2H_struct, PWM2L_struct, MOTOR_2, 30);    
    
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
    //SPI_flash_init(FLASH_struct, SPI_flash, ((CODEC_BLOCK_TRANSFER * 2) + 4), ((CODEC_BLOCK_TRANSFER * 2) + 4), DMA_CH2, DMA_CH1);     
    //CODEC_init(CODEC_sgtl5000, SPI_codec, SPI_3, SYS_FS_16kHz, CODEC_BLOCK_TRANSFER, CODEC_BLOCK_TRANSFER, DMA_CH3, DMA_CH0);
    
    // Enable dsPeak native RS-485 port @ 460800bps
    UART_init(UART_485_struct, UART_1, 460800, 32, 32, DMA_CH12);
    
#ifdef RS485_CLICK_UART2
    // Enable RS-485 Click6 RS-485 interface on MikroBus port @ 460800bps
    UART_init(UART_MKB_struct, UART_2, 460800, 32, 32, DMA_CH13);
#endif
    
    //Physical rotary encoder initialization
    //ENCODER_init(ENC1_struct, ENC_1, 30); 
    
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
        
    //MOTOR_set_rpm(MOTOR_1, speed_rpm_table[0]);
    //MOTOR_set_rpm(MOTOR_2, speed_rpm_table[0]);
    
    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER1_struct, TIMER_1, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER2_struct, TIMER_2, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER3_struct, TIMER_3, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);   
    TIMER_init(TIMER4_struct, TIMER_4, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);
    TIMER_init(TIMER5_struct, TIMER_5, TIMER_MODE_16B, TIMER_PRESCALER_256, 5);   
    TIMER_init(TIMER6_struct, TIMER_6, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);   
    TIMER_init(TIMER7_struct, TIMER_7, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);                    // Motor driver refresh timer     
    //TIMER_init(TIMER8_struct, TIMER_8, TIMER_MODE_16B, TIMER_PRESCALER_256, 60);    // Eve refresh timer 
    //TIMER_init(TIMER9_struct, TIMER_9, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);    

    TIMER_start(TIMER1_struct);
    TIMER_start(TIMER2_struct);
    TIMER_start(TIMER3_struct);   
    TIMER_start(TIMER4_struct);
    TIMER_start(TIMER5_struct);
    TIMER_start(TIMER6_struct);
    TIMER_start(TIMER7_struct);
    //TIMER_start(TIMER8_struct);
    //TIMER_start(TIMER9_struct);
    
    while (1)
    {   
        // Handle UART_1 RX interrupt flag
        if (UART_rx_done(UART_485_struct) == UART_RX_COMPLETE)
        {            
            u485_1_data_flag = 1;
            dsPeak_led_write(LED1_struct, LOW);
        } 

        // Handle UART_2 RX interrupt flag
        if (UART_rx_done(UART_MKB_struct) == UART_RX_COMPLETE)
        {           
            u485_2_data_flag = 1; 
            dsPeak_led_write(LED2_struct, LOW); 
        }
        
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
        
#ifdef RS485_CLICK_UART2             
        if (U2STAbits.TRMT == 1)
        {
            // Put the transceiver in receive mode
            LATDbits.LATD0 = 0;     // DE = 0
            LATHbits.LATH15 = 0;    // RE = 0              
        }
#endif    
        
        // Handle UART_1 TX
        if (TIMER_get_state(TIMER1_struct, TIMER_INT_STATE) == 1)
        {
            if (u485_1_data_flag == 1)
            {                              
                if (UART_putstr_dma(UART_485_struct, "Message sent from native port!\r\n") == 1)
                {
                    u485_1_data_flag = 0;
                    counter_485_native = 0;
                    dsPeak_led_write(LED1_struct, HIGH);
                }             
            }
            else
            {
                if (++counter_485_native > 25)
                {
                    counter_485_native = 0;
                    u485_1_data_flag = 1;   // Auto-retry transmission after 5s
                }
            }
        }       

        // Handle UART_2 TX
        if (TIMER_get_state(TIMER2_struct, TIMER_INT_STATE) == 1)
        {
#ifdef RS485_CLICK_UART2               
            if (u485_2_data_flag == 1)
            {        
                if (UART_putstr_dma(UART_MKB_struct, "Message sent from MikroB port!\r\n") == 1)
                {
                    u485_2_data_flag = 0;
                    dsPeak_led_write(LED2_struct, HIGH);
                }
            }
#endif      
        }         
        
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
        
        // Motor debug port
        if (TIMER_get_state(TIMER4_struct, TIMER_INT_STATE) == 1)
        {   
            setpoint_rpm = MOTOR_get_setpoint_rpm(MOTOR_1);
            actual_rpm = QEI_get_speed_rpm(QEI_1);
            error_rpm = setpoint_rpm - actual_rpm;
            motor_debug_buf[0] = 0xAE;
            motor_debug_buf[1] = 0xAE;
            motor_debug_buf[2] = ((setpoint_rpm & 0xFF00)>>8);
            motor_debug_buf[3] = setpoint_rpm&0x00FF;    
            motor_debug_buf[4] = ((error_rpm & 0xFF00)>>8);
            motor_debug_buf[5] = error_rpm&0x00FF;    
            motor_debug_buf[6] = ((actual_rpm & 0xFF00)>>8);
            motor_debug_buf[7] = actual_rpm&0x00FF; 
            motor_debug_buf[8] = 0;
            motor_debug_buf[9] = new_pid_out1;
            setpoint_rpm = MOTOR_get_setpoint_rpm(MOTOR_2);
            actual_rpm = QEI_get_speed_rpm(QEI_2);
            error_rpm = setpoint_rpm - actual_rpm;            
            motor_debug_buf[10] = ((setpoint_rpm & 0xFF00)>>8);
            motor_debug_buf[11] = setpoint_rpm&0x00FF;    
            motor_debug_buf[12] = ((error_rpm & 0xFF00)>>8);
            motor_debug_buf[13] = error_rpm&0x00FF;    
            motor_debug_buf[14] = ((actual_rpm & 0xFF00)>>8);
            motor_debug_buf[15] = actual_rpm&0x00FF; 
            motor_debug_buf[16] = 0;
            motor_debug_buf[17] = new_pid_out2;            
            UART_putbuf_dma(UART_DEBUG_struct, motor_debug_buf, 18);
        }  
//        
        if (TIMER_get_state(TIMER5_struct, TIMER_INT_STATE) == 1)
        {
            if (++counter_5sec >= 10)
            {                  
                counter_5sec = 0;
                state++;
                if (state > 4){state = 0;}
                //MOTOR_set_rpm(MOTOR_1, speed_rpm_table[state]);
                //MOTOR_set_rpm(MOTOR_2, speed_rpm_table[state]);
            }
        } 
        
        // QEI velocity refresh rate
        if (TIMER_get_state(TIMER6_struct, TIMER_INT_STATE) == 1)
        {           
            QEI_calculate_velocity(QEI_2);  
            new_pid_out2 = MOTOR_drive_pid(MOTOR_2);
            MOTOR_drive_perc(MOTOR_2, MOTOR_get_direction(MOTOR_2), new_pid_out2);
        }        
        
        // QEI velocity refresh rate
        if (TIMER_get_state(TIMER7_struct, TIMER_INT_STATE) == 1)
        {           
            QEI_calculate_velocity(QEI_1);  
            new_pid_out1 = MOTOR_drive_pid(MOTOR_1);
            MOTOR_drive_perc(MOTOR_1, MOTOR_get_direction(MOTOR_1), new_pid_out1);
        }

#ifdef EVE_SCREEN_ENABLE        
        if (TIMER_get_state(TIMER8_struct, TIMER_INT_STATE) == 1)
        {           
            // DisplayList write to BT8XX

            FT8XX_start_new_dl(eve);					// Start a new display list, reset ring buffer and ring pointer
            FT8XX_write_dl_long(eve, TAG_MASK(1));
            FT8XX_write_dl_long(eve, CMD_COLDSTART);

            FT8XX_write_dl_long(eve, TAG(st_Gradient[0].touch_tag));
            FT8XX_draw_gradient(eve, &st_Gradient[0]);

            FT8XX_draw_text(eve, &st_Text[0]);    
            FT8XX_draw_text(eve, &st_Text[1]);    
            FT8XX_draw_text(eve, &st_Text[2]);    
            FT8XX_draw_text(eve, &st_Text[3]);     

            FT8XX_write_dl_long(eve, COLOR_RGB(85, 170, 0));
            FT8XX_set_context_fcolor(eve, 0xFDFF59);
            FT8XX_write_dl_long(eve, COLOR_RGB(0, 0, 0));
            
            FT8XX_CMD_tracker(eve, st_Slider[0].x, st_Slider[0].y, st_Slider[0].w, st_Slider[0].h, st_Slider[0].touch_tag);
            FT8XX_write_dl_long(eve, TAG(st_Slider[0].touch_tag));
            FT8XX_draw_slider(eve, &st_Slider[0]); 

            FT8XX_write_dl_long(eve, TAG(st_Keys[0].touch_tag));
            FT8XX_draw_keys(eve, &st_Keys[0]);
            FT8XX_write_dl_long(eve, TAG(st_Keys[1].touch_tag));
            FT8XX_draw_keys(eve, &st_Keys[1]);
            FT8XX_write_dl_long(eve, TAG(st_Keys[2].touch_tag));
            FT8XX_draw_keys(eve, &st_Keys[2]);
            FT8XX_write_dl_long(eve, TAG(st_Keys[3].touch_tag));
            FT8XX_draw_keys(eve, &st_Keys[3]);

            FT8XX_write_dl_long(eve, BEGIN(RECTS));
            FT8XX_write_dl_long(eve, COLOR_RGB(0, 0, 0));
            FT8XX_write_dl_long(eve, VERTEX2II(250, 30, 0, 0));
            FT8XX_write_dl_long(eve, VERTEX2II(460, 230, 0, 0));
            FT8XX_write_dl_long(eve, END());

            FT8XX_update_screen_dl(eve);         		// Update display list 
                       
            tag = FT8XX_read_touch_tag(eve);         
            tracker = FT8XX_rd32(eve, REG_TRACKER); 
            
            if (tag != 0)
            {               
                if ((tracker & 0x0000FFFF) == st_Slider[0].touch_tag)
                {
                    track_upd = ((tracker&0xFFFF0000)>>16);
                    FT8XX_modify_slider(&st_Slider[0], SLIDER_VAL, track_upd);
                    track_upd = (track_upd >> 9); // Divide by 512 to get a range of 0-128
                    if (track_upd < 10){track_upd = 10;}
                    FT8XX_wr8(eve, REG_PWM_DUTY, track_upd);
                }
                
                if (((tag >= 0x41) && (tag <= 0x5A)) || ((tag >= 0x30) && (tag <= 0x39)) )   // On-screen keyboard key pressed 
                {
                    key = tag;                                      // save tag value as key press
                    FT8XX_modify_keys(&st_Keys[0], KEYS_OPT, OPT_FLAT | OPT_CENTERX | key);  // Setting option to tag value will show key depressed
                    FT8XX_modify_keys(&st_Keys[1], KEYS_OPT, OPT_FLAT | OPT_CENTERX | key);  // Setting option to tag value will show key depressed
                    FT8XX_modify_keys(&st_Keys[2], KEYS_OPT, OPT_FLAT | OPT_CENTERX | key);  // Setting option to tag value will show key depressed
                    FT8XX_modify_keys(&st_Keys[3], KEYS_OPT, OPT_FLAT | OPT_CENTERX | key);  // Setting option to tag value will show key depressed
                }
            }
        }
#endif

        if (TIMER_get_state(TIMER9_struct, TIMER_INT_STATE) == 1)
        {   
        } 
    }
    return 0;
}
