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
void QEI_init (unsigned char channel)
{
    switch (channel)
    {
        case QEI_MOT1:
            QEI1CONbits.QEIEN = 0;      // Disable QEI module counters
            IFS3bits.QEI1IF = 0;        // Clear interrupt flag
            
            TRISAbits.TRISA7 = 1;       // RA7 configured as an input (QEI_1B)
            TRISAbits.TRISA6 = 1;       // RA6 configured as an input (QEI_1A)
            RPINR14bits.QEB1R = 23;     // RA7 (RPI23) assigned to QEI_1B
            RPINR14bits.QEA1R = 22;     // RA6 (RPI22) assigned to QEI_1A
            
            QEI1CONbits.PIMOD = 5;      // Reset position counter when poscnt = QEI1GEC
            QEI1GECH = 0;               // Only use 32bit value for the Greather than or equal compare register
            QEI1GECL = QEI_MOT1_PPR;    // Set default pulse per revolution value
            QEI1STATbits.PCHEQIEN = 1;  // Enable position counter >= cmp interrupt enable
            QEI1IOCbits.FLTREN = 1;     // Enable digital input filter for QEI pins
            
            QEI_set_gear_derate(channel, QEI_MOT1_GDR);
            QEI_set_cpr(channel, QEI_MOT1_PPR);
            
            IEC3bits.QEI1IE = 1;        // Enable QEI1 interrupt
            QEI1CONbits.QEIEN = 1;      // Enable QEI module counters
            break;
            
        case QEI_MOT2:
            QEI2CONbits.QEIEN = 0;      // Disable QEI module counters
            IFS4bits.QEI2IF = 0;        // Clear interrupt flag
            
            TRISGbits.TRISG13 = 1;      // RG13 configured as an input (QEI_2B)
            TRISGbits.TRISG12 = 1;      // RG12 configured as an input (QEI_2A)
            RPINR16bits.QEB2R = 125;    // RG13 (RP125) assigned to QEI_2B
            RPINR16bits.QEA2R = 124;    // RG12 (RP124) assigned to QEI_2A   
            
            IEC4bits.QEI2IE = 1;        // Enable QEI2 interrupt
            break;
            
        case QEI_ROT_ENC:
            TRISAbits.TRISA15 = 1;      // RA15 configured as an input (QEI_ROT_ENC_B)
            TRISAbits.TRISA14 = 1;      // RA14 configured as an input (QEI_ROT_ENC_A)       
            CNENAbits.CNIEA15 = 1;      // Change notice interrupt enable on QEI_ROT_ENC_B pin
            CNENAbits.CNIEA14 = 1;      // Change notice interrupt enable on QEI_ROT_ENC_A pin            
            break;
            
        default:
            break;
            
    }
//    QEI_struct[channel].int_event = 0;          // Reset QEI vars on initialization
//    QEI_struct[channel].pulse_getter = 0;       
//    QEI_struct[channel].pulse_cnter_dist = 0;
//    QEI_struct[channel].direction = 0;          //
//    QEI_struct[channel].pulse_cnter = 0;        //
//    QEI_struct[channel].tour_cnter = 0;         //
//    QEI_struct[channel].tour_getter = 0;        //
//    QEI_struct[channel].pulse_for_tour = 0;
//    QEI_struct[channel].speed_rpm = 0;          //
//    
//    QEI_struct[channel].refresh_freq = refresh_freq;                // Default values
//    QEI_struct[channel].motor_gear_derate = GEAR_DERATE;            // Default values
//    QEI_struct[channel].motor_cpr = COUNT_PER_REVOLUTION;           // Default values
//    QEI_struct[channel].pulse_per_tour = QEI_struct[channel].motor_gear_derate * QEI_struct[channel].motor_cpr;
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
    unsigned long position = 0, hold = 0;
    switch (channel)
    {
        case QEI_MOT1:
            position = POS1CNTL;
            //hold = POS1HLD;
            //position |= (hold << 16);
            return position;
            break;
            
        case QEI_MOT2:
            position &= POS2CNTL;
            hold = POS2HLD;
            position |= (hold << 16);
            return position;
            break;
            
        default:
            return 0;
            break;
    }    
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
    switch (channel)
    {
        case QEI_MOT1:
            POS1HLD = 0;
            POS1CNTL = 0;
            break;
            
        case QEI_MOT2:
            POS2HLD = 0;
            POS2CNTL = 0;
            break;
            
        default:
            break;
    }
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
    switch (channel)
    {
        case QEI_MOT1:
            return QEI_struct[channel].speed_rpm;
            break;
                    
        case QEI_MOT2:
            return QEI_struct[channel].speed_rpm;
            break;
            
        case QEI_ROT_ENC:
            return 0;
            break;
            
        default:
            return 0;
            break;
    }
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
        case QEI_MOT1:
            // Update the actual speed
            QEI_struct[channel].new_pulse = VEL1CNT;
            if (QEI_struct[channel].new_pulse > 0)
            {
                // To get speed in RPM             
                QEI_struct[channel].speed_rpm = (unsigned int)((QEI_struct[channel].new_pulse * 60 * QEI_struct[channel].refresh_freq) / (QEI_struct[channel].pulse_per_tour));
            }
            else
            {
                QEI_struct[channel].speed_rpm = 0;
            }
            break;     
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _QEI1Interrupt(void)
{
    QEI_struct[QEI_MOT1].tour_getter++;                    // Increm tour cnter
    QEI_struct[QEI_MOT1].tour_cnter++;
    if (QEI_struct[QEI_MOT1].tour_cnter >= MAX_TOUR_CNT){QEI_struct[QEI_MOT1].tour_cnter=0;}
    if (QEI_struct[QEI_MOT1].tour_getter >= MAX_TOUR_CNT){QEI_struct[QEI_MOT1].tour_getter=0;}
    IFS3bits.QEI1IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _QEI2Interrupt(void)
{
    IFS4bits.QEI2IF = 0;
}