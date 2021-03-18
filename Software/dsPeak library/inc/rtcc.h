#ifndef __rtcc_h__
#define __rtcc_h__

#include "general.h"

typedef struct
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char weekday;
    unsigned char date;    
    unsigned char month;
    unsigned int year;
}RTCC_time;

#define RTC_SECOND      1
#define RTC_MINUTE      2
#define RTC_HOUR        3
#define RTC_WEEKDAY     4
#define RTC_DATE        5
#define RTC_MONTH       6
#define RTC_YEAR        7

void RTCC_init (void);
void RTCC_write_time (RTCC_time t);
void RTCC_read_time (void);
unsigned char RTCC_get_time_parameter (unsigned char type);

#endif