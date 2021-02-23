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
#pragma config BOREN = OFF              // Brown-out Reset (BOR) Detection Enable bit (BOR is disabled)
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

unsigned char flag = 0;
unsigned int counter = 0;
unsigned char buf[2] = {0, 0};
unsigned char test_spi[8] = {'J', 'e', 't', 'a', 'i', 'm', 'e', '!'};
unsigned char *ptr;

RTCC_time clock;
unsigned char hour, minute, second;

int main() 
{
    DSPIC_init();
    RTCC_init();
    RTCC_write_time(clock);
    UART_init(UART_1, 115200, 16);
    UART_init(UART_2, 115200, 2);
    UART_init(UART_3, 115200, 1);
    
    PWM_init(PWM_1L, PWM_TYPE_LOAD);
    PWM_init(PWM_1H, PWM_TYPE_LOAD);
    PWM_init(PWM_5H, PWM_TYPE_SERVO);
    PWM_init(PWM_6L, PWM_TYPE_SERVO);
    
    CODEC_init(SYS_FS_48kHz);
    
    SPI_flash_init();
    SPI_flash_write_enable();
    SPI_flash_erase(CMD_BLOCK_ERASE_4k, 0);
    SPI_flash_write(0x100, test_spi, 8);
    
//    TIMER_init(TIMER_1, 1000);
//    TIMER_init(TIMER_2, 1000);
//    TIMER_start(TIMER_1);
//    TIMER_start(TIMER_2);
//    
//    QEI_init(QEI_MOT1);
//    QEI_set_fs(QEI_MOT1, 1000);
    
    FT8XX_init();
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
    
    UART_putstr(UART_3, "dsPeak - UART3 test 12345ABCDEF");
    
    PWM_change_duty(PWM_1H, PWM_TYPE_LOAD, 25);
    
    while (1)
    {
        RTCC_read_time();
        hour = RTCC_get_time_parameter(RTC_HOUR);
        minute = RTCC_get_time_parameter(RTC_MINUTE);
        second = RTCC_get_time_parameter(RTC_SECOND);
        FT8XX_modify_clock_hms(&st_Clock[0], hour, minute, second);
        FT8XX_start_new_dl();					// Start a new display list, reset ring buffer and ring pointer
        
        FT8XX_write_dl_long(CLEAR(1, 1, 1));
        FT8XX_write_dl_long(TAG_MASK(1));    
        FT8XX_write_dl_long(TAG(st_Gradient[0].touch_tag));
        FT8XX_draw_gradient(&st_Gradient[0]);
        FT8XX_write_dl_long(COLOR_RGB(255, 255, 255));
        FT8XX_draw_clock(&st_Clock[0]);
        FT8XX_update_screen_dl();         		// Update display list  
        
        __delay_ms(10);
        UART_putstr(UART_3, "Time : ");  
        UART_putc(UART_3, dec2bcd((hour & 0xF0)>>4)+0x30);
        UART_putc(UART_3, dec2bcd(hour & 0x0F)+0x30);
        UART_putc(UART_3,':');
        UART_putc(UART_3, dec2bcd((minute & 0xF0)>>4)+0x30);
        UART_putc(UART_3, dec2bcd(minute & 0x0F)+0x30);
        UART_putc(UART_3,':');
        UART_putc(UART_3, dec2bcd((second & 0xF0)>>4)+0x30);
        UART_putc(UART_3, dec2bcd(second & 0x0F)+0x30);
        UART_putc(UART_3, 0x0D);
        UART_putc(UART_3, 0x0A);
//        if (TIMER_get_state(TIMER_1, TIMER_INT_STATE))
//        {
//            QEI_calculate_velocity(QEI_MOT1);
//        }
        
//        if (TIMER_get_state(TIMER_2, TIMER_INT_STATE))
//        {
//            if (++counter > 1000)
//            {
//                counter = 0;
//                unsigned long pulse = QEI_get_pulse(QEI_MOT1);
//                unsigned int velocity = QEI_get_velocity(QEI_MOT1);
//                UART_putstr(UART_3, "QEI pos cnt : ");
//                UART_putc_ascii(UART_3, ((pulse & 0x0000FF00)>>8));    
//                UART_putc_ascii(UART_3, pulse);
//                buf[0] = 0x0D; buf[1] = 0x0A;
//                UART_putbuf(UART_3, buf, 2);
//                UART_putstr(UART_3, "QEI vel rpm : ");
//                UART_putc_ascii(UART_3, ((velocity & 0x0000FF00)>>8));    
//                UART_putc_ascii(UART_3, velocity);      
//                buf[0] = 0x0D; buf[1] = 0x0A;
//                UART_putbuf(UART_3, buf, 2);                
//            }
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
    
    TRISHbits.TRISH8 = 0;
    TRISHbits.TRISH9 = 0;
    TRISHbits.TRISH10 = 0;
    TRISHbits.TRISH11 = 0;
    
    TRISDbits.TRISD8 = 0;
    LATDbits.LATD8 = 0;
    
    TRISBbits.TRISB3 = 0;
    LATBbits.LATB3 = 1;
    
    TRISBbits.TRISB4 = 0;
    LATBbits.LATB4 = 1;
    
    TRISBbits.TRISB5 = 0;
    LATBbits.LATB5 = 1;
    
    TRISAbits.TRISA5 = 1;
    
    TRISCbits.TRISC2 = 0;
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