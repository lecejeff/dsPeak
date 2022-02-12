#include "DMA.h"
STRUCT_DMA DMA_struct[DMA_QTY];

// DMA channel usage
// DMA_CH0 -> UART1_TX
// DMA_CH1 -> UART2_TX
// DMA_CH2 -> CAN1_RX
// DMA_CH3 -> CAN1_TX
// DMA_CH4 -> SPI1_RX FTDI EVE
// DMA_CH5 -> UART3 TX debug port
// DMA_CH6 -> SPI1_TX FTDI EVE

void DMA_struct_init (uint8_t channel)
{   
    if ((channel >= 0) && (channel < DMA_QTY))
    {
        DMA_struct[channel].state = DMA_STATE_UNASSIGNED;
        DMA_struct[channel].txfer_state = DMA_TXFER_DONE; 
    }
    
    if (channel == DMA_ALL_INIT)
    {
        uint8_t i = 0;
        for (; i < DMA_QTY; i++)
        {
            DMA_struct[i].state = DMA_STATE_UNASSIGNED;
            DMA_struct[i].txfer_state = DMA_TXFER_DONE;             
        }
    }
}

void DMA_init (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].state = DMA_STATE_ASSIGNED;
        DMA_struct[channel].txfer_state = DMA_TXFER_DONE; 
    }
    switch (channel)
    {     
        case DMA_CH0:              
            IEC0bits.DMA0IE = 0;                    // Disable DMA0 interrupt
            IFS0bits.DMA0IF = 0;                    // Lower DMA0 interrupt flag           
            break;

        case DMA_CH1:

            IEC0bits.DMA1IE = 0;
            IFS0bits.DMA1IF = 0;
            break;  

        case DMA_CH2:
            IEC1bits.DMA2IE = 0;
            IFS1bits.DMA2IF = 0;
            break; 

        case DMA_CH3:
            IEC2bits.DMA3IE = 0;
            IFS2bits.DMA3IF = 0;
            break; 

        case DMA_CH4:
            IEC2bits.DMA4IE = 0;
            IFS2bits.DMA4IF = 0;
            break;

        case DMA_CH5:
            IEC3bits.DMA5IE = 0;
            IFS3bits.DMA5IF = 0;
            break;   

        case DMA_CH6:
            IEC4bits.DMA6IE = 0;
            IFS4bits.DMA6IF = 0;
            break;      

        case DMA_CH7:
            IEC4bits.DMA7IE = 0;
            IFS4bits.DMA7IF = 0;
            break;   

        case DMA_CH8:
            IEC7bits.DMA8IE = 0;
            IFS7bits.DMA8IF = 0;
            break; 

        case DMA_CH9:
            IEC7bits.DMA9IE = 0;
            IFS7bits.DMA9IF = 0;                
            break;    

        case DMA_CH10:
            IEC7bits.DMA10IE = 0;
            IFS7bits.DMA10IF = 0;                
            break;       

        case DMA_CH11:
            IEC7bits.DMA11IE = 0;
            IFS7bits.DMA11IF = 0;                
            break;

        case DMA_CH12:
            IEC8bits.DMA12IE = 0;
            IFS8bits.DMA12IF = 0;
            break;   

        case DMA_CH13:
            IEC8bits.DMA13IE = 0;
            IFS8bits.DMA13IF = 0;                
            break;

        case DMA_CH14:
            IEC8bits.DMA14IE = 0;
            IFS8bits.DMA14IF = 0;                
            break;            
    }
}

void DMA_disable (uint8_t channel)
{
    switch (channel)
    {
        case DMA_CH0:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA0CONbits.CHEN = 0;
            IEC0bits.DMA0IE = 0;                    // Disable DMA0 interrupt
            IFS0bits.DMA0IF = 0;                    // Lower DMA0 interrupt flag            
            break;
            
        case DMA_CH1:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA1CONbits.CHEN = 0;
            IEC0bits.DMA1IE = 0;                
            IFS0bits.DMA1IF = 0;                
            break;

        case DMA_CH2:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA2CONbits.CHEN = 0;
            IEC1bits.DMA2IE = 0;                
            IFS1bits.DMA2IF = 0;             
            break;

        case DMA_CH3:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA3CONbits.CHEN = 0;
            IEC2bits.DMA3IE = 0;
            IFS2bits.DMA3IF = 0;            
            break;

        case DMA_CH4:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA4CONbits.CHEN = 0;
            IEC2bits.DMA4IE = 0;
            IFS2bits.DMA4IF = 0;            
            break;

        case DMA_CH5:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA5CONbits.CHEN = 0;
            IEC3bits.DMA5IE = 0;
            IFS3bits.DMA5IF = 0;
            break;

        case DMA_CH6:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA6CONbits.CHEN = 0;
            IEC4bits.DMA6IE = 0;
            IFS4bits.DMA6IF = 0;
            break;

        case DMA_CH7:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA7CONbits.CHEN = 0;
            IEC4bits.DMA7IE = 0;
            IFS4bits.DMA7IF = 0;
            break;

        case DMA_CH8:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA8CONbits.CHEN = 0;
            IEC7bits.DMA8IE = 0;
            IFS7bits.DMA8IF = 0;
            break;

        case DMA_CH9:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA9CONbits.CHEN = 0;
            IEC7bits.DMA9IE = 0;
            IFS7bits.DMA9IF = 0;
            break;

        case DMA_CH10:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA10CONbits.CHEN = 0;
            IEC7bits.DMA10IE = 0;
            IFS7bits.DMA10IF = 0;
            break;

        case DMA_CH11:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA11CONbits.CHEN = 0;
            IEC7bits.DMA11IE = 0;
            IFS7bits.DMA11IF = 0;
            break;

        case DMA_CH12:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA12CONbits.CHEN = 0;
            IEC8bits.DMA12IE = 0;
            IFS8bits.DMA12IF = 0;
            break;

        case DMA_CH13:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA13CONbits.CHEN = 0;
            IEC8bits.DMA13IE = 0;
            IFS8bits.DMA13IF = 0;
            break;

        case DMA_CH14:
            DMA_struct[channel].state = DMA_STATE_DISABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
            DMA14CONbits.CHEN = 0;
            IEC8bits.DMA14IE = 0;
            IFS8bits.DMA14IF = 0;            
            break;     
            
        default:
            break;
    }
}

void DMA_enable (uint8_t channel)
{
    switch (channel)
    {
        case DMA_CH0:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA0CONbits.CHEN = 1;
            IFS0bits.DMA0IF = 0;                    // Lower DMA interrupt flag 
            IEC0bits.DMA0IE = 1;                    // Enable DMA interrupt                       
            break;
            
        case DMA_CH1:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA1CONbits.CHEN = 1;
            IEC0bits.DMA1IE = 1;                
            IFS0bits.DMA1IF = 0;                
            break;

        case DMA_CH2:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA2CONbits.CHEN = 1;
            IEC1bits.DMA2IE = 1;                
            IFS1bits.DMA2IF = 0;             
            break;

        case DMA_CH3:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA3CONbits.CHEN = 1;
            IEC2bits.DMA3IE = 1;
            IFS2bits.DMA3IF = 0;            
            break;

        case DMA_CH4:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA4CONbits.CHEN = 1;
            IEC2bits.DMA4IE = 1;
            IFS2bits.DMA4IF = 0;            
            break;

        case DMA_CH5:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA5CONbits.CHEN = 1;
            IEC3bits.DMA5IE = 1;
            IFS3bits.DMA5IF = 0;
            break;

        case DMA_CH6:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA6CONbits.CHEN = 1;
            IEC4bits.DMA6IE = 1;
            IFS4bits.DMA6IF = 0;
            break;

        case DMA_CH7:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA7CONbits.CHEN = 1;
            IEC4bits.DMA7IE = 1;
            IFS4bits.DMA7IF = 0;
            break;

        case DMA_CH8:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA8CONbits.CHEN = 1;
            IEC7bits.DMA8IE = 1;
            IFS7bits.DMA8IF = 0;
            break;

        case DMA_CH9:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA9CONbits.CHEN = 1;
            IEC7bits.DMA9IE = 1;
            IFS7bits.DMA9IF = 0;
            break;

        case DMA_CH10:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA10CONbits.CHEN = 1;
            IEC7bits.DMA10IE = 1;
            IFS7bits.DMA10IF = 0;
            break;

        case DMA_CH11:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA11CONbits.CHEN = 1;
            IEC7bits.DMA11IE = 1;
            IFS7bits.DMA11IF = 0;
            break;

        case DMA_CH12:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA12CONbits.CHEN = 1;
            IEC8bits.DMA12IE = 1;
            IFS8bits.DMA12IF = 0;
            break;

        case DMA_CH13:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA13CONbits.CHEN = 1;
            IEC8bits.DMA13IE = 1;
            IFS8bits.DMA13IF = 0;
            break;

        case DMA_CH14:
            DMA_struct[channel].state = DMA_STATE_ENABLED;
            DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
            DMA14CONbits.CHEN = 1;
            IEC8bits.DMA14IE = 1;
            IFS8bits.DMA14IF = 0;            
            break;     
            
        default:
            break;
    }
}

void DMA_force_txfer (uint8_t channel)
{
    switch (channel)
    {
        case DMA_CH0:
            DMA0REQbits.FORCE = 1;                     
            break;
            
        case DMA_CH1:
            DMA1REQbits.FORCE = 1;               
            break;

        case DMA_CH2:
            DMA2REQbits.FORCE = 1;            
            break;

        case DMA_CH3:
            DMA3REQbits.FORCE = 1;           
            break;

        case DMA_CH4:
            DMA4REQbits.FORCE = 1;            
            break;

        case DMA_CH5:
            DMA5REQbits.FORCE = 1;
            break;

        case DMA_CH6:
            DMA6REQbits.FORCE = 1;
            break;

        case DMA_CH7:
            DMA7REQbits.FORCE = 1;
            break;

        case DMA_CH8:
            DMA8REQbits.FORCE = 1;
            break;

        case DMA_CH9:
            DMA9REQbits.FORCE = 1;
            break;

        case DMA_CH10:
            DMA10REQbits.FORCE = 1;
            break;

        case DMA_CH11:
            DMA11REQbits.FORCE = 1;
            break;

        case DMA_CH12:
            DMA12REQbits.FORCE = 1;
            break;

        case DMA_CH13:
            DMA13REQbits.FORCE = 1;
            break;

        case DMA_CH14:
            DMA14REQbits.FORCE = 1;            
            break;     
            
        default:
            break;
    }    
}

void DMA_set_txfer_length(uint8_t channel, uint16_t length)
{
    // Saturate DMA tx length
    if (length > DMA_MAX_TX_LENGTH)
    {
        length = DMA_MAX_TX_LENGTH;
    }
    
    switch (channel)
    {
        case DMA_CH0:
            DMA0CNT = length;                    
            break;
            
        case DMA_CH1:
            DMA1CNT = length;                 
            break;

        case DMA_CH2:
            DMA2CNT = length;             
            break;

        case DMA_CH3:
            DMA3CNT = length;            
            break;

        case DMA_CH4:
            DMA4CNT = length;             
            break;

        case DMA_CH5:
            DMA5CNT = length;   
            break;

        case DMA_CH6:
            DMA6CNT = length;   
            break;

        case DMA_CH7:
            DMA7CNT = length;   
            break;

        case DMA_CH8:
            DMA8CNT = length;   
            break;

        case DMA_CH9:
            DMA9CNT = length;   
            break;

        case DMA_CH10:
            DMA10CNT = length;   
            break;

        case DMA_CH11:
            DMA11CNT = length;   
            break;

        case DMA_CH12:
            DMA12CNT = length;   
            break;

        case DMA_CH13:
            DMA13CNT = length;   
            break;

        case DMA_CH14:
            DMA14CNT = length;             
            break;     
            
        default:
            break;
    }
}

uint8_t DMA_get_txfer_state (uint8_t channel)
{
    if (DMA_struct[channel].txfer_state == DMA_TXFER_DONE)
    {
        return DMA_TXFER_DONE;
    }
    else
        return DMA_TXFER_IN_PROGRESS;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;
    DMA_struct[DMA_CH0].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;
#ifdef RS485_CLICK_UART2    
    while(!U2STAbits.TRMT);
    // Put the transceiver in receive mode
    LATDbits.LATD0 = 0;     // DE = 0
    LATHbits.LATH15 = 0;    // RE = 0    
#endif
    DMA_struct[DMA_CH1].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA2Interrupt(void)
{
    IFS1bits.DMA2IF = 0;
    DMA_struct[DMA_CH2].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA3Interrupt(void)
{
    IFS2bits.DMA3IF = 0;
    DMA_struct[DMA_CH3].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA4Interrupt(void)
{
    IFS2bits.DMA4IF = 0;
    DMA_struct[DMA_CH4].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA5Interrupt(void)
{
    IFS3bits.DMA5IF = 0;
    DMA_struct[DMA_CH5].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA6Interrupt(void)
{
    IFS4bits.DMA6IF = 0;
    DMA_struct[DMA_CH6].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA7Interrupt(void)
{
    IFS4bits.DMA7IF = 0;
    DMA_struct[DMA_CH7].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA8Interrupt(void)
{
    IFS7bits.DMA8IF = 0;
    DMA_struct[DMA_CH8].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA9Interrupt(void)
{
    IFS7bits.DMA9IF = 0;
    DMA_struct[DMA_CH9].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA10Interrupt(void)
{
    IFS7bits.DMA10IF = 0;
    DMA_struct[DMA_CH10].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA11Interrupt(void)
{
    IFS7bits.DMA11IF = 0;
    DMA_struct[DMA_CH11].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA12Interrupt(void)
{
    IFS8bits.DMA12IF = 0;
    DMA_struct[DMA_CH12].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA13Interrupt(void)
{
    IFS8bits.DMA13IF = 0;
    DMA_struct[DMA_CH13].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA14Interrupt(void)
{
    IFS8bits.DMA14IF = 0;
    DMA_struct[DMA_CH14].txfer_state = DMA_TXFER_DONE;
}
