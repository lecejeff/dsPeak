#ifndef __rtcc_h__
#define __rtcc_h__

#include "general.h"

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t weekday;
    uint8_t date;    
    uint8_t month;
    uint16_t year;
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
uint8_t RTCC_get_time_parameter (uint8_t type);

#endif