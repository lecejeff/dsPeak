#ifndef __rc_dac_h__
#define __rc_dac_h__

#include "dsPeak_generic.h"
#include "i2c.h"
#include "pwm.h"

typedef struct
{
    uint8_t PWM_channel;
    uint16_t PWM_frequency;
    uint16_t PWM_duty_cycle;
    uint8_t wiper;
}STRUCT_RCDAC;

#endif