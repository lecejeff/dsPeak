//****************************************************************************//
// File      :  main.c
//
// Includes  :  general.h
//              Project-related .h files
//
// Purpose   : dsPeak development project
//
// Intellitrol                   MPLab X v5.45                        13/01/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
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
#pragma config ALTI2C2 = ON             // Alternate I2C pins for I2C2 (I2C2 mapped to ASDA2/ASCL2 pins)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable Bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Aux Flash may be written)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)
// End of dsPIC33EP512MU814 configuration fuses ------------------------------//

#include "general.h"
#include "adc.h"
#include "rtcc.h"
#include "timer.h"
#include "ft8xx.h"
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

#define SCREEN_ENABLE

CAN_struct CAN_native;

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
uint8_t test_spi[8] = {'J', 'e', 't', 'a', 'i', 'm', 'e', '!'};
const char *str = "-- Testing a full 64 bytes transfer from DPSRAM->DMA->USART3!\r\n";
uint8_t can_buf[8] = {'d', 's', 'P', 'e', 'a', 'k', ':', ')'};
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
uint16_t color = 0;
uint8_t color_counter = 0;
uint32_t color_888 = 0;

uint16_t RGB_PWM_frequency = 60;
uint8_t RGB_PWM_counter = 0;
uint8_t RGB_R_duty = 0;
uint8_t RGB_G_duty = 0;
uint8_t RGB_B_duty = 0;
uint8_t RGB_test = 0;
uint16_t mem_adr[10] = {0x1010, 0x2020, 0x3030, 0x4040, 0x5050, 0x6060, 0x7070, 0x8080, 0x9090, 0xA0A0};
uint16_t mem_dat[10] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA};

STRUCT_ADC ADC_struct_AN0, ADC_struct_AN1, ADC_struct_AN2, ADC_struct_AN12, 
            ADC_struct_AN13, ADC_struct_AN14, ADC_struct_AN15;

int main() 
{
    DSPIC_init();
    RTCC_init();
    RTCC_write_time(clock);
    UART_init(UART_1, 115200, 16);
    UART_init(UART_2, 115200, 16);
    UART_init(UART_3, 230400, 1);
    
    MOTOR_init(MOTOR_1, 30);
    MOTOR_init(MOTOR_2, 30);            

    PWM_init(PWM_5H, PWM_TYPE_SERVO);
    PWM_init(PWM_6L, PWM_TYPE_SERVO);
   
    CODEC_init(SYS_FS_48kHz);
    
    SPI_flash_init();
    SPI_flash_write_enable();
    SPI_flash_erase(CMD_BLOCK_ERASE_4k, 0);
    SPI_flash_write(0x100, test_spi, 8);
 
    
    CAN_init_struct(&CAN_native, CAN_1, 500000, 0x0123, 0, 0x0300, 0x0300);
    CAN_init(&CAN_native);
    CAN_set_mode(&CAN_native, CAN_MODE_NORMAL);
    CAN_fill_payload(&CAN_native, can_buf, 8);
    
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
    
#ifdef SCREEN_ENABLE
    FT8XX_init();
    FT8XX_CMD_text(0, 60, 20, 22, OPT_CENTER,  "# of tour ");
    FT8XX_CMD_number(0, 130, 20, 22, OPT_CENTER, QEI_get_tour(QEI_1));
    FT8XX_CMD_text(1, 60, 40, 22, OPT_CENTER,  "RPM setp ");
    FT8XX_CMD_number(1, 130, 40, 22, OPT_CENTER, MOTOR_get_setpoint_rpm(MOTOR_1));
    FT8XX_CMD_text(2, 60, 60, 22, OPT_CENTER,  "Raw velcnt ");
    FT8XX_CMD_number(2, 130, 60, 22, OPT_CENTER, QEI_get_velocity(QEI_1));
    FT8XX_CMD_text(3, 60, 80, 22, OPT_CENTER,  "Nb pulses ");
    FT8XX_CMD_number(3, 130, 80, 22, OPT_CENTER, QEI_get_pulse(QEI_1));
    FT8XX_CMD_text(4, 60, 100, 22, OPT_CENTER, "Speed rpm ");
    FT8XX_CMD_number(4, 130, 100, 22, OPT_CENTER, QEI_get_speed_rpm(QEI_1));
    
    FT8XX_CMD_text(5, 360, 20, 22, OPT_CENTER,  "# of tour ");
    FT8XX_CMD_number(5, 430, 20, 22, OPT_CENTER, QEI_get_tour(QEI_2));
    FT8XX_CMD_text(6, 360, 40, 22, OPT_CENTER,  "RPM setp ");
    FT8XX_CMD_number(6, 430, 40, 22, OPT_CENTER, MOTOR_get_setpoint_rpm(MOTOR_2));
    FT8XX_CMD_text(7, 360, 60, 22, OPT_CENTER,  "Raw velcnt ");
    FT8XX_CMD_number(7, 430, 60, 22, OPT_CENTER, QEI_get_velocity(QEI_2));
    FT8XX_CMD_text(8, 360, 80, 22, OPT_CENTER,  "Nb pulses ");
    FT8XX_CMD_number(8, 430, 80, 22, OPT_CENTER, QEI_get_pulse(QEI_2));
    FT8XX_CMD_text(9, 360, 100, 22, OPT_CENTER, "Speed rpm ");
    FT8XX_CMD_number(9, 430, 100, 22, OPT_CENTER, QEI_get_speed_rpm(QEI_2));    
    FT8XX_CMD_gradient(0, 0, 0, 0x00AA00, 480, 272, 0x5555FF);  
    FT8XX_CMD_clock(0, 240, 136, 70, OPT_FLAT | OPT_NOBACK, 16, 20, 0, 0);  
    
    FT8XX_CMD_text(10, 60, 130, 22, OPT_CENTER, "AN0 RawCnt ");
    FT8XX_CMD_number(10, 140, 130, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(11, 60, 150, 22, OPT_CENTER, "AN1 RawCnt ");
    FT8XX_CMD_number(11, 140, 150, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(12, 60, 170, 22, OPT_CENTER, "AN2 RawCnt ");
    FT8XX_CMD_number(12, 140, 170, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(13, 60, 190, 22, OPT_CENTER, "AN12 RawCnt ");
    FT8XX_CMD_number(13, 140, 190, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(14, 60, 210, 22, OPT_CENTER, "AN13 RawCnt ");
    FT8XX_CMD_number(14, 140, 210, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(15, 60, 230, 22, OPT_CENTER, "AN14 RawCnt ");
    FT8XX_CMD_number(15, 140, 230, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(16, 60, 250, 22, OPT_CENTER, "AN15 RawCnt ");
    FT8XX_CMD_number(16, 140, 250, 22, OPT_CENTER, 0);  
    
    FT8XX_CMD_text(17, 360, 130, 22, OPT_CENTER, "AN0 Volt -> ");
    FT8XX_CMD_number(17, 440, 130, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(18, 360, 150, 22, OPT_CENTER, "AN1 Volt -> ");
    FT8XX_CMD_number(18, 440, 150, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(19, 360, 170, 22, OPT_CENTER, "AN2 Volt -> ");
    FT8XX_CMD_number(19, 440, 170, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(20, 360, 190, 22, OPT_CENTER, "AN12 Volt -> ");
    FT8XX_CMD_number(20, 440, 190, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(21, 360, 210, 22, OPT_CENTER, "AN13 Volt -> ");
    FT8XX_CMD_number(21, 440, 210, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(22, 360, 230, 22, OPT_CENTER, "AN14 Volt -> ");
    FT8XX_CMD_number(22, 440, 230, 22, OPT_CENTER, 0);  
    FT8XX_CMD_text(23, 360, 250, 22, OPT_CENTER, "AN15 Volt -> ");
    FT8XX_CMD_number(23, 440, 250, 22, OPT_CENTER, 0);    
    
    
    FT8XX_start_new_dl();					// Start a new display list, reset ring buffer and ring pointer
    FT8XX_write_dl_long(CLEAR(1, 1, 1));
    FT8XX_write_dl_long(TAG_MASK(1));    
    FT8XX_write_dl_long(TAG(st_Gradient[0].touch_tag));
    FT8XX_draw_gradient(&st_Gradient[0]);
    FT8XX_write_dl_long(COLOR_RGB(255, 255, 255));
    FT8XX_draw_clock(&st_Clock[0]);
    FT8XX_update_screen_dl();         		// Update display list    
    
    FT8XX_write_bitmap(image, image_lut, 8160, 0);
#endif
    
    //UART_putbuf_dma(UART_3, (uint8_t *)"dsPeak - UART3 test 12345ABCDEF", strlen("dsPeak - UART3 test 12345ABCDEF"));
    
    RGB_LED_set_color(0x000000);
    
    MOTOR_set_rpm(MOTOR_1, speed_rpm_table[0]);
    MOTOR_set_rpm(MOTOR_2, speed_rpm_table[0]);
    
//    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER_1, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER_2, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER_3, TIMER_PRESCALER_256, 60);
    TIMER_init(TIMER_4, TIMER_PRESCALER_256, 60);
    TIMER_init(TIMER_5, TIMER_PRESCALER_256, 5);
    TIMER_init(TIMER_7, TIMER_PRESCALER_256, QEI_get_fs(QEI_2));
    TIMER_init(TIMER_8, TIMER_PRESCALER_256, QEI_get_fs(QEI_1));
    //TIMER_init(TIMER_9, TIMER_PRESCALER_256, 60000);

    PMP_init(PMP_MODE_SRAM);
    for (i=0; i<10; i++)
    {
        PMP_write_single(PMP_MODE_SRAM, mem_adr[i], mem_dat[i]);
        if (PMP_read_single(PMP_MODE_SRAM, mem_adr[i]) != mem_dat[i])
        {
            LATHbits.LATH9 = !LATHbits.LATH9;
        }   
        else
        {
            LATHbits.LATH10 = !LATHbits.LATH10;
        }  
        __delay_ms(100);
    }
    
    ST7735_init();
    color = RGB888_to_RGB565(0x00FF00);
    ST7735_Clear(color);
            
    TIMER_start(TIMER_1);
    TIMER_start(TIMER_2);
    TIMER_start(TIMER_3);
    TIMER_start(TIMER_4);
    TIMER_start(TIMER_5);
    TIMER_start(TIMER_7);
    TIMER_start(TIMER_8);
    //TIMER_start(TIMER_9);
    while (1)
    {           
        if (TIMER_get_state(TIMER_1, TIMER_INT_STATE) == 1)
        {
            setpoint_rpm = MOTOR_get_setpoint_rpm(MOTOR_1);
            actual_rpm = QEI_get_speed_rpm(QEI_1);
            error_rpm = setpoint_rpm - actual_rpm;
            buf[0] = 0xAE;
            buf[1] = 0xAE;
            buf[2] = ((setpoint_rpm & 0xFF00)>>8);
            buf[3] = setpoint_rpm&0x00FF;    
            buf[4] = ((error_rpm & 0xFF00)>>8);
            buf[5] = error_rpm&0x00FF;    
            buf[6] = ((actual_rpm & 0xFF00)>>8);
            buf[7] = actual_rpm&0x00FF; 
            buf[8] = 0;
            buf[9] = new_pid_out1;
            setpoint_rpm = MOTOR_get_setpoint_rpm(MOTOR_2);
            actual_rpm = QEI_get_speed_rpm(QEI_2);
            error_rpm = setpoint_rpm - actual_rpm;            
            buf[10] = ((setpoint_rpm & 0xFF00)>>8);
            buf[11] = setpoint_rpm&0x00FF;    
            buf[12] = ((error_rpm & 0xFF00)>>8);
            buf[13] = error_rpm&0x00FF;    
            buf[14] = ((actual_rpm & 0xFF00)>>8);
            buf[15] = actual_rpm&0x00FF; 
            buf[16] = 0;
            buf[17] = new_pid_out2;            
            UART_putbuf_dma(UART_3, buf, 18);
        }     
        
        if (TIMER_get_state(TIMER_2, TIMER_INT_STATE) == 1)
        {           
#ifdef SCREEN_ENABLE    
            FT8XX_modify_number(&st_Number[0], NUMBER_VAL, QEI_get_tour(QEI_1));
            FT8XX_modify_number(&st_Number[1], NUMBER_VAL, MOTOR_get_setpoint_rpm(MOTOR_1));
            FT8XX_modify_number(&st_Number[2], NUMBER_VAL, QEI_get_velocity(QEI_1));
            FT8XX_modify_number(&st_Number[3], NUMBER_VAL, QEI_get_pulse(QEI_1));
            FT8XX_modify_number(&st_Number[4], NUMBER_VAL, QEI_get_speed_rpm(QEI_1));
            
            FT8XX_modify_number(&st_Number[5], NUMBER_VAL, QEI_get_tour(QEI_2));
            FT8XX_modify_number(&st_Number[6], NUMBER_VAL, MOTOR_get_setpoint_rpm(MOTOR_2));
            FT8XX_modify_number(&st_Number[7], NUMBER_VAL, QEI_get_velocity(QEI_2));
            FT8XX_modify_number(&st_Number[8], NUMBER_VAL, QEI_get_pulse(QEI_2));
            FT8XX_modify_number(&st_Number[9], NUMBER_VAL, QEI_get_speed_rpm(QEI_2)); 
            
            FT8XX_modify_number(&st_Number[10], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN0));
            FT8XX_modify_number(&st_Number[11], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN1));
            FT8XX_modify_number(&st_Number[12], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN2));
            FT8XX_modify_number(&st_Number[13], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN12));
            FT8XX_modify_number(&st_Number[14], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN13));
            FT8XX_modify_number(&st_Number[15], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN14));
            FT8XX_modify_number(&st_Number[16], NUMBER_VAL, ADC_get_raw_channel(&ADC_struct_AN15));

            FT8XX_modify_number(&st_Number[17], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN0));
            FT8XX_modify_number(&st_Number[18], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN1));
            FT8XX_modify_number(&st_Number[19], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN2));
            FT8XX_modify_number(&st_Number[20], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN12));
            FT8XX_modify_number(&st_Number[21], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN13));
            FT8XX_modify_number(&st_Number[22], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN14));
            FT8XX_modify_number(&st_Number[23], NUMBER_VAL, ADC_get_eng_channel(&ADC_struct_AN15));            
            
            FT8XX_start_new_dl();                               // Start a new display list, reset ring buffer and ring pointer
            FT8XX_write_dl_long(CLEAR(1, 1, 1));
            FT8XX_write_dl_long(BITMAP_HANDLE(0));
            FT8XX_write_dl_long(BITMAP_SOURCE(0));
            FT8XX_write_dl_long(BITMAP_LAYOUT(PALETTED, 120, 68));
            FT8XX_write_dl_long(BITMAP_SIZE(NEAREST, BORDER, BORDER, 120, 68));
            FT8XX_write_dl_long(BEGIN(BITMAPS));
            FT8XX_write_dl_long(VERTEX2II(178, 102, 0, 0));            
            FT8XX_write_dl_long(TAG_MASK(1));    
            FT8XX_write_dl_long(TAG(st_Gradient[0].touch_tag));
            //FT8XX_draw_gradient(&st_Gradient[0]);
            FT8XX_write_dl_long(COLOR_RGB(255, 255, 255));
            FT8XX_draw_clock(&st_Clock[0]);
            
            FT8XX_draw_text(&st_Text[0]);
            FT8XX_draw_text(&st_Text[1]);
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
            FT8XX_draw_text(&st_Text[16]);     
            FT8XX_draw_text(&st_Text[17]);
            FT8XX_draw_text(&st_Text[18]);
            FT8XX_draw_text(&st_Text[19]);
            FT8XX_draw_text(&st_Text[20]);
            FT8XX_draw_text(&st_Text[21]);
            FT8XX_draw_text(&st_Text[22]);
            FT8XX_draw_text(&st_Text[23]);            
            
            FT8XX_draw_number(&st_Number[0]);
            FT8XX_draw_number(&st_Number[1]);
            FT8XX_draw_number(&st_Number[2]);
            FT8XX_draw_number(&st_Number[3]);
            FT8XX_draw_number(&st_Number[4]);                        
            FT8XX_draw_number(&st_Number[5]);
            FT8XX_draw_number(&st_Number[6]);
            FT8XX_draw_number(&st_Number[7]);
            FT8XX_draw_number(&st_Number[8]);
            FT8XX_draw_number(&st_Number[9]);
            
            FT8XX_draw_number(&st_Number[10]);
            FT8XX_draw_number(&st_Number[11]);
            FT8XX_draw_number(&st_Number[12]);
            FT8XX_draw_number(&st_Number[13]);
            FT8XX_draw_number(&st_Number[14]);
            FT8XX_draw_number(&st_Number[15]);
            FT8XX_draw_number(&st_Number[16]);
            FT8XX_draw_number(&st_Number[17]);
            FT8XX_draw_number(&st_Number[18]);
            FT8XX_draw_number(&st_Number[19]);
            FT8XX_draw_number(&st_Number[20]);
            FT8XX_draw_number(&st_Number[21]);
            FT8XX_draw_number(&st_Number[22]);
            FT8XX_draw_number(&st_Number[23]);            
            
            FT8XX_update_screen_dl();                           // Update display list 
#endif
        }
        
        if (TIMER_get_state(TIMER_3, TIMER_INT_STATE) == 1)
        { 
//            RGB_LED_set_color(0x000000);
//            if (++RGB_test > 255){RGB_test = 0;}            
            if (++counter_5sec >= 300)
            {                  
                counter_5sec = 0;
                state++;
                if (state > 4){state = 0;}
                MOTOR_set_rpm(MOTOR_1, speed_rpm_table[state]);
                MOTOR_set_rpm(MOTOR_2, speed_rpm_table[state]);
            }
            if (++counter_sec >= 60)
            {                    
                CAN_send_txmsg(&CAN_native);
                LATHbits.LATH8 = !LATHbits.LATH8;
                RTCC_read_time();
                hour = RTCC_get_time_parameter(RTC_HOUR);
                minute = RTCC_get_time_parameter(RTC_MINUTE);
                second = RTCC_get_time_parameter(RTC_SECOND);
#ifdef SCREEN_ENABLE
                FT8XX_modify_clock_hms(&st_Clock[0], hour, minute, second);
#endif
                counter_sec = 0; 
            }
        }

        // MikroBus RS-485 click test ------------------------------------------
        if (TIMER_get_state(TIMER_4, TIMER_INT_STATE) == 1)
        {   
            
            if (rs485_state == 0)
            {
                UART_putstr(UART_2, "Test RS-485$%?&*");
                if (UART_rx_done(UART_1) == 1)
                {
                    rs485_state = 1;
                    LATHbits.LATH11 = 1;
                    //UART_putstr(UART_2, "Test RS-485$%?&*");
                }
            }
            else
            {
                UART_putstr(UART_1, "Test RS-485$%?&*");
                if (UART_rx_done(UART_2) == 1)
                {
                    rs485_state = 0;
                    LATHbits.LATH11 = 0;
                    //UART_putstr(UART_2, "Test RS-485$%?&*");
                }                
            }
        } 

        if (TIMER_get_state(TIMER_5, TIMER_INT_STATE) == 1)
        {
            switch(color_counter)
            {
                case 0:
                    color = RGB888_to_RGB565(0x000000);
                    break;
                case 1:
                    color = RGB888_to_RGB565(0x0000FF);
                    break; 
                case 2:
                    color = RGB888_to_RGB565(0x00FF00);
                    break;
                case 3:
                    color = RGB888_to_RGB565(0x00FFFF);
                    break;      
                case 4:
                    color = RGB888_to_RGB565(0xFF0000);
                    break;
                case 5:
                    color = RGB888_to_RGB565(0xFF00FF);
                    break;      
                case 6:
                    color = RGB888_to_RGB565(0xFFFF00);
                    break;
                case 7:
                    color = RGB888_to_RGB565(0xFFFFFF);
                    break;                          
            }
            if (++color_counter > 7){color_counter = 0;}
            //ST7735_Clear(color); 
        }
        
        // QEI velocity refresh rate
        if (TIMER_get_state(TIMER_7, TIMER_INT_STATE) == 1)
        {           
            QEI_calculate_velocity(QEI_2);  
            new_pid_out2 = MOTOR_drive_pid(MOTOR_2);
            MOTOR_drive_perc(MOTOR_2, MOTOR_get_direction(MOTOR_2), new_pid_out2);
        }        
        
        // QEI velocity refresh rate
        if (TIMER_get_state(TIMER_8, TIMER_INT_STATE) == 1)
        {           
            QEI_calculate_velocity(QEI_1);  
            new_pid_out1 = MOTOR_drive_pid(MOTOR_1);
            MOTOR_drive_perc(MOTOR_1, MOTOR_get_direction(MOTOR_1), new_pid_out1);
        }
        
        // SWPWM RGB LED timer
//        if (TIMER_get_state(TIMER_9, TIMER_INT_STATE) == 1)
//        {                   
//            if (RGB_PWM_counter > RGB_R_duty){RGB_LED_RED = 1;}
//            if (RGB_PWM_counter > RGB_G_duty){RGB_LED_GREEN = 1;}
//            if (RGB_PWM_counter > RGB_B_duty){RGB_LED_BLUE = 1;}
// 
//            if (RGB_PWM_counter >= RGB_PWM_frequency)
//            {
//                if (RGB_R_duty > 0){RGB_LED_RED = 0;}
//                if (RGB_G_duty > 0){RGB_LED_GREEN = 0;}
//                if (RGB_B_duty > 0){RGB_LED_BLUE = 0;}
//                RGB_PWM_counter = 0;
//            }
//            RGB_PWM_counter++;       
//        }        
    }
    return 0;
}

void DSPIC_init (void)
{ 
    INTCON1bits.NSTDIS = 1;                         // Nested interrupt disabled 
    RCONbits.SWDTEN=0;                              // Watchdog timer disabled 
    
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    // Input HS OSC 20MHz. Output required is 140MHz for 70MIPS  
    // Configure PLL prescaler (N1), PLL postscaler (N2), PLL divisor (M)
    // Where Fout = Fin * (M / (N1 x N2))
    // M = PLLDIV + 2
    // N1 = PLLPRE + 2
    // N2 = 2 x (PLLPOST + 1)
    // Refer to dsPIC33E FRM - Oscillator (DS70850C) p.23 figure 7-8 for PLL
    // limitations for the M, N1 and N2 values
    //
    // PLLPRE = 3, N1 = 5
    // PLLDIV = 68, M = 70
    // PLLPOST = 0, N2 = 2
    // Fout = 20MHz * (140 / (10*2)) = 140MHz = 70MIPS
    PLLFBDbits.PLLDIV = 68; 
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 3;
    
    // Initiate Clock Switch to Primary oscillator with PLL
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);    // While COSC doesn't read back Primary Oscillator config
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
    
    // Set default system time
    clock.second = 0;
    clock.minute = 20;
    clock.hour = 16;
    clock.weekday = 4;
    clock.date = 4;
    clock.month = 4;
    clock.year = 2020;
    
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELG = 0;
    
    // On-board LEDs (LED1..4)
    TRISHbits.TRISH8 = 0;
    TRISHbits.TRISH9 = 0;
    TRISHbits.TRISH10 = 0;
    TRISHbits.TRISH11 = 0;
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

void hex_to_ascii (uint8_t ucByte, uint8_t *ucByteH, uint8_t *ucByteL)
{
    *ucByteH = ucByte >> 4;	   
    *ucByteL = (ucByte & 0x0F);
    if ((*ucByteH < 10) && (*ucByteH >= 0)) 
    {
        *ucByteH = *ucByteH + 0x30;			
    }
    
    else if ((*ucByteH >= 0x0A) && (*ucByteH <= 0x0F))
    {
        *ucByteH = (*ucByteH + 0x37);		
    }
    
    if ((*ucByteL < 10) && (*ucByteL >= 0))	
    {
        *ucByteL = (*ucByteL + 0x30);		
    }
    
    else if ((*ucByteL >= 0x0A) && (*ucByteL <= 0x0F))
    {
        *ucByteL = (*ucByteL + 0x37);
    }   
}

int bcd_to_decimal(uint8_t x) 
{
    return x - 6 * (x >> 4);
}

uint16_t dec2bcd(uint16_t num) // num is now 65535
{
    uint16_t ones = 0;
    uint16_t tens = 0;
    uint16_t temp = 0;

    ones = num%10; // 65535%10 = 5
    temp = num/10; // 65535/10 = 6553
    tens = temp<<4;  // what's displayed is by tens is actually the lower
                     // 4 bits of tens, so tens is 6553%16=9
    return (tens + ones);// so the result is 95
}

uint8_t hex_to_dec (uint8_t hex)
{
    uint8_t val;
    uint8_t decimal;
    if(hex>='0' && hex<='9')  
    {  
        val = hex - 48;  
    }  
    else if(hex>='a' && hex<='f')  
    {  
        val = hex - 97 + 10;  
    }  
    else if(hex>='A' && hex<='F')  
    {  
        val = hex - 65 + 10;  
    }
    decimal = val * pow(16, 1);  
    return decimal;    
}