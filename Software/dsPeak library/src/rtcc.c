#include "rtcc.h"

RTCC_time rtcc_time;

void RTCC_init (void)
{
    //TRISDbits.TRISD8 = 0;                     // RD8 configured as an output (RTCC_ALARM)
    
    TRISCbits.TRISC14 = 1;
    CNPDCbits.CNPDC14 = 1;
    __builtin_write_OSCCONL(OSCCON | 0x02); // Enable secondary oscillator
    __delay_us(100);
    while(OSCCONbits.LPOSCEN != 1);
    __builtin_write_RTCWEN();               // Enable access to the RTC registers
    __delay_us(100);
    RCFGCALbits.RTCEN = 0;
    ALCFGRPTbits.ALRMEN = 0;                // Disable the alarm as well (stops false alarms)
}

void RTCC_write_time (RTCC_time t)
{
    RCFGCALbits.RTCEN = 0;
    RCFGCALbits.RTCPTR = 3;
    RTCVAL = dec2bcd(t.year) & 0x00FF;
    RTCVAL = ((dec2bcd(t.month) << 8) | dec2bcd(t.date));
    RTCVAL = ((dec2bcd(t.weekday) << 8) | dec2bcd(t.hour));
    RTCVAL = ((dec2bcd(t.minute) << 8) | dec2bcd(t.second));
    RCFGCALbits.RTCEN = 1;
}

void RTCC_read_time (void)
{   
    unsigned int read3 = 0, read2 = 0, read1 = 0, read0 = 0;
    //while(RCFGCALbits.RTCSYNC == 1);
    RCFGCALbits.RTCPTR = 3;    
    read3 = RTCVAL;
    read2 = RTCVAL;
    read1 = RTCVAL;
    read0 = RTCVAL;

    rtcc_time.second = bcd_to_decimal(read0 & 0x00FF);
    rtcc_time.minute = bcd_to_decimal((read0 & 0xFF00)>>8);
    rtcc_time.hour = bcd_to_decimal(read1 & 0x00FF);
    rtcc_time.weekday = bcd_to_decimal((read1 & 0xFF00)>>8);
    rtcc_time.date = bcd_to_decimal(read2 & 0x00FF);
    rtcc_time.month = bcd_to_decimal((read2 & 0xFF00)>>8);
    rtcc_time.year = bcd_to_decimal(read3);
}

unsigned char RTCC_get_time_parameter (unsigned char type)
{
    switch (type)
    {
        case RTC_SECOND:
            return rtcc_time.second;
            break;
            
        case RTC_MINUTE:
            return rtcc_time.minute;
            break;
            
        case RTC_HOUR:
            return rtcc_time.hour;
            break;
            
        case RTC_WEEKDAY:
            return rtcc_time.weekday;
            break;
            
        case RTC_DATE:
            return rtcc_time.date;
            break;
            
        case RTC_MONTH:
            return rtcc_time.month;
            break;
            
        case RTC_YEAR:
            return rtcc_time.year;
            break;
            
        default:
            return 0;
            break;
    }
}
