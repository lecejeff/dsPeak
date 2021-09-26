#ifndef __ROT_ENCODER_H__
#define __ROT_ENCODER_H__

#include "dsPeak_generic.h"

typedef struct
{
    uint8_t direction;
    uint32_t counter;
    uint32_t velocity_counter;
    uint32_t count_per_rev;
    uint16_t tour_counter;
    uint16_t velocity_rpm;
    uint8_t switch_state;
    uint8_t switch_debouncer;
    uint16_t maximum_cpr;
    uint16_t position;
    uint8_t A_state;
    uint8_t B_state;
    uint8_t channel_change;
    uint32_t fs_velocity;
}STRUCT_ENCODER;

void ENCODER_init (uint32_t fs_velocity);
uint8_t ENCODER_get_direction (void);
uint16_t ENCODER_get_velocity (void);
uint16_t ENCODER_get_position (void);
uint8_t ENCODER_get_switch_state (void);
uint16_t ENCODER_get_velocity (void);
void ENCODER_interrupt_handle (void);

#define ENCODER_MAX_CPR 24

#endif