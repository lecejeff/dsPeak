#ifndef __CAN_H_
#define	__CAN_H_

#include "general.h"

// CAN channel definitions for dsPIC33E
#define CAN_1               0
#define CAN_2               1

#define CAN_LBK_ACTIVE      1
#define CAN_LBK_INACTIVE    0
#define CAN_LBK_STATE   LATGbits.LATG15

// Define CAN operating mode
#define CAN_MODE_LISTEN_ALL     7
#define CAN_MODE_CONFIG         4
#define CAN_MODE_LISTEN_ONLY    3
#define CAN_MODE_LOOPBACK_INT   2
#define CAN_MODE_DISABLE        1
#define CAN_MODE_NORMAL         0

// Define system Time quanta and nominal bit time
#define NOMINAL_TIME_QUANTA         10  // Between 8 and 25
#define PHASE_SEGMENT_1             4   //     
#define PHASE_SEGMENT_2             4   //  
#define PROPAGATION_SEGMENT         1   //
#define SYNCHRONIZATION_JUMP_WIDTH  1   //

#define NUM_OF_CAN_BUFFERS 32

#define CAN_MAXIMUM_BUS_FREQ    550000UL

typedef struct
{
    unsigned char channel;              // Physical CAN channel
    
    unsigned char mode;
    unsigned char old_mode;
    unsigned char int_state;
    
    unsigned long time_quantum_frequency;
    unsigned long bus_freq;
    
    unsigned char message_buffer_channel;
    unsigned int SID;
    unsigned int RX_MASK;
    unsigned int RX_SID;
    unsigned int EID;
    unsigned char SRR;
    unsigned char IDE;
    unsigned char RTR;
    unsigned char RB1;
    unsigned char RB0;
    unsigned char DLC;
    unsigned char can_payload[8];
    unsigned int can_message[8];
}CAN_struct;

unsigned char CAN_init (CAN_struct *node);
unsigned char CAN_set_mode (CAN_struct *node, unsigned char mode);
unsigned char CAN_get_mode (CAN_struct *node);
unsigned int CAN_get_tx_err_cnt (CAN_struct *node);
unsigned int CAN_get_rx_err_cnt (CAN_struct *node);
unsigned char CAN_tx_msg (CAN_struct *node);

#endif	

