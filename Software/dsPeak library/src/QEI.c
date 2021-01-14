//****************************************************************************//
// File      :  QEI.c
//
// Functions :  void QEI_init (unsigned char channel);
//              unsigned long QEI_get_pulse (unsigned char channel);
//              void QEI_reset_pulse (unsigned char channel);
//              void QEI_set_gear_derate (unsigned char channel, unsigned int new_gear_derate);
//              unsigned int QEI_get_gear_derate (unsigned char channel);
//              void QEI_set_cpr (unsigned char channel, unsigned int new_cpr);
//              unsigned int QEI_get_cpr (unsigned char channel);
//              unsigned long QEI_get_tour (unsigned char channel);
//              void QEI_reset_tour (unsigned char channel);
//              unsigned char QEI_get_event (unsigned char channel);
//              unsigned char QEI_get_direction (unsigned char channel);
//              unsigned int QEI_get_velocity (unsigned char channel);
//              void QEI_calculate_velocity (unsigned char channel);
//              void QEI_interrupt_handle (unsigned char channel);
//
// Includes  :  QEI.h
//
// Purpose   :  QEI driver for the dsPIC33EP
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
#include "QEI.h"

STRUCT_QEI QEI_struct[QEI_QTY];

//*****************void QEI_init (unsigned char channel)**********************//
//Description : Function initializes hardware QEI module (CNI pin, pos edge int)
//
//Function prototype : void QEI_init (unsigned char channel)
//
//Enter params       : unsigned char channel : channel QEI1 is CNI, QEI2 is QEIhw)
//
//Exit params        : unsigned char : 0 if channel exists
//                          1 if channel doesn't exists
//
//Function call      : unsigned char = QEI_init(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void QEI_init (unsigned char channel, unsigned int refresh_freq)
{
    QEI_struct[channel].int_event = 0;          // Reset QEI vars on initialization
    QEI_struct[channel].pulse_getter = 0;       
    QEI_struct[channel].pulse_cnter_dist = 0;
    QEI_struct[channel].direction = 0;          //
    QEI_struct[channel].pulse_cnter = 0;        //
    QEI_struct[channel].tour_cnter = 0;         //
    QEI_struct[channel].tour_getter = 0;        //
    QEI_struct[channel].pulse_for_tour = 0;
    QEI_struct[channel].speed_rpm = 0;          //
    
    QEI_struct[channel].refresh_freq = refresh_freq;                // Default values
    QEI_struct[channel].motor_gear_derate = GEAR_DERATE;            // Default values
    QEI_struct[channel].motor_cpr = COUNT_PER_REVOLUTION;           // Default values
    QEI_struct[channel].pulse_per_tour = QEI_struct[channel].motor_gear_derate * QEI_struct[channel].motor_cpr;
            
    if (channel == QEI_1)
    {
        TRISAbits.TRISA2 = 1;   // DIR A is on RA2 (input)
        TRISAbits.TRISA3 = 1;   // DIR B is on RA3 (input)
        CNENAbits.CNIEA2 = 1;   // Change notice interrupt on DIR A pin 
        CNENAbits.CNIEA3 = 1;   // Change notice interrupt on DIR B pin 
    }
}

unsigned long QEI_get_distance (unsigned char channel)
{
    return QEI_struct[channel].pulse_cnter_dist;
}

void QEI_reset_distance (unsigned char channel)
{
    QEI_struct[channel].pulse_cnter_dist = 0;
}

void QEI_set_fs (unsigned char channel, unsigned int refresh_freq)
{
    QEI_struct[channel].refresh_freq = refresh_freq;
}

//*************unsigned long QEI_get_pulse (unsigned char channel)************//
//Description : Function return amount of pulses counted by QEI
//
//Function prototype : unsigned long QEI_get_pulse (unsigned char channel)
//
//Enter params       : unsigned char channel : qei channel
//
//Exit params        : unsigned long : counted pulses
//
//Function call      : unsigned long = QEI_get_pulse(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020   
//****************************************************************************//
unsigned long QEI_get_pulse (unsigned char channel)
{
    return QEI_struct[channel].pulse_getter;
}

//*****************void QEI_reset_pulse (unsigned char channel)***************//
//Description : Function reset the pulse counter on assigned channel
//
//Function prototype : void QEI_reset_pulse (unsigned char channel)
//
//Enter params       : unsigned char channel : qei channel
//
//Exit params        : None
//
//Function call      : QEI_reset_pulse(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void QEI_reset_pulse (unsigned char channel)
{
    QEI_struct[channel].pulse_getter = 0;
    QEI_struct[channel].pulse_cnter = 0;
}

//void QEI_set_gear_derate (unsigned char channel, unsigned int new_gear_derate)//
//Description : Function sets the gear derating ratio of the motor
//
//Function prototype : void QEI_set_gear_derate (unsigned char channel, unsigned char new_gear_derate)
//
//Enter params       : unsigned char channel : QEI channel
//                     unsigned int new_gear_derate : new gear derate ratio 
//
//Exit params        : None
//
//Function call      : QEI_set_gear_derate(QEI_1, 499);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void QEI_set_gear_derate (unsigned char channel, unsigned int new_gear_derate)
{
    QEI_struct[channel].motor_gear_derate = new_gear_derate;
    QEI_struct[channel].pulse_per_tour = QEI_struct[channel].motor_gear_derate * QEI_struct[channel].motor_cpr;
    QEI_reset_tour(channel);
    QEI_reset_pulse(channel);    
}

//**********unsigned int QEI_get_gear_derate (unsigned char channel)**********//
//Description : Function return the gear derate ratio of the motor
//
//Function prototype : unsigned int QEI_get_gear_derate (unsigned char channel)
//
//Enter params       : unsigned char channel : QEI channel
//
//Exit params        : unsigned int : gear derate ratio
//
//Function call      : unsigned int = QEI_get_gear_derate(QEI_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
unsigned int QEI_get_gear_derate (unsigned char channel)
{
    return QEI_struct[channel].motor_gear_derate;
}

//******void QEI_set_cpr (unsigned char channel, unsigned int new_cpr)*******//
//Description : Function sets the CPR (count per revolution) of the QEI encoder
//
//Function prototype : void QEI_set_cpr (unsigned char channel, unsigned int new_cpr)
//
//Enter params       : unsigned char channel : QEI channel
//                     unsigned int new_cpr : new count per revolution value 
//
//Exit params        : None
//
//Function call      : QEI_set_cpr(QEI_1, 48);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void QEI_set_cpr (unsigned char channel, unsigned int new_cpr)
{
    QEI_struct[channel].motor_cpr = new_cpr;
    QEI_struct[channel].pulse_per_tour = QEI_struct[channel].motor_gear_derate * QEI_struct[channel].motor_cpr;
    QEI_reset_tour(channel);
    QEI_reset_pulse(channel);
}

//**************unsigned int QEI_get_cpr (unsigned char channel)**************//
//Description : Function return the count per revolution value
//
//Function prototype : unsigned int QEI_get_cpr (unsigned char channel)
//
//Enter params       : unsigned char channel : QEI channel
//
//Exit params        : unsigned int : count per revolution value
//
//Function call      : unsigned int = QEI_get_cpr(QEI_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
unsigned int QEI_get_cpr (unsigned char channel)
{
    return QEI_struct[channel].motor_cpr;
}

//***********************long QEI_hard_get_tour (void))***********************//
//Description : Function get counted QEI tour number since reset
//
//Function prototype : long QEI_get_tour (void)
//
//Enter params       : None
//
//Exit params        : long : Tour number since reset
//
//Function call      : long = QEI_get_tour();
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
unsigned long QEI_get_tour (unsigned char channel)
{
    return QEI_struct[channel].tour_cnter_dist;
}

//******************void QEI_reset_tour (void)********************************//
//Description : Function reset tour counter
//
//Function prototype : void QEI_reset_tour (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : QEI_reset_tour();
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020   
//****************************************************************************//
void QEI_reset_tour (unsigned char channel)
{
    QEI_struct[channel].tour_cnter_dist = 0;
    QEI_struct[channel].tour_cnter = 0;
    QEI_struct[channel].tour_getter = 0;
}

//**********unsigned char QEI_get_event (unsigned char channel)***************//
//Description : Function return QEI interrupt event state
//
//Function prototype : unsigned char QEI_get_event (unsigned char channel)
//
//Enter params       : unsigned char channel : QEIx channel
//
//Exit params        : unsigned char : Event happened or not
//
//Function call      : unsigned char = QEI_get_event(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
unsigned char QEI_get_event (unsigned char channel)
{
    if (QEI_struct[channel].int_event)
    {
        QEI_struct[channel].int_event = 0;
        return 0;
    }
    else
        return 1;
}

//*************unsigned char QEI_get_direction (unsigned char channel)********//
//Description : Function gets actual motor direction
//
//Function prototype : unsigned char QEI_get_direction (unsigned char channel)
//
//Enter params       : unsigned char channel : QEIx channel
//
//Exit params        : unsigned char : direction of motor
//
//Function call      : unsigned char = QEI_get_direction(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
unsigned char QEI_get_direction (unsigned char channel)
{
    return QEI_struct[channel].direction;
}

//*************unsigned long QEI_get_velocity (unsigned char channel)**********//
//Description : Function gets calculated motor rpm
//
//Function prototype : unsigned long QEI_get_velocity (unsigned char channel)
//
//Enter params       : unsigned char channel : QEIx channel
//
//Exit params        : unsigned long : motor rpm
//
//Function call      : unsigned long = QEI_get_velocity(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
unsigned int QEI_get_velocity (unsigned char channel)
{
    return QEI_struct[channel].speed_rpm;
}

//*********void QEI_calculate_velocity (unsigned char channel)***********//
//Description : Function computes and interpolates speed to cm/s
//
//Function prototype : void QEI_calculate_velocity (unsigned char channel)
//
//Enter params       : unsigned char channel : QEIx channel
//
//Exit params        : None
//
//Function call      : QEI_calculate_velocity(QEI_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void QEI_calculate_velocity (unsigned char channel)
{
    switch (channel)
    {
        case QEI_1:
            // Update the actual speed
            QEI_struct[channel].new_pulse = QEI_get_pulse(QEI_1);
            if (QEI_struct[channel].new_pulse > 0)
            {
                // To get speed in RPM
                // (Pulses / interrupt * 60s * QEI refresh freq) / (Pulses per tour)                
                QEI_struct[QEI_1].speed_rpm = (unsigned int)((QEI_struct[channel].new_pulse * 60 * QEI_struct[channel].refresh_freq) / (QEI_struct[channel].pulse_per_tour));
                QEI_reset_pulse(QEI_1);
            }
            else
            {
                QEI_struct[QEI_1].speed_rpm = 0;
            }
            break;     
    }
}

//********void QEI_interrupt_handle (unsigned char channel)*******************//
//Description : Function checks motor direction and actualizes QEI values
//
//Function prototype : void QEI_interrupt_handle (unsigned char channel)
//
//Enter params       : unsigned char channel : QEIx channel
//
//Exit params        : None
//
//Function call      : QEI_interrupt_handle(QEI1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void QEI_interrupt_handle (unsigned char channel)
{
    // Forward direction condition : QEI_A is high on low-to-high transition
    // on QEI_B.
    // QEI_A  ---___---___---___---___---___---
    // QEI_B ___---___---___---___---___---___---

    // Backward direction condition : QEI_A is low on low-to-high transition
    // on QEI_B.        
    // QEI_A  ___---___---___---___---___---___
    // QEI_B ___---___---___---___---___---___--- 
    
    if (channel == QEI_1)
    {
        // Forward
        if (QEI1A_PIN == 1)
        {
            QEI_struct[channel].direction = QEI_DIR_FORWARD;
        }   
        
        // Backward 
        else 
        {
            QEI_struct[channel].direction = QEI_DIR_BACKWARD;
        }            
    }
    
    QEI_struct[channel].pulse_for_tour++;
    QEI_struct[channel].pulse_cnter++;
    QEI_struct[channel].pulse_getter++;
    QEI_struct[channel].pulse_cnter_dist++;
    
    if (QEI_struct[channel].pulse_for_tour >= QEI_struct[channel].pulse_per_tour)
    {
        QEI_struct[channel].pulse_for_tour = 0;
        QEI_struct[channel].tour_cnter_dist++;
    }
    
    if (QEI_struct[channel].pulse_cnter >= QEI_struct[channel].pulse_per_tour)      //chaque tour parcouru, forward ou backward
    { 
        QEI_struct[channel].pulse_cnter = 0;
        QEI_struct[channel].tour_getter++;                    // Increm tour cnter
        QEI_struct[channel].tour_cnter++;
    }  
    
    if (QEI_struct[channel].tour_cnter >= MAX_TOUR_CNT){QEI_struct[channel].tour_cnter=0;}
    if (QEI_struct[channel].pulse_cnter >= MAX_PULSE_CNT){QEI_struct[channel].pulse_cnter = 0;}
    QEI_struct[channel].int_event = 1;    
}