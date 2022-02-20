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

void DSPIC_init (void);

#include "dspeak_generic.h"

#ifdef EVE_SCREEN_ENABLE
#include "ft8xx.h"
#endif

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

CAN_struct CAN_native;
STRUCT_IVIDAC IVIDAC_struct[2];

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

uint8_t i = 0;
uint8_t state = 0;
uint16_t counter_sec = 0;
uint16_t counter_5sec = 0;
uint16_t counter_timer1 = 0;
uint32_t rgb_r = 0, rgb_g = 0, rgb_b = 0;
uint16_t counter_ms = 0;
uint16_t counter_100us = 0;
uint16_t hour_bcd = 0, minute_bcd = 0, second_bcd = 0;
uint8_t buf[128] = {0};
uint8_t can_buf_1[8] = {'d', 's', 'P', 'e', 'a', 'k', '1', '!'};
uint8_t can_buf_2[8] = {'d', 's', 'P', 'e', 'a', 'k', '2', '!'};
uint16_t test = 1;
uint8_t speed = 20;
char new_pid_out1 = 0, new_pid_out2 = 0;
uint16_t speed_rpm_table[5] = {30, 95, 60, 18, 55};

RTCC_time clock;
uint8_t hour, minute, second;
int error_rpm;
uint16_t setpoint_rpm;
uint16_t actual_rpm;
uint8_t high, low;
uint8_t rs485_state = 0;
uint8_t pid_out = 0;
uint8_t color_counter = 0;
uint32_t color_888 = 0;

uint16_t RGB_PWM_frequency = 60;
uint8_t RGB_PWM_counter = 0;
uint8_t RGB_R_duty = 0;
uint8_t RGB_G_duty = 0;
uint8_t RGB_B_duty = 0;
uint8_t RGB_test = 0;
uint16_t mem_adr[10] = {0x0000, 0x0100, 0x0200, 0x0300, 0x0010, 0x0020, 0x0030, 0x0040, 0x0050, 0x0060};
uint16_t mem_dat[10] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA};

STRUCT_ADC ADC_struct_AN0, ADC_struct_AN1, ADC_struct_AN2, ADC_struct_AN12, 
            ADC_struct_AN13, ADC_struct_AN14, ADC_struct_AN15;

uint16_t data_rx_can[4] = {0};
uint8_t can_parse_return;
uint8_t I2C_buf[4] = {0};
uint16_t i2c_sine_counter = 0;

uint8_t record_flag = 0, playback_flag = 0;
uint8_t record_cnt = 1, playback_cnt = 1;
uint16_t sample_counter = 0;
uint16_t playback_counter = 0;
uint32_t mem_page_counter = 0;
uint16_t tx_ch_left = 0;
uint16_t tx_ch_right = 0;
uint8_t buff_write_0[256];
uint8_t buff_write_1[256];
uint8_t buff_read_0[256];
uint8_t buff_read_1[256];
uint8_t mem_write_flag = 0;
uint8_t mem_read_flag = 0;
uint8_t do_once = 0;
uint16_t test_chl = 0, test_chr = 0;
uint8_t first_read = 0;
uint8_t *mem_read_ptr;
uint8_t *mem_read_2_ptr;
extern uint8_t data_tx_ready;
uint8_t page_0_read_once = 0;
uint8_t buff_sel = 0;
uint8_t buff_write_sel = 0;
uint8_t buff_read = 0;
uint8_t first_block = 0;
uint16_t z = 0;
uint8_t first_write = 0;
uint8_t spi_flash_state_m = 0;
uint8_t flash_initialized = 0;
uint8_t status_reg;
uint8_t spi_op = 0;
uint8_t counter_timer4 = 0;

uint8_t retry_counter = 0;
uint8_t CAN_tx_return = 0;
uint8_t CAN_tx_state = 0;

uint8_t spi4_buf[260] = {0};

extern uint16_t rx_ch_left;
extern uint16_t rx_ch_right;

uint8_t *u485_rx_buf1;
uint8_t *u485_rx_buf2;
uint8_t u485_1_data_flag = 0;
uint8_t u485_2_data_flag = 0;
uint8_t counter_485 = 0;

uint16_t new_rpm_ain = 0;
uint32_t tracker = 0, color = 0;
uint16_t val = 0x7FFF;
uint8_t tag = 0;
uint8_t tag_val_write = 1;
uint16_t track_upd = 0;
uint8_t encoder_dir = 0;

uint16_t encoder_rpm = 0;
uint16_t encoder_tour = 0;

uint16_t dac_out = 0;
uint8_t dac_step = 0;
uint32_t counter_zero = 0;
uint8_t ividac_write_once = 0;
uint8_t btn1_debounce = 1;
uint8_t btn4_debounce = 1;

uint16_t inact_counter = 0;

int main() 
{
    DSPIC_init();
    RTCC_init();
    RTCC_write_time(clock);
        
    DSPEAK_LED1_DIR = 0;
    DSPEAK_LED1_STATE = 0;
    DSPEAK_LED2_DIR = 0;
    DSPEAK_LED2_STATE = 0;
    DSPEAK_LED3_DIR = 0;
    DSPEAK_LED4_STATE = 0;
    DSPEAK_LED4_DIR = 0;
    DSPEAK_LED4_STATE = 0;     

#ifdef UART_DEBUG_ENABLE
    UART_init(UART_DEBUG_struct, UART_3, 115200, UART_MAX_TX, UART_MAX_RX);
    UART_putstr_dma(UART_DEBUG_struct, "dsPeak UART debug port with DMA is enabled\r\n");
#endif

    //MOTOR_init(MOTOR_1, 30);
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
 
    //I2C_init(I2C_port_1, I2C_mode_master, 0);
    //I2C_buf[0] = 0x90;
    //I2C_buf[1] = 0x01;// Device config
    //I2C_buf[2] = 0x00;
    //I2C_buf[3] = 0x00; // Power-up all channels  
    //I2C_master_write(I2C_port_1, I2C_buf, 4);
//    TRISCbits.TRISC1 = 0;
//    TRISHbits.TRISH12 = 0;
//    LATCbits.LATC1 = 0;
//    LATHbits.LATH12 = 1;
    
    
    //PWM_init(PWM_5H, PWM_TYPE_SERVO);
    //PWM_init(PWM_6L, PWM_TYPE_SERVO);
    
    //SPI_init(SPI_2, SPI_MODE0, PPRE_1_1, SPRE_7_1);   // Set SPI2 to 10MHz (70MIPS / 7)
    //SPI_flash_init();
    //SPI_flash_write_enable();
    //SPI_flash_erase(CMD_CHIP_ERASE, 0);    
    
    CODEC_init(CODEC_sgtl5000, SPI_codec, SPI_3, SYS_FS_48kHz, CODEC_BLOCK_TRANSFER, CODEC_BLOCK_TRANSFER);
    
    // Enable dsPeak native RS-485 port @ 460800bps
    UART_init(UART_485_struct, UART_1, 460800, 32, 32);
    
#ifdef RS485_CLICK_UART2
    // Enable RS-485 Click6 RS-485 interface on MikroBus port @ 460800bps
    UART_init(UART_MKB_struct, UART_2, 460800, 32, 32);
#endif
    
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
    ADC_init_struct(&ADC_struct_AN12, ADC_PORT_1, ADC_CHANNEL_AN12, 
                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);
    ADC_init_struct(&ADC_struct_AN13, ADC_PORT_1, ADC_CHANNEL_AN13, 
                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100); 
    ADC_init_struct(&ADC_struct_AN14, ADC_PORT_1, ADC_CHANNEL_AN14, 
                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);
    ADC_init_struct(&ADC_struct_AN15, ADC_PORT_1, ADC_CHANNEL_AN15, 
                    ADC_RESOLUTION_12b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100); 
    
    ADC_init_struct(&ADC_struct_AN0, ADC_PORT_2, ADC_CHANNEL_AN0, 
                    ADC_RESOLUTION_10b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);
    ADC_init_struct(&ADC_struct_AN1, ADC_PORT_2, ADC_CHANNEL_AN1, 
                    ADC_RESOLUTION_10b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100); 
    ADC_init_struct(&ADC_struct_AN2, ADC_PORT_2, ADC_CHANNEL_AN2, 
                    ADC_RESOLUTION_10b, ADC_FORMAT_UNSIGNED_INTEGER, ADC_AUTO_CONVERT, ADC_SSRCG_SET_0, 100);    
 
    ADC_init(&ADC_struct_AN0);
    ADC_init(&ADC_struct_AN1);
    ADC_init(&ADC_struct_AN2); 
    
    ADC_init(&ADC_struct_AN12);
    ADC_init(&ADC_struct_AN13);
    ADC_init(&ADC_struct_AN14);
    ADC_init(&ADC_struct_AN15);
    
    ADC_start(&ADC_struct_AN0);    // Start ADC2
    ADC_start(&ADC_struct_AN12);   // Start ADC1
    
#ifdef EVE_SCREEN_ENABLE
    FT8XX_init();

    FT8XX_CMD_gradient(0, 0, 0, 0xFF0000, 480, 272, 0x5500FF);
    FT8XX_set_touch_tag(FT_PRIM_GRADIENT, 0, 1);
    FT8XX_CMD_text(0, 16, 7, FONT_18, 0, "dsPeak bring-up Rev1_00-0B");
    FT8XX_CMD_text(1, 19, 25, FONT_18, 0, "Designed by : Intellitrol");
    FT8XX_CMD_button(0, 330, 228, 140, 36, 27, 0, "Touch calibrate");
    FT8XX_set_touch_tag(FT_PRIM_BUTTON, 0, 2);
    FT8XX_CMD_text(2, 220, 240, 18, 0, "Brightness"); 
    
    FT8XX_CMD_text(3, 335, 7, 18, 0, "Analog inputs"); 
    FT8XX_CMD_text(4, 312, 27, 18, 0, "Identifier Raw value"); 
    FT8XX_CMD_text(5, 310, 50, 18, 0, "AN12 +-10V"); 
    FT8XX_CMD_text(6, 310, 75, 18, 0, "AN13 +-10V"); 
    FT8XX_CMD_text(7, 310, 100, 18, 0, "AN14 +-10V"); 
    FT8XX_CMD_text(8, 310, 125, 18, 0, "AN15 +-10V"); 
    FT8XX_CMD_text(9, 310, 150, 18, 0, "AN0  +-3V3"); 
    FT8XX_CMD_text(10, 310, 175, 18, 0, "AN1  +-3V3"); 
    FT8XX_CMD_text(11, 310, 200, 18, 0, "AN2    POT"); 
    
    FT8XX_CMD_text(12, 15, 155, 18, 0, "Rotary encoder direction"); 
    FT8XX_CMD_text(13, 220, 155, 18, 0, "Backward"); 
    FT8XX_CMD_text(14, 15, 172, 18, 0, "Rotary encoder tour"); 
    FT8XX_CMD_text(15, 15, 190, 18, 0, "Rotary encoder velocity"); 
    
    FT8XX_CMD_number(0, 415, 50, 18, 0, 32768);
    FT8XX_CMD_number(1, 415, 75, 18, 0, 32768);
    FT8XX_CMD_number(2, 415, 100, 18, 0, 32768);
    FT8XX_CMD_number(3, 415, 125, 18, 0, 32768);
    FT8XX_CMD_number(4, 415, 150, 18, 0, 32768);
    FT8XX_CMD_number(5, 415, 175, 18, 0, 32768);
    FT8XX_CMD_number(6, 415, 200, 18, 0, 32768);
    FT8XX_CMD_number(7, 220, 190, 18, 0, 0);
    FT8XX_CMD_number(8, 220, 172, 18, 0, 42);
       
    FT8XX_CMD_slider(0, 20, 245, 180, 10, 0, 65535, 65535);
    FT8XX_set_touch_tag(FT_PRIM_SLIDER, 0, 3);
    FT8XX_CMD_dial(0, 260, 75, 30, 0, 0);
    FT8XX_set_touch_tag(FT_PRIM_DIAL, 0, 4);
   
    FT8XX_CMD_button(1, 15, 210, 285, 22, 18, 0, "Rotary encoder switch");
    FT8XX_set_touch_tag(FT_PRIM_BUTTON, 1, 5);
    
    FT8XX_start_new_dl();					// Start a new display list, reset ring buffer and ring pointer
    FT8XX_write_dl_long(TAG_MASK(1));
    FT8XX_write_dl_long(CMD_COLDSTART);
    
    FT8XX_write_dl_long(TAG(st_Gradient[0].touch_tag));
    FT8XX_draw_gradient(&st_Gradient[0]);
    
    FT8XX_draw_text(&st_Text[0]);    
    FT8XX_draw_text(&st_Text[1]);  
    
    FT8XX_write_dl_long(TAG(st_Button[0].touch_tag));
    FT8XX_draw_button(&st_Button[0]);
    
    FT8XX_draw_text(&st_Text[2]); 
    
    FT8XX_draw_text(&st_Text[3]);
    FT8XX_draw_text(&st_Text[4]);
    FT8XX_draw_text(&st_Text[5]);
    FT8XX_draw_text(&st_Text[6]);
    FT8XX_draw_text(&st_Text[7]);
    FT8XX_draw_text(&st_Text[8]);
    FT8XX_draw_text(&st_Text[9]);
    FT8XX_draw_text(&st_Text[10]);
    FT8XX_draw_text(&st_Text[11]);
    FT8XX_draw_text(&st_Text[12]);
    FT8XX_draw_text(&st_Text[13]);
    FT8XX_draw_text(&st_Text[14]);
    FT8XX_draw_text(&st_Text[15]);
    
    FT8XX_draw_number(&st_Number[0]);
    FT8XX_draw_number(&st_Number[1]);
    FT8XX_draw_number(&st_Number[2]);
    FT8XX_draw_number(&st_Number[3]);
    FT8XX_draw_number(&st_Number[4]);
    FT8XX_draw_number(&st_Number[5]);
    FT8XX_draw_number(&st_Number[6]);
    FT8XX_draw_number(&st_Number[7]);
    FT8XX_draw_number(&st_Number[8]);
    
    FT8XX_write_dl_long(BEGIN(LINES));
    FT8XX_write_dl_long(VERTEX2II(305, 45, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 45, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 220, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 220, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 45, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 220, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 45, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 220, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(395, 25, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(395, 220, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 70, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 70, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 95, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 95, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 120, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 120, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 145, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 145, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 170, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 170, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(305, 195, 0, 0));
    FT8XX_write_dl_long(VERTEX2II(470, 195, 0, 0));       
    
    FT8XX_write_dl_long(COLOR_RGB(85, 170, 0));
    FT8XX_set_context_fcolor(0xFDFF59);
    FT8XX_write_dl_long(COLOR_RGB(0, 0, 0));
    FT8XX_write_dl_long(TAG(st_Slider[0].touch_tag));
    FT8XX_draw_slider(&st_Slider[0]);  

    FT8XX_draw_button(&st_Button[0]);
    
    FT8XX_write_dl_long(TAG(st_Dial[0].touch_tag));
    FT8XX_draw_dial(&st_Dial[0]);
    
    FT8XX_update_screen_dl();         		// Update display list 
#endif
//    RGB_LED_set_color(0x000000);
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
//    ST7735_init();
//    color = RGB888_to_RGB565(0x00FF00);
//    ST7735_Clear(color);
            
    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER1_struct, TIMER_1, TIMER_PRESCALER_256, 5);
    TIMER_init(TIMER2_struct, TIMER_2, TIMER_PRESCALER_256, 30);
    TIMER_init(TIMER3_struct, TIMER_3, TIMER_PRESCALER_256, 5);
    //TIMER_init(TIMER4_struct, TIMER_4, TIMER_PRESCALER_256, 5);
    //TIMER_init(TIMER5_struct, TIMER_5, TIMER_PRESCALER_256, 5);
    //TIMER_init(TIMER6_struct, TIMER_6, TIMER_PRESCALER_256, 5);
    //TIMER_init(TIMER7_struct, TIMER_7, TIMER_PRESCALER_256, 10);
    //TIMER_init(TIMER8_struct, TIMER_8, TIMER_PRESCALER_256, 5);
    
    // Encoder initialization with associated velocity timer   
    ENCODER_init(10); 
    TIMER_init(TIMER9_struct, TIMER_9, TIMER_PRESCALER_256, ENCODER_get_fs());
                                  // 
    TIMER_start(TIMER1_struct);
    TIMER_start(TIMER2_struct);
    TIMER_start(TIMER3_struct);
    //TIMER_start(TIMER4_struct);
    //TIMER_start(TIMER5_struct);
    //TIMER_start(TIMER6_struct);
    //TIMER_start(TIMER7_struct);
    //TIMER_start(TIMER8_struct);
    TIMER_start(TIMER9_struct);
    
#ifdef UART_DEBUG_ENABLE
    UART_putstr_dma(UART_DEBUG_struct, "Program while(1) starts here\r\n");
#endif
  
    while (1)
    {            
        encoder_dir = ENCODER_get_direction();
        encoder_tour = ENCODER_get_position();             
        
        if (DSPEAK_BTN1_STATE == 0)
        {
            if (++btn1_debounce > 25)
            {
                btn1_debounce = 25;
                if (do_once == 0)
                {
                    do_once = 1;
                    UART_putstr_dma(UART_485_struct, "Message sent from native port!\r\n");
                }
            }
        }
        else
        {
            if (--btn1_debounce < 1)
            {
                btn1_debounce = 1;
                do_once = 0;
            }
        }

        if (UART_rx_done(UART_485_struct) == UART_RX_COMPLETE)
        {            
            u485_1_data_flag = 1;
            DSPEAK_LED1_STATE = 1;
        } 

        if (UART_rx_done(UART_MKB_struct) == UART_RX_COMPLETE)
        {           
            u485_2_data_flag = 1; 
            DSPEAK_LED2_STATE = 1;
        }
        
        // Handle DCI transfer DMA interrupt
#ifdef DCI0_DMA_ENABLE
        if (DCI_get_interrupt_state(CODEC_sgtl5000, DCI_DMA_TX) == DCI_TRANSMIT_COMPLETE)
        {

        }

        if (DCI_get_interrupt_state(CODEC_sgtl5000, DCI_DMA_RX) == DCI_RECEIVE_COMPLETE)
        {
            
        }
#endif
        
        if (TIMER_get_state(TIMER1_struct, TIMER_INT_STATE) == 1)
        {
            if (u485_1_data_flag == 1)
            {                
                u485_1_data_flag = 0;
                UART_putstr_dma(UART_485_struct, "Message sent from native port!\r\n");
                DSPEAK_LED1_STATE = 0;
            }
        }       

        if (TIMER_get_state(TIMER3_struct, TIMER_INT_STATE) == 1)
        {
#ifdef RS485_CLICK_UART2               
            if (u485_2_data_flag == 1)
            {                
                u485_2_data_flag = 0;
                UART_putstr_dma(UART_MKB_struct, "Message sent from MikroB port!\r\n");
                DSPEAK_LED2_STATE = 0;
            }              
#endif               
        }   
        
        if (TIMER_get_state(TIMER2_struct, TIMER_INT_STATE) == 1)
        {           
#ifdef EVE_SCREEN_ENABLE 
            if (encoder_dir == 0)
            {
                FT8XX_modify_element_string(13, FT_PRIM_TEXT, "Forward ");
            }
            else
            {
                FT8XX_modify_element_string(13, FT_PRIM_TEXT, "Backward");
            }            
            
            FT8XX_modify_number(&st_Number[0], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN12));
            FT8XX_modify_number(&st_Number[1], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN13));
            FT8XX_modify_number(&st_Number[2], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN14));
            FT8XX_modify_number(&st_Number[3], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN15));
            FT8XX_modify_number(&st_Number[4], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN0));
            FT8XX_modify_number(&st_Number[5], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN1));
            FT8XX_modify_number(&st_Number[6], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN2));
            FT8XX_modify_number(&st_Number[7], NUMBER_VAL, encoder_rpm);
            FT8XX_modify_number(&st_Number[8], NUMBER_VAL, encoder_tour);
            //tracker = FT8XX_rd32(REG_TRACKER);
                       
            FT8XX_start_new_dl();					// Start a new display list, reset ring buffer and ring pointer
            FT8XX_write_dl_long(TAG_MASK(1));
            FT8XX_write_dl_long(CMD_COLDSTART);
            
            FT8XX_write_dl_long(TAG(st_Gradient[0].touch_tag));
            FT8XX_draw_gradient(&st_Gradient[0]);

            FT8XX_draw_text(&st_Text[0]);

            FT8XX_draw_text(&st_Text[1]);  

            FT8XX_draw_text(&st_Text[3]);
            FT8XX_draw_text(&st_Text[4]);
            FT8XX_draw_text(&st_Text[5]);
            FT8XX_draw_text(&st_Text[6]);
            FT8XX_draw_text(&st_Text[7]);
            FT8XX_draw_text(&st_Text[8]);
            FT8XX_draw_text(&st_Text[9]);
            FT8XX_draw_text(&st_Text[10]);
            FT8XX_draw_text(&st_Text[11]);
            FT8XX_draw_text(&st_Text[12]);

            FT8XX_draw_text(&st_Text[13]);
            FT8XX_draw_text(&st_Text[14]);   
            FT8XX_draw_text(&st_Text[15]); 

            FT8XX_draw_number(&st_Number[0]);
            FT8XX_draw_number(&st_Number[1]);
            FT8XX_draw_number(&st_Number[2]);
            FT8XX_draw_number(&st_Number[3]);
            FT8XX_draw_number(&st_Number[4]);
            FT8XX_draw_number(&st_Number[5]);
            FT8XX_draw_number(&st_Number[6]);
            FT8XX_draw_number(&st_Number[7]);
            FT8XX_draw_number(&st_Number[8]);

            FT8XX_write_dl_long(BEGIN(LINES));
            FT8XX_write_dl_long(VERTEX2II(305, 45, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 45, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 220, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 220, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 45, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 220, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 45, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 220, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(395, 25, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(395, 220, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 70, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 70, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 95, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 95, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 120, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 120, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 145, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 145, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 170, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 170, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(305, 195, 0, 0));
            FT8XX_write_dl_long(VERTEX2II(470, 195, 0, 0));               
            
            FT8XX_write_dl_long(TAG(st_Button[0].touch_tag));
            FT8XX_draw_button(&st_Button[0]);

            FT8XX_draw_text(&st_Text[2]);   

            FT8XX_write_dl_long(COLOR_RGB(85, 170, 0));
            FT8XX_set_context_fcolor(0xFDFF59);
            FT8XX_write_dl_long(COLOR_RGB(0, 0, 0));

            FT8XX_CMD_tracker(st_Slider[0].x, st_Slider[0].y, st_Slider[0].w, st_Slider[0].h, st_Slider[0].touch_tag);
            FT8XX_write_dl_long(TAG(st_Slider[0].touch_tag));
            FT8XX_draw_slider(&st_Slider[0]);  

            FT8XX_CMD_tracker(st_Dial[0].x, st_Dial[0].y, 1, 1, st_Dial[0].touch_tag);
            FT8XX_write_dl_long(TAG(st_Dial[0].touch_tag));
            FT8XX_draw_dial(&st_Dial[0]);
            FT8XX_write_dl_long(COLOR_RGB(255, 255, 255));
            
            if (ENCODER_get_switch_state() == 0)     
            {
                
                FT8XX_set_context_fcolor(0x00FF00);
                FT8XX_draw_button(&st_Button[1]);
            }
            else
            {
                FT8XX_set_context_fcolor(0xFF0000);
                FT8XX_draw_button(&st_Button[1]);
            }
            
            FT8XX_update_screen_dl();         		// Update display list  
                        
            tag = FT8XX_read_touch_tag();         
            tracker = FT8XX_rd32(REG_TRACKER); 
            
            if (tag != 0)
            {
                
                if ((tracker & 0x0000FFFF) == st_Slider[0].touch_tag)
                {
                    track_upd = ((tracker&0xFFFF0000)>>16);
                    FT8XX_modify_slider(&st_Slider[0], SLIDER_VAL, track_upd);
                    track_upd = (track_upd >>9); // Divide by 512 to get a range of 0-128
                    if (track_upd < 10){track_upd = 10;}
                    FT8XX_wr8(REG_PWM_DUTY, track_upd);
                }
                
                else if ((tracker & 0x0000FFFF) == st_Dial[0].touch_tag)
                {
                    track_upd = ((tracker&0xFFFF0000)>>16);
                    FT8XX_modify_dial(&st_Dial[0], DIAL_VALUE, track_upd);
                }
            }
            
            if (tag == st_Button[0].touch_tag)
            {
                while(((FT8XX_rd32(REG_TOUCH_DIRECT_XY)) & 0x8000) == 0x0000);
                FT8XX_touchpanel_calibrate();
                FT8XX_clear_touch_tag();
                while(((FT8XX_rd32(REG_TOUCH_DIRECT_XY)) & 0x8000) == 0x0000);
            }        
#endif
        } 
        
        if (TIMER_get_state(TIMER4_struct, TIMER_INT_STATE) == 1)
        {
        }  
        
        if (TIMER_get_state(TIMER5_struct, TIMER_INT_STATE) == 1)
        {
            if (++color_counter > TIMER_get_freq(TIMER5_struct))
            {
                color_counter = 0;
                //UART_putstr_dma(&UART_struct[UART_3], "Timer 5 tick\r\n");              
            }
        } 
        
        if (TIMER_get_state(TIMER6_struct, TIMER_INT_STATE) == 1)
        {
            
        } 
        
        if (TIMER_get_state(TIMER7_struct, TIMER_INT_STATE) == 1)
        {
            
        }
        
        if (TIMER_get_state(TIMER8_struct, TIMER_INT_STATE) == 1)
        {
            
        } 
               
        if (TIMER_get_state(TIMER9_struct, TIMER_INT_STATE) == 1)
        {
            encoder_rpm = ENCODER_get_velocity();
        }
 
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

void DSPIC_init (void)
{ 
    INTCON1bits.NSTDIS = 0;                         // Nested interrupt enabled 
    RCONbits.SWDTEN=0;                              // Watchdog timer disabled 
    
    dsPeak_posc_20MHz_init();
    
    // At power-up, set default system time
    clock.second = 0;
    clock.minute = 20;
    clock.hour = 16;
    clock.weekday = 4;
    clock.date = 4;
    clock.month = 4;
    clock.year = 2020;
    
    DSPEAK_LED1_DIR = 0;
    DSPEAK_LED1_DIR = 0;
    DSPEAK_LED1_DIR = 0;
    DSPEAK_LED1_DIR = 0;
    
    DSPEAK_BTN1_DIR = 1;
    DSPEAK_BTN4_DIR = 1;
    
    // At power-up, disable all analog pins
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELG = 0;

    // At power-up, initialize DMA struct to default POR
    DMA_struct_init(DMA_ALL_INIT);
}

void RGB_LED_set_color (uint32_t color)
{
    uint8_t R = ((color & 0xFF0000)>>16);
    uint8_t G = ((color & 0x00FF00)>>8);
    uint8_t B = color & 0x0000FF;
    
    RGB_R_duty = (R * (RGB_PWM_frequency/255.0));
    RGB_G_duty = (G * (RGB_PWM_frequency/255.0));
    RGB_B_duty = (B * (RGB_PWM_frequency/255.0));
}
