#ifndef __mcontrol_h__
#define __mcontrol_h__

#include "general.h"
#include "pwm.h"
#include "qei.h"

typedef struct
{
    unsigned char pid_out;
    double pid_p_gain;
    double pid_i_gain;
    double pid_d_gain;
    unsigned int pid_fs;
    double pid_T;
    double p_calc_gain;
    double i_calc_gain;
    double d_calc_gain;
    double i_value;          
    double p_value; 
    double d_value;
    double d_input;
    double i_term;

    unsigned int pid_high_limit;
    double error_rpm;
    double last_error_rpm;
    unsigned int actual_rpm;
    unsigned int last_actual_rpm;    
    
    unsigned int max_rpm;
    unsigned int min_rpm;
    
    unsigned int speed_rpm;
    
    unsigned char speed_perc;
    unsigned char direction;
    unsigned char pwm_h_channel;
    unsigned char pwm_l_channel;
    unsigned char qei_channel;
}STRUCT_MCONTROL;

#define MOTOR_1             0
#define MOTOR_2             1
#define MOTOR_QTY           2

#define DIRECTION_FORWARD   0
#define DIRECTION_BACKWARD  1

void MOTOR_init (unsigned char channel, unsigned int speed_fs);
void MOTOR_drive_perc (unsigned char channel, unsigned char direction, unsigned char perc);
void MOTOR_set_rpm (unsigned char channel, unsigned int new_rpm);
void MOTOR_pid_calc_gains (unsigned char channel);
double MOTOR_get_error (unsigned char channel);
unsigned char MOTOR_get_direction (unsigned char channel);
unsigned int MOTOR_get_speed_rpm (unsigned char channel);
unsigned char MOTOR_get_speed_perc (unsigned char channel);
unsigned int MOTOR_get_setpoint_rpm (unsigned char channel);
//char MOTOR_drive_pid (unsigned char channel);
unsigned char MOTOR_drive_pid (unsigned char channel);
#endif