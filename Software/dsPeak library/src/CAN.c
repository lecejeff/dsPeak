#include "CAN.h"
#include "DMA.h"

__eds__ uint16_t CAN_MSG_BUFFER[32][8] __attribute__((eds, space(dma), aligned(32*16)));

// Return 0 on successful configuration
// Return 1 on error
uint8_t CAN_init (CAN_struct *node)
{
    uint8_t i = 0;
    for (; i < 8; i++)
    {
        CAN_MSG_BUFFER[0][i] = 0;
        //node->can_message[i] = 0;   // Initialize can_message to all 0
    }
    //--------- Module always resets in the CAN configuration mode -----------//
    node->old_mode = CAN_MODE_CONFIG;   //
    node->mode = CAN_MODE_CONFIG;       //
    
    switch (node->channel)
    {
        case CAN_1:
            IEC2bits.C1IE = 0;                  // Disable CAN interrupt during initialization
            IFS2bits.C1IF = 0;                  // Reset interrupt flag 
            
            CAN_set_mode(node, CAN_MODE_CONFIG);// Set CAN module to configuration 
            C1CTRL1bits.WIN = 0;                
            
            TRISGbits.TRISG15 = 0;              // RG15 configured as an output (CAN1_LBK)
            CAN_LBK_STATE = CAN_LBK_INACTIVE;   // Disable loopback during initialization
            TRISEbits.TRISE5 = 0;               // RE5 configured as an output (CAN1_TX)
            TRISEbits.TRISE6 = 1;               // RE6 configured as an input (CAN1_RX)
            RPOR6bits.RP85R = 0x0E;             // RE5 (RP85) assigned to CAN1_TX
            RPINR26bits.C1RXR = 86;             // RE6 (RPI86) assigned to CAN1_RX
      
            // CAN bus nominal bit time and time quanta
            C1CTRL1bits.CANCKS = 1;             // IMPLEMENTATION REVERSED, SEE ERRATA DS80000526H-page 8 -> Fcan = Fp (Fcy) = (FOSC / 2)
            C1CFG2bits.SAM = 1;                 // BUS sampled 3x times at the sample point
            
            if (node->bus_freq > CAN_MAXIMUM_BUS_FREQ)
            {
                // Return error because requested bus frequency is higher than
                // the hardware limit of the platform
                return 1;
            }
            else
            {   
                node->time_quantum_frequency = NOMINAL_TIME_QUANTA * node->bus_freq;
                C1CFG1bits.BRP = (node->time_quantum_frequency / (2*node->bus_freq))+1;                         // For maximum resolution
                //C1CFG1bits.BRP = 6;
                // Hard requirements for the CAN bus bit timing that must be followed :
                // Bit time = Sync segment + Propagation segment + Phase segment 1 + Phase segment 2
                // 1) Propagation segment + Phase segment 1 >=  Phase segment 2
                // 2) Phase segment 2 >= Synchronous jump width (SJW)
                C1CFG2bits.SEG1PH = PHASE_SEGMENT_1 - 1;        // Set phase segment 1 
                C1CFG2bits.SEG2PHTS = 0;                        // Set SEG2 constrained to "Maximum of SEG1PHx bits"
                C1CFG2bits.SEG2PH = PHASE_SEGMENT_2 - 1;        // Set phase segment 2
                C1CFG2bits.PRSEG = PROPAGATION_SEGMENT - 1;         // Set propagation segment
                C1CFG1bits.SJW = SYNCHRONIZATION_JUMP_WIDTH - 1;// Set SJW
            }
            
            DMA_init(DMA_CH1);
            DMA1CON = DMA_SIZE_WORD | DMA_TXFER_WR_PER | DMA_AMODE_PIA | DMA_CHMODE_CPPD;
            DMA1REQ = DMAREQ_ECAN1TX;
            DMA1PAD = (volatile uint16_t)&C1TXD;
            DMA1STAH = __builtin_dmapage(CAN_MSG_BUFFER);
            DMA1STAL = __builtin_dmaoffset(CAN_MSG_BUFFER);                      
            DMA1CNT = 0x7;        // 8 word transfers 
            
            // As a first test, use buffer 0 to transmit message, buffer 1 to receive message
            C1TR01CONbits.TXEN0 = 1;            // Set buffer 0 to transmit buffer
            C1TR01CONbits.TX0PRI = 0x3; 
            
            // Enable CAN interrupts
            IEC2bits.C1IE = 1;
            C1INTEbits.TBIE = 1;
            //C1INTEbits.RBIE = 1;
            
            // Enable DMA channel and interrupt
            DMA_enable(DMA_CH1);  
            return 0;
            break;
            
        case CAN_2:
            IEC3bits.C2IE = 0;                  // Disable CAN interrupt during initialization
            IFS3bits.C2IF = 0;                  // Reset interrupt flag      
            return 0;                           
            break;
            
        default:
            return 1;                           // Return error 
            break;
    }
}

// Returns 0 on successful execution
// Returns 0xFF on error
uint8_t CAN_set_mode (CAN_struct *node, uint8_t mode)
{
    node->old_mode = node->mode;
    node->mode = mode;  
    switch (node->channel)
    {
        case CAN_1:
            C1CTRL1bits.REQOP = mode;
            break;

        case CAN_2:
            C2CTRL1bits.REQOP = mode;
            break;
        
        default:
            return 0xFF;
            break;
    }
    while(CAN_get_mode(node) != node->mode);
    return node->mode;
}

// Returns 0,1,2,3,4 or 7 in case of successful operation
// Returns 0xFF on error
uint8_t CAN_get_mode (CAN_struct *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1CTRL1bits.OPMODE;
            break;
            
        case CAN_2:
            return C2CTRL1bits.OPMODE;
            break;
            
        default:
            return 0xFF;
            break;
    }
}

uint8_t CAN_tx_msg (CAN_struct *node)
{
    if (CAN_get_mode(node)!=CAN_MODE_NORMAL)
    {
        // CAN bus is not in normal mode, cannot transfer message
        return 1;
    }
    CAN_MSG_BUFFER[0][0] = ((node->SID << 2) | (node->SRR << 1) | node->IDE);
    CAN_MSG_BUFFER[0][1] = node->EID & 0x3FFC0;
    CAN_MSG_BUFFER[0][2] = (((node->EID & 0x0003F) << 10) | (node->RTR << 9)
                            | (node->RB1 << 8) | (node->RB0 << 4) | (node->DLC));
    CAN_MSG_BUFFER[0][3] = (uint16_t)((node->can_payload[1]<<8) | node->can_payload[0]);
    CAN_MSG_BUFFER[0][4] = (uint16_t)((node->can_payload[3]<<8) | node->can_payload[2]);
    CAN_MSG_BUFFER[0][5] = (uint16_t)((node->can_payload[5]<<8) | node->can_payload[4]);
    CAN_MSG_BUFFER[0][6] = (uint16_t)((node->can_payload[7]<<8) | node->can_payload[6]);
    CAN_MSG_BUFFER[0][7] = 0;
                 
    C1TR01CONbits.TXREQ0 = 0x1; 
    while (C1TR01CONbits.TXREQ0 == 1);
    return 0;
}

uint16_t CAN_get_tx_err_cnt (CAN_struct *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1ECbits.TERRCNT;
            break;
            
        case CAN_2:
            return C2ECbits.TERRCNT;
            break;
            
        default:
            return 0;
            break;
    }
}

uint16_t CAN_get_rx_err_cnt (CAN_struct *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1ECbits.RERRCNT;
            break;
            
        case CAN_2:
            return C2ECbits.RERRCNT;
            break;
            
        default:
            return 0;
            break;
    }    
}

// ECAN1 event interrupt
void __attribute__((__interrupt__, no_auto_psv))_C1Interrupt(void)
{
    IFS2bits.C1IF = 0;      // clear interrupt flag
    if( C1INTFbits.TBIF )
    {
        C1INTFbits.TBIF = 0;
    }

    if( C1INTFbits.RBIF )
    {
        C1INTFbits.RBIF = 0;
    }
}

//// ECAN1 Receive data ready interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C1RxRdyInterrupt(void)
//{
//    
//}
//
// ECAN1 Transmit data request interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C1TxReqInterrupt(void)
//{
//}

// ECAN2 event interrupt
void __attribute__((__interrupt__, no_auto_psv))_C2Interrupt(void)
{
    IFS3bits.C2IF = 0;
}

//// ECAN2 Receive data ready interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C2RxRdyInterrupt(void)
//{
//    
//}
//
//// ECAN2 Transmit data request interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C2TxReqInterrupt(void)
//{
//    
//}
