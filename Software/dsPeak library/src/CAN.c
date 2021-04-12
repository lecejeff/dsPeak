//***************************************************************************//
// File      :  can.c
//
// Functions :  
//
// Includes  :  #include "CAN.h"
//              #include "DMA.h"
//           
// Purpose   :  Driver for the dsPIC33E CAN peripheral
//              1x native channel on dsPeak
//              CAN_1 : Native CAN2.0B channel
//
// Intellitrol                   MPLab X v5.45                        06/04/2021
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "CAN.h"
#include "DMA.h"

__eds__ uint16_t CAN_MSG_BUFFER[32][8] __attribute__((eds, space(dma), aligned(32*16)));
uint16_t CAN_RX_MSG[8] = {0};

//void CAN_init_struct (CAN_struct *node, uint8_t channel, uint32_t bus_freq, uint16_t SID, uint16_t EID, uint16_t rx_mask, uint16_t rx_sid)//
//Description : Function initialize CAN structure parameters
//
//Function prototype : void CAN_init_struct (CAN_struct *node, uint8_t channel, uint32_t bus_freq, uint16_t SID, uint16_t EID, uint16_t rx_mask, uint16_t rx_sid)
//
//Enter params       : CAN_struct *node    : Pointer to a CAN_struct item
//                   : uint8_t channel     : CAN physical channel
//                   : uint32_t bus_freq   : CAN bus frequency, max is 550kbps (hardware constraint)
//                   : uint16_t SID        : CAN Standard identifier
//                   : uint16_t EID        : CAN extended identifier
//                   : uint16_t rx_mask    : CAN receive mask
//                   : uint16_t rx_sid     : CAN receive SID
//
//Exit params        : None
//
//Function call      : CAN_init_struct(&CAN_native, CAN_1, 500000, 0x0123, 0, 0x0300, 0x0300);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void CAN_init_struct (CAN_struct *node, uint8_t channel, uint32_t bus_freq, uint16_t SID,
                        uint16_t EID, uint16_t rx_mask, uint16_t rx_sid)
{
    uint8_t i = 0;
    node->bus_freq = bus_freq;
    node->channel = channel;
    node->SID = SID;
    node->EID = EID;
    node->RX_MASK = rx_mask;
    node->RX_SID = rx_sid;
    node->txmsg_buffer_channel = 0; // Use CAN MSGBUF0 for message transmission
    node->rxmsg_buffer_channel = 1; // Use CAN MSGBUF1 for message transmission
    
    // See if these should be configured by default
    node->SRR = 0;
    node->IDE = 0;    
    node->RTR = 0;
    node->RB1 = 0;
    node->RB0 = 0;
    node->DLC = 8;
    
    // Initialize payload bytes to 0 by default
    for (; i<8; i++)
    {
        node->can_payload[i] = 0;
    }
    
    // Set node CAN channel to CAN_MODE_CONFIG
    if (CAN_set_mode(node, CAN_MODE_CONFIG) == 0xFF)
    {
        // Error while trying to set CAN operating mode
    }
}

//********************uint8_t CAN_init (CAN_struct *node)*********************//
//Description : Function initialize CAN state machine and registers
//
//Function prototype : uint8_t CAN_init (CAN_struct *node)
//
//Enter params       : CAN_struct *node    : Pointer to a CAN_struct item
//
//Exit params        : uint8_t : CAN configuration status
//                               0 : Configuration successful
//                               1 : Error
//Function call      : CAN_init(&CAN_native);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t CAN_init (CAN_struct *node)
{
    uint8_t i = 0, j = 0;
   
    // Make sure node CAN physical channel is in config mode before initializing
    // the CAN registers, otherwise write to these registers will be discarded
    if (CAN_get_mode(node) != CAN_MODE_CONFIG){return 1;}    
    else
    {
        // Initialize CAN DMA message buffers to 0
        for (; i < 8; i++)
        {
            for (; j < 32; j++)
            {
                CAN_MSG_BUFFER[j][i] = 0;
            }
        }        
        switch (node->channel)
        {
            case CAN_1:
                IEC2bits.C1IE = 0;                  // Disable CAN interrupt during initialization
                IFS2bits.C1IF = 0;                  // Reset interrupt flag 
                C1CTRL1bits.WIN = 0;                // Set at 0 to access CAN control registers               
                C1FCTRLbits.DMABS = 6;              // 32 message buffers in device RAM
                // CAN channel physical pin configuration
                TRISGbits.TRISG15 = 0;              // RG15 configured as an output (CAN1_LBK)
                CAN_LBK_STATE = CAN_LBK_INACTIVE;   // Disable loopback during initialization
                TRISEbits.TRISE5 = 0;               // RE5 configured as an output (CAN1_TX)
                TRISEbits.TRISE6 = 1;               // RE6 configured as an input (CAN1_RX)
                RPOR6bits.RP85R = 0x0E;             // RE5 (RP85) assigned to CAN1_TX
                RPINR26bits.C1RXR = 86;             // RE6 (RPI86) assigned to CAN1_RX

                // CAN bus nominal bit time and time quanta
                C1CTRL1bits.CANCKS = 1;             // IMPLEMENTATION REVERSED, SEE ERRATA DS80000526H-page 8 -> Fcan = Fp (Fcy) = (FOSC / 2)
                C1CFG2bits.SAM = 1;                 // BUS sampled 3x times at the sample point
                // Make sure to limit the CAN bus frequency to 550kHz
                if (node->bus_freq > CAN_MAXIMUM_BUS_FREQ){return 1;}
                else
                {  
                    // Hard requirements for the CAN bus bit timing that must be followed :
                    // Bit time = Sync segment + Propagation segment + Phase segment 1 + Phase segment 2
                    // 1) Propagation segment + Phase segment 1 >=  Phase segment 2
                    // 2) Phase segment 2 >= Synchronous jump width (SJW)  
                    // If you want to change the bit timing, make sure to use 
                    // MPLAB X IDE Tools / Embedded / ECAN bit rate calculator
                    node->time_quantum_frequency = NOMINAL_TIME_QUANTA * node->bus_freq;
                    C1CFG1bits.BRP = (node->time_quantum_frequency / (2*node->bus_freq))+1;
                    C1CFG2bits.SEG1PH = PHASE_SEGMENT_1 - 1;        // Set phase segment 1 
                    C1CFG2bits.SEG2PHTS = 0;                        // Set SEG2 constrained to "Maximum of SEG1PHx bits"
                    C1CFG2bits.SEG2PH = PHASE_SEGMENT_2 - 1;        // Set phase segment 2
                    C1CFG2bits.PRSEG = PROPAGATION_SEGMENT - 1;     // Set propagation segment
                    C1CFG1bits.SJW = SYNCHRONIZATION_JUMP_WIDTH - 1;// Set SJW
                }
                // Use CAN TX buffer 0 for message transmission
                // Use CAN RX buffer 1 for message reception
                // DMA channel initialization, 1x channel for message transmission
                DMA_init(DMA_CH1);
                DMA1CON = DMA_SIZE_WORD | DMA_TXFER_WR_PER | DMA_AMODE_PIA | DMA_CHMODE_CPPD;
                DMA1REQ = DMAREQ_ECAN1TX;
                DMA1PAD = (volatile uint16_t)&C1TXD;
                DMA1STAH = __builtin_dmapage(CAN_MSG_BUFFER);
                DMA1STAL = __builtin_dmaoffset(CAN_MSG_BUFFER);                      
                DMA1CNT = 0x7;        
                
                // DMA channel initialization, 1x channel for message reception
                DMA_init(DMA_CH2);
                DMA2CON = DMA_SIZE_WORD | DMA_TXFER_RD_PER | DMA_AMODE_PIA | DMA_CHMODE_CPPD;
                DMA2REQ = DMAREQ_ECAN1RX;
                DMA2PAD = (volatile uint16_t)&C1RXD;
                DMA2STAH = __builtin_dmapage(CAN_MSG_BUFFER);
                DMA2STAL = __builtin_dmaoffset(CAN_MSG_BUFFER);                      
                DMA2CNT = 0x7;                 
                C1CTRL1bits.WIN = 1;                // Set at 1 to access CAN filter / mask registers                       
                C1RXF1SIDbits.SID = node->RX_SID;   // Setup acceptance filter 1 SID
                C1RXM1SIDbits.SID = node->RX_MASK;  // Setup acceptance mask 1
                C1FMSKSEL1bits.F1MSK = 1;           // Acceptance mask 1 registers contain mask
                C1BUFPNT1bits.F1BP = 1;             // Acceptance filter 1 will use message buffer 1
                C1FEN1bits.FLTEN1 = 1;              // Acceptance filter 1 enabled 
                
                C1CTRL1bits.WIN = 0;                // Set at 0 to access CAN control registers   
                
                C1TR01CONbits.TXEN0 = 1;            // Set buffer 0 to transmit buffer
                C1TR01CONbits.RTREN1 = 1;           // Set buffer 1 to receive buffer
                C1TR01CONbits.TX0PRI = 0x3; 

                // Enable CAN interrupts
                IEC2bits.C1IE = 1;
                C1INTEbits.TBIE = 1;
                C1INTEbits.RBIE = 1;
               
                DMA_enable(DMA_CH1);                // Enable DMA channel and interrupt  
                DMA_enable(DMA_CH2);                // Enable DMA channel and interrupt 
                return 0;
                break;

            default:
                return 1;                           // Return error 
                break;
        }
    }
}

void CAN_fill_payload (CAN_struct *node, uint8_t *buf, uint8_t length)
{
    uint8_t i = 0;
    for (; i < length; i++)
    {
        node->can_payload[i] = *buf++;
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
        
        default:
            return 0xFF;
            break;
    }
    // Mode change occurs only when the bus is idle (11 successive recessive bits)
    // The following blocks the program execution until mode change is done
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
            
        default:
            return 0xFF;
            break;
    }
}

uint8_t CAN_send_txmsg (CAN_struct *node)
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

uint16_t * CAN_parse_rxmsg (CAN_struct *node)
{
    uint8_t can_rx_payload[8] = {0};
    if ((CAN_MSG_BUFFER[node->rxmsg_buffer_channel][0] & 0x1FFC)>>2 == node->RX_SID)
    {
        CAN_RX_MSG[0] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][0];
        CAN_RX_MSG[1] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][1];
        CAN_RX_MSG[2] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][2];
        CAN_RX_MSG[3] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][3];
        CAN_RX_MSG[4] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][4];
        CAN_RX_MSG[5] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][5];
        CAN_RX_MSG[6] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][6];
        CAN_RX_MSG[7] = CAN_MSG_BUFFER[node->rxmsg_buffer_channel][7];
        can_rx_payload[0] = CAN_RX_MSG[3]&0x00FF;
        can_rx_payload[1] = ((CAN_RX_MSG[3]&0xFF00)>>8);
        can_rx_payload[2] = CAN_RX_MSG[4]&0x00FF;
        can_rx_payload[3] = ((CAN_RX_MSG[4]&0xFF00)>>8);
        can_rx_payload[4] = CAN_RX_MSG[5]&0x00FF;
        can_rx_payload[5] = ((CAN_RX_MSG[5]&0xFF00)>>8);
        can_rx_payload[6] = CAN_RX_MSG[6]&0x00FF;
        can_rx_payload[7] = ((CAN_RX_MSG[6]&0xFF00)>>8);        
        return &CAN_RX_MSG[0];
    }
    else
        return 0;
}

uint16_t CAN_get_txmsg_errcnt (CAN_struct *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1ECbits.TERRCNT;
            break;
            
        default:
            return 0;
            break;
    }
}

uint16_t CAN_get_rxmsg_errcnt (CAN_struct *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1ECbits.RERRCNT;
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
