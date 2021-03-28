//****************************************************************************//
// File      : main.c
//
// Includes  : general.h
//
// Purpose   : dsPeak development project
//
//Jean-Francois Bilodeau    MPLab X v5.45    13/01/2021  
//****************************************************************************//

//----------------------------------------------------------------------------//
// dsPIC33EP512MU814 configuration fuses, do not remove
// Configuration settings can be changed in "Production -> Set configuration bits"
// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Primary oscillator at POR is XT, HS or EC, without PLL
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
#pragma config BOREN = ON               // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled)
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

#define SCREEN_ENABLE

CAN_struct CAN_native;

unsigned char i = 0;
unsigned char state = 0;
unsigned int counter_sec = 0;
unsigned int counter_5sec = 0;
unsigned int counter_timer1 = 0;
unsigned long rgb_r = 0, rgb_g = 0, rgb_b = 0;
unsigned int counter_ms = 0;
unsigned int counter_100us = 0;
unsigned int hour_bcd = 0, minute_bcd = 0, second_bcd = 0;
unsigned char buf[128] = {0};
unsigned char test_spi[8] = {'J', 'e', 't', 'a', 'i', 'm', 'e', '!'};
char *str = "-- Testing a full 64 bytes transfer from DPSRAM->DMA->USART3!\r\n";
unsigned int test = 1;
unsigned char speed = 20;
char new_pid_out1 = 0, new_pid_out2 = 0;
unsigned int speed_rpm_table[5] = {30, 30, 30, 30, 30};

RTCC_time clock;
unsigned char hour, minute, second;
int error_rpm;
unsigned int setpoint_rpm;
unsigned int actual_rpm;
unsigned char high, low;
unsigned char rs485_state = 0;
unsigned char pid_out = 0;
unsigned int color = 0;
unsigned char color_counter = 0;
unsigned long color_888 = 0;

unsigned int RGB_PWM_frequency = 60;
unsigned char RGB_PWM_counter = 0;
unsigned char RGB_R_duty = 0;
unsigned char RGB_G_duty = 0;
unsigned char RGB_B_duty = 0;
unsigned char RGB_test = 0;
int main() 
{
    DSPIC_init();
    RTCC_init();
    RTCC_write_time(clock);
    UART_init(UART_1, 115200, 16);
    UART_init(UART_2, 115200, 16);
    UART_init(UART_3, 115200, 1);
    
    MOTOR_init(MOTOR_1, 30);
    MOTOR_init(MOTOR_2, 30);            

    PWM_init(PWM_5H, PWM_TYPE_SERVO);
    PWM_init(PWM_6L, PWM_TYPE_SERVO);
   
    CODEC_init(SYS_FS_48kHz);
    
    SPI_flash_init();
    SPI_flash_write_enable();
    SPI_flash_erase(CMD_BLOCK_ERASE_4k, 0);
    SPI_flash_write(0x100, test_spi, 8);
    
    CAN_native.bus_freq = 500000;
    CAN_native.channel = CAN_1;
    CAN_native.SID = 0x0123;
    CAN_native.SRR = 0;
    CAN_native.IDE = 0;
    CAN_native.EID = 0;
    CAN_native.RTR = 0;
    CAN_native.RB1 = 0;
    CAN_native.RB0 = 0;
    CAN_native.DLC = 8;
    CAN_native.can_payload[0] = 'd';
    CAN_native.can_payload[1] = 's';
    CAN_native.can_payload[2] = 'P';
    CAN_native.can_payload[3] = 'e';
    CAN_native.can_payload[4] = 'a';
    CAN_native.can_payload[5] = 'k';
    CAN_native.can_payload[6] = '!';
    CAN_native.can_payload[7] = '-';
    CAN_native.RX_MASK = 0x0300;
    CAN_native.RX_SID = CAN_native.RX_MASK;   
    
    CAN_init(&CAN_native);
    CAN_set_mode(&CAN_native, CAN_MODE_NORMAL);
    
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
    
    //UART_putbuf_dma(UART_3, (unsigned char *)"dsPeak - UART3 test 12345ABCDEF", strlen("dsPeak - UART3 test 12345ABCDEF"));
    
    RGB_LED_set_color(0xFFFFFF);
    
    MOTOR_set_rpm(MOTOR_1, speed_rpm_table[0]);
    MOTOR_set_rpm(MOTOR_2, speed_rpm_table[0]);
    
//    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER_1, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER_2, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER_3, TIMER_PRESCALER_256, 60);
    TIMER_init(TIMER_4, TIMER_PRESCALER_256, 60);
    TIMER_init(TIMER_5, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER_7, TIMER_PRESCALER_256, QEI_get_fs(QEI_2));
    TIMER_init(TIMER_8, TIMER_PRESCALER_256, QEI_get_fs(QEI_1));
    TIMER_init(TIMER_9, TIMER_PRESCALER_256, 60000);

    ST7735_init();
    color = RGB888_to_RGB565(0x00FF00);
    ST7735_Clear(color);
    
    PMP_write(PMP_MODE_SRAM, 0x0001, 0x009C);
    if (PMP_read(PMP_MODE_SRAM, 0x0001) != 0x009C)
    {
        LATHbits.LATH9 = !LATHbits.LATH9;
    }   
    else
    {
        LATHbits.LATH10 = !LATHbits.LATH10;
    }
        
    TIMER_start(TIMER_1);
    TIMER_start(TIMER_2);
    TIMER_start(TIMER_3);
    TIMER_start(TIMER_4);
    TIMER_start(TIMER_5);
    TIMER_start(TIMER_7);
    TIMER_start(TIMER_8);
    TIMER_start(TIMER_9);
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
            
            FT8XX_update_screen_dl();                           // Update display list 
#endif
        }
        
        if (TIMER_get_state(TIMER_3, TIMER_INT_STATE) == 1)
        { 
            RGB_LED_set_color(RGB_test);
            if (++RGB_test > 255){RGB_test = 0;}            
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
                CAN_tx_msg(&CAN_native);
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
            ST7735_Clear(color); 
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
        if (TIMER_get_state(TIMER_9, TIMER_INT_STATE) == 1)
        {                   
            if (RGB_PWM_counter > RGB_R_duty){RGB_LED_RED = 1;}
            if (RGB_PWM_counter > RGB_G_duty){RGB_LED_GREEN = 1;}
            if (RGB_PWM_counter > RGB_B_duty){RGB_LED_BLUE = 1;}
 
            if (RGB_PWM_counter >= RGB_PWM_frequency)
            {
                if (RGB_R_duty > 0){RGB_LED_RED = 0;}
                if (RGB_G_duty > 0){RGB_LED_GREEN = 0;}
                if (RGB_B_duty > 0){RGB_LED_BLUE = 0;}
                RGB_PWM_counter = 0;
            }
            RGB_PWM_counter++;       
        }        
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
    
    // SWPWM RGB LED pins
    TRISBbits.TRISB3 = 0;   // RGB LED - Red   
    LATBbits.LATB3 = 1;     // RGB LED - Red
    TRISBbits.TRISB4 = 0;   // RGB LED - Green 
    LATBbits.LATB4 = 1;     // RGB LED - Green   
    TRISBbits.TRISB5 = 0;   // RGB LED - Blue 
    LATBbits.LATB5 = 1;     // RGB LED - Blue 
}

void RGB_LED_set_color (unsigned long color)
{
    unsigned char R = ((color & 0xFF0000)>>16);
    unsigned char G = ((color & 0x00FF00)>>8);
    unsigned char B = color & 0x0000FF;
    
    RGB_R_duty = (R * (RGB_PWM_frequency/255.0));
    RGB_G_duty = (G * (RGB_PWM_frequency/255.0));
    RGB_B_duty = (B * (RGB_PWM_frequency/255.0));
}

void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL)
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

int bcd_to_decimal(unsigned char x) 
{
    return x - 6 * (x >> 4);
}

unsigned int dec2bcd(unsigned int num) // num is now 65535
{
    unsigned int ones = 0;
    unsigned int tens = 0;
    unsigned int temp = 0;

    ones = num%10; // 65535%10 = 5
    temp = num/10; // 65535/10 = 6553
    tens = temp<<4;  // what's displayed is by tens is actually the lower
                     // 4 bits of tens, so tens is 6553%16=9
    return (tens + ones);// so the result is 95
}

unsigned char hex_to_dec (unsigned char hex)
{
    unsigned char val;
    unsigned char decimal;
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