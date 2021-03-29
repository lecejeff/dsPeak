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
    uint8_t channel;              // Physical CAN channel
    
    uint8_t mode;
    uint8_t old_mode;
    uint8_t int_state;
    
    uint32_t time_quantum_frequency;
    uint32_t bus_freq;
    
    uint8_t message_buffer_channel;
    uint16_t SID;
    uint16_t RX_MASK;
    uint16_t RX_SID;
    uint16_t EID;
    uint8_t SRR;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t RB1;
    uint8_t RB0;
    uint8_t DLC;
    uint8_t can_payload[8];
    uint16_t can_message[8];
}CAN_struct;

uint8_t CAN_init (CAN_struct *node);
uint8_t CAN_set_mode (CAN_struct *node, uint8_t mode);
uint8_t CAN_get_mode (CAN_struct *node);
uint16_t CAN_get_tx_err_cnt (CAN_struct *node);
uint16_t CAN_get_rx_err_cnt (CAN_struct *node);
uint8_t CAN_tx_msg (CAN_struct *node);

#endif	

