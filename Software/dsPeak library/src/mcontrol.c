#include "mcontrol.h"
STRUCT_MCONTROL m_control[MOTOR_QTY];


void MOTOR_init (unsigned char channel, unsigned int speed_fs)
{
    switch (channel)
    {
        case MOTOR_1:
            PWM_init(PWM_1L, PWM_TYPE_LOAD);    // Configure PWM_1L pin to output and enable PWM
            PWM_init(PWM_1H, PWM_TYPE_LOAD);    // Configure PWM_1H pin to output and enable PWM
            QEI_init(QEI_1);                    // Configure QEI_1 
            QEI_set_fs(QEI_1, speed_fs);        // Set QEI channel velocity refresh rate
            TRISGbits.TRISG14 = 1;              // RG14 configured as an input (nFAULT1)
            m_control[channel].direction = DIRECTION_FORWARD;
            m_control[channel].speed_perc = 0;    
            m_control[channel].speed_rpm = 0;    
            m_control[channel].pwm_h_channel = PWM_1H;
            m_control[channel].pwm_l_channel = PWM_1L;
            m_control[channel].qei_channel = QEI_1;
            
            m_control[channel].pid_fs = QEI_get_fs(QEI_1);
            m_control[channel].pid_p_gain = 0.55;
            m_control[channel].pid_i_gain = 5;
            m_control[channel].pid_d_gain = 0.0000001;
            m_control[channel].max_rpm = QEI_get_max_rpm(QEI_1);
            m_control[channel].min_rpm = 0;
            m_control[channel].pid_high_limit = 100;        // Max output is 100% ON duty cycle to PWM
            MOTOR_pid_calc_gains(MOTOR_1);
            break;
            
        case MOTOR_2:
            PWM_init(PWM_2L, PWM_TYPE_LOAD);    // Configure PWM_2L pin to output and enable PWM
            PWM_init(PWM_2H, PWM_TYPE_LOAD);    // Configure PWM_2H pin to output and enable PWM
            QEI_init(QEI_2);                    // Configure QEI_1 
            TRISEbits.TRISE4 = 1;               // RE4 configured as an input (nFAULT2)
            m_control[channel].direction = DIRECTION_FORWARD;
            m_control[channel].speed_perc = 0;    
            m_control[channel].speed_rpm = 0;    
            m_control[channel].pwm_h_channel = PWM_2H;
            m_control[channel].pwm_l_channel = PWM_2L;  
            m_control[channel].qei_channel = QEI_2;
            break;

    }
}

unsigned char MOTOR_get_direction (unsigned char channel)
{
    return m_control[channel].direction;
}

unsigned int MOTOR_get_speed_rpm (unsigned char channel)
{
    return m_control[channel].actual_rpm;
}

unsigned int MOTOR_get_setpoint_rpm (unsigned char channel)
{
    return m_control[channel].speed_rpm;
}

unsigned char MOTOR_get_speed_perc (unsigned char channel)
{
    return m_control[channel].speed_perc;
}

void MOTOR_drive_perc (unsigned char channel, unsigned char direction, unsigned char perc)
{
    m_control[channel].direction = direction;
    m_control[channel].speed_perc = perc;     
    
    if (m_control[channel].direction == DIRECTION_FORWARD)
    {
        PWM_change_duty(m_control[channel].pwm_h_channel, PWM_TYPE_LOAD, m_control[channel].speed_perc);
        PWM_change_duty(m_control[channel].pwm_l_channel, PWM_TYPE_LOAD,  0);        
    }

    else
    {
        PWM_change_duty(m_control[channel].pwm_h_channel, PWM_TYPE_LOAD,  0);
        PWM_change_duty(m_control[channel].pwm_l_channel, PWM_TYPE_LOAD,  m_control[channel].speed_perc);        
    }
}

void MOTOR_set_rpm (unsigned char channel, unsigned int new_rpm)
{
    m_control[channel].speed_rpm = new_rpm;
}

void MOTOR_pid_calc_gains (unsigned char channel)
{
    m_control[channel].pid_T = 1.0 / (double)m_control[channel].pid_fs;
    m_control[channel].p_calc_gain = m_control[channel].pid_p_gain;
    m_control[channel].i_calc_gain = m_control[channel].pid_i_gain * m_control[channel].pid_T;
    m_control[channel].d_calc_gain = m_control[channel].pid_d_gain / m_control[channel].pid_T;
}

unsigned char MOTOR_drive_pid (unsigned char channel)
{
    if (m_control[channel].speed_rpm > 0)
    {           
        // Get velocity
        m_control[channel].actual_rpm = QEI_get_speed_rpm(channel);            
        if (m_control[channel].actual_rpm > m_control[channel].max_rpm)
        {
            m_control[channel].actual_rpm = m_control[channel].max_rpm;
        }

        // Error = setpoint - input
        m_control[channel].error_rpm = (double)m_control[channel].speed_rpm - (double)m_control[channel].actual_rpm;
        // Limit the error to maximum motor error, between -MAX_RPM and MAX_RPM specified in datasheet
//        if (m_control[channel].error_rpm > (double)m_control[channel].max_rpm)
//        {
//            m_control[channel].error_rpm = (double)m_control[channel].max_rpm;
//        }
//        else if (m_control[channel].error_rpm < (double)-m_control[channel].max_rpm)
//        {
//            m_control[channel].error_rpm = (double)-m_control[channel].max_rpm;
//        }
        // Calculate proportional term
        m_control[channel].p_value = m_control[channel].p_calc_gain * m_control[channel].error_rpm;
        
        // Calculate integral term
        m_control[channel].i_term += (m_control[channel].i_calc_gain * m_control[channel].error_rpm);
        // Limit the cumulative error to controls system limit, in this case PWM duty cycle = 100
        if (m_control[channel].i_term > (double)m_control[channel].pid_high_limit)
        {
            m_control[channel].i_term = (double)m_control[channel].pid_high_limit;
        }
        
        // Calculate derivative term
        m_control[channel].d_input = (double)(m_control[channel].actual_rpm - m_control[channel].last_actual_rpm);
        m_control[channel].d_value = m_control[channel].d_calc_gain * m_control[channel].d_input;

        // Compute PID output
        m_control[channel].pid_out = (unsigned char)(m_control[channel].p_value + m_control[channel].i_term - m_control[channel].d_value);   
           
        if (m_control[channel].pid_out > m_control[channel].pid_high_limit)
        {
            m_control[channel].pid_out = m_control[channel].pid_high_limit;
        }
        else if (m_control[channel].pid_out < 0)
        {
            m_control[channel].pid_out = 0;
        }
//            
        // Remember some variables
        m_control[channel].last_actual_rpm = m_control[channel].actual_rpm; 
        m_control[channel].last_error_rpm = m_control[channel].error_rpm;  
        return m_control[channel].pid_out;
    }
    else
    {
        return 0;
    }
}

