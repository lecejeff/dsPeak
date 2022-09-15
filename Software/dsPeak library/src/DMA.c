#include "DMA.h"
STRUCT_DMA DMA_struct[DMA_QTY];

void DMA_init (uint8_t channel)
{ 
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].state = DMA_STATE_ASSIGNED;
        DMA_struct[channel].txfer_state = DMA_TXFER_DONE; 

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
                
            default:
                break;
        }
    }
}

void DMA_disable (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].state = DMA_STATE_DISABLED;
        DMA_struct[channel].txfer_state = DMA_TXFER_DONE;
        switch (channel)
        {
            case DMA_CH0:
                DMA0CONbits.CHEN = 0;
                IEC0bits.DMA0IE = 0;                    // Disable DMA0 interrupt
                IFS0bits.DMA0IF = 0;                    // Lower DMA0 interrupt flag            
                break;

            case DMA_CH1:
                DMA1CONbits.CHEN = 0;
                IEC0bits.DMA1IE = 0;                
                IFS0bits.DMA1IF = 0;                
                break;

            case DMA_CH2:
                DMA2CONbits.CHEN = 0;
                IEC1bits.DMA2IE = 0;                
                IFS1bits.DMA2IF = 0;             
                break;

            case DMA_CH3:
                DMA3CONbits.CHEN = 0;
                IEC2bits.DMA3IE = 0;
                IFS2bits.DMA3IF = 0;            
                break;

            case DMA_CH4:
                DMA4CONbits.CHEN = 0;
                IEC2bits.DMA4IE = 0;
                IFS2bits.DMA4IF = 0;            
                break;

            case DMA_CH5:
                DMA5CONbits.CHEN = 0;
                IEC3bits.DMA5IE = 0;
                IFS3bits.DMA5IF = 0;
                break;

            case DMA_CH6:
                DMA6CONbits.CHEN = 0;
                IEC4bits.DMA6IE = 0;
                IFS4bits.DMA6IF = 0;
                break;

            case DMA_CH7:
                DMA7CONbits.CHEN = 0;
                IEC4bits.DMA7IE = 0;
                IFS4bits.DMA7IF = 0;
                break;

            case DMA_CH8:
                DMA8CONbits.CHEN = 0;
                IEC7bits.DMA8IE = 0;
                IFS7bits.DMA8IF = 0;
                break;

            case DMA_CH9:
                DMA9CONbits.CHEN = 0;
                IEC7bits.DMA9IE = 0;
                IFS7bits.DMA9IF = 0;
                break;

            case DMA_CH10:
                DMA10CONbits.CHEN = 0;
                IEC7bits.DMA10IE = 0;
                IFS7bits.DMA10IF = 0;
                break;

            case DMA_CH11:
                DMA11CONbits.CHEN = 0;
                IEC7bits.DMA11IE = 0;
                IFS7bits.DMA11IF = 0;
                break;

            case DMA_CH12:
                DMA12CONbits.CHEN = 0;
                IEC8bits.DMA12IE = 0;
                IFS8bits.DMA12IF = 0;
                break;

            case DMA_CH13:
                DMA13CONbits.CHEN = 0;
                IEC8bits.DMA13IE = 0;
                IFS8bits.DMA13IF = 0;
                break;

            case DMA_CH14:
                DMA14CONbits.CHEN = 0;
                IEC8bits.DMA14IE = 0;
                IFS8bits.DMA14IF = 0;            
                break;     

            default:
                break;
        }
    }
}

void DMA_enable (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].state = DMA_STATE_ENABLED;
        DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;
        switch (channel)
        {
            case DMA_CH0:
                DMA0CONbits.CHEN = 1;
                IFS0bits.DMA0IF = 0;                    // Lower DMA interrupt flag 
                IEC0bits.DMA0IE = 1;                    // Enable DMA interrupt                       
                break;

            case DMA_CH1:
                DMA1CONbits.CHEN = 1;
                IEC0bits.DMA1IE = 1;                
                IFS0bits.DMA1IF = 0;                
                break;

            case DMA_CH2:
                DMA2CONbits.CHEN = 1;
                IEC1bits.DMA2IE = 1;                
                IFS1bits.DMA2IF = 0;             
                break;

            case DMA_CH3:
                DMA3CONbits.CHEN = 1;
                IEC2bits.DMA3IE = 1;
                IFS2bits.DMA3IF = 0;            
                break;

            case DMA_CH4:
                DMA4CONbits.CHEN = 1;
                IEC2bits.DMA4IE = 1;
                IFS2bits.DMA4IF = 0;            
                break;

            case DMA_CH5:
                DMA5CONbits.CHEN = 1;
                IEC3bits.DMA5IE = 1;
                IFS3bits.DMA5IF = 0;
                break;

            case DMA_CH6:
                DMA6CONbits.CHEN = 1;
                IEC4bits.DMA6IE = 1;
                IFS4bits.DMA6IF = 0;
                break;

            case DMA_CH7:
                DMA7CONbits.CHEN = 1;
                IEC4bits.DMA7IE = 1;
                IFS4bits.DMA7IF = 0;
                break;

            case DMA_CH8:
                DMA8CONbits.CHEN = 1;
                IEC7bits.DMA8IE = 1;
                IFS7bits.DMA8IF = 0;
                break;

            case DMA_CH9:
                DMA9CONbits.CHEN = 1;
                IEC7bits.DMA9IE = 1;
                IFS7bits.DMA9IF = 0;
                break;

            case DMA_CH10:
                DMA10CONbits.CHEN = 1;
                IEC7bits.DMA10IE = 1;
                IFS7bits.DMA10IF = 0;
                break;

            case DMA_CH11:
                DMA11CONbits.CHEN = 1;
                IEC7bits.DMA11IE = 1;
                IFS7bits.DMA11IF = 0;
                break;

            case DMA_CH12:
                DMA12CONbits.CHEN = 1;
                IEC8bits.DMA12IE = 1;
                IFS8bits.DMA12IF = 0;
                break;

            case DMA_CH13:
                DMA13CONbits.CHEN = 1;
                IEC8bits.DMA13IE = 1;
                IFS8bits.DMA13IF = 0;
                break;

            case DMA_CH14:
                DMA14CONbits.CHEN = 1;
                IEC8bits.DMA14IE = 1;
                IFS8bits.DMA14IF = 0;            
                break;     

            default:
                break;
        }
    }
}

void DMA_force_txfer (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
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
}

uint8_t DMA_get_force_state (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {        
        switch (channel)
        {
            case DMA_CH0:
                return DMA0REQbits.FORCE;                     
                break;

            case DMA_CH1:
                return DMA1REQbits.FORCE;               
                break;

            case DMA_CH2:
                return DMA2REQbits.FORCE;            
                break;

            case DMA_CH3:
                return DMA3REQbits.FORCE;           
                break;

            case DMA_CH4:
                return DMA4REQbits.FORCE;            
                break;

            case DMA_CH5:
                return DMA5REQbits.FORCE;
                break;

            case DMA_CH6:
                return DMA6REQbits.FORCE;
                break;

            case DMA_CH7:
                return DMA7REQbits.FORCE;
                break;

            case DMA_CH8:
                return DMA8REQbits.FORCE;
                break;

            case DMA_CH9:
                return DMA9REQbits.FORCE;
                break;

            case DMA_CH10:
                return DMA10REQbits.FORCE;
                break;

            case DMA_CH11:
                return DMA11REQbits.FORCE;
                break;

            case DMA_CH12:
                return DMA12REQbits.FORCE;
                break;

            case DMA_CH13:
                return DMA13REQbits.FORCE;
                break;

            case DMA_CH14:
                return DMA14REQbits.FORCE;            
                break;     

            default:
                return 0;
                break;
        }
    }
    else
        return 0;
}

uint16_t DMA_get_buffer_address (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {          
        switch (channel)
        {
            case DMA_CH0:
                return DMA0STAL;                  
                break;

            case DMA_CH1:
                return DMA1STAL;                
                break;

            case DMA_CH2:
                return DMA2STAL;               
                break;

            case DMA_CH3:
                return DMA3STAL;             
                break;

            case DMA_CH4:
                return DMA4STAL;              
                break;

            case DMA_CH5:
                return DMA5STAL;    
                break;

            case DMA_CH6:
                return DMA6STAL;    
                break;

            case DMA_CH7:
                return DMA7STAL;     
                break;

            case DMA_CH8:
                return DMA8STAL;    
                break;

            case DMA_CH9:
                return DMA9STAL;    
                break;

            case DMA_CH10:
                return DMA10STAL;    
                break;

            case DMA_CH11:
                return DMA11STAL;   
                break;

            case DMA_CH12:
                return DMA12STAL;    
                break;

            case DMA_CH13:
                return DMA13STAL;    
                break;

            case DMA_CH14:
                return DMA14STAL;               
                break;     

            default:
                return 0;
                break;
        }
    }
    else
        return 0;
}

void DMA_set_buffer_offset_sgl (uint8_t channel, uint16_t offset_h, uint16_t offset_l)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMA0STAL = offset_l;
                DMA0STAH = offset_h;
                break;

            case DMA_CH1:
                DMA1STAL = offset_l;
                DMA1STAH = offset_h;             
                break;

            case DMA_CH2:
                DMA2STAL = offset_l;
                DMA2STAH = offset_h;              
                break;

            case DMA_CH3:
                DMA3STAL = offset_l;
                DMA3STAH = offset_h;            
                break;

            case DMA_CH4:
                DMA4STAL = offset_l;
                DMA4STAH = offset_h;          
                break;

            case DMA_CH5:
                DMA5STAL = offset_l;
                DMA5STAH = offset_h;   
                break;

            case DMA_CH6:
                DMA6STAL = offset_l;
                DMA6STAH = offset_h;
                break;

            case DMA_CH7:
                DMA7STAL = offset_l;
                DMA7STAH = offset_h; 
                break;

            case DMA_CH8:
                DMA8STAL = offset_l;
                DMA8STAH = offset_h;
                break;

            case DMA_CH9:
                DMA9STAL = offset_l;
                DMA9STAH = offset_h;
                break;

            case DMA_CH10:
                DMA10STAL = offset_l;
                DMA10STAH = offset_h;
                break;

            case DMA_CH11:
                DMA11STAL = offset_l;
                DMA11STAH = offset_h;
                break;

            case DMA_CH12:
                DMA12STAL = offset_l;
                DMA12STAH = offset_h; 
                break;

            case DMA_CH13:
                DMA13STAL = offset_l;
                DMA13STAH = offset_h;
                break;

            case DMA_CH14:
                DMA14STAL = offset_l;
                DMA14STAH = offset_h;          
                break;     

            default:
                break;
        }    
    }
}

void DMA_set_buffer_offset_pp (uint8_t channel, uint16_t offset_h_a, uint16_t offset_l_a, uint16_t offset_h_b, uint16_t offset_l_b)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMA0STAL = offset_l_a;
                DMA0STAH = offset_h_a;
                DMA0STBL = offset_l_b;
                DMA0STBH = offset_h_b;
                break;

            case DMA_CH1:
                DMA1STAL = offset_l_a;
                DMA1STAH = offset_h_a; 
                DMA1STBL = offset_l_b;
                DMA1STBH = offset_h_b;            
                break;

            case DMA_CH2:
                DMA2STAL = offset_l_a;
                DMA2STAH = offset_h_a;  
                DMA2STBL = offset_l_b;
                DMA2STBH = offset_h_b;            
                break;

            case DMA_CH3:
                DMA3STAL = offset_l_a;
                DMA3STAH = offset_h_a; 
                DMA3STBL = offset_l_b;
                DMA3STBH = offset_h_b;
                break;

            case DMA_CH4:
                DMA4STAL = offset_l_a;
                DMA4STAH = offset_h_a;
                DMA4STBL = offset_l_b;
                DMA4STBH = offset_h_b;
                break;

            case DMA_CH5:
                DMA5STAL = offset_l_a;
                DMA5STAH = offset_h_a;
                DMA5STBL = offset_l_b;
                DMA5STBH = offset_h_b;
                break;

            case DMA_CH6:
                DMA6STAL = offset_l_a;
                DMA6STAH = offset_h_a;
                DMA6STBL = offset_l_b;
                DMA6STBH = offset_h_b;
                break;

            case DMA_CH7:
                DMA7STAL = offset_l_a;
                DMA7STAH = offset_h_a; 
                DMA7STBL = offset_l_b;
                DMA7STBH = offset_h_b;
                break;

            case DMA_CH8:
                DMA8STAL = offset_l_a;
                DMA8STAH = offset_h_a;
                DMA8STBL = offset_l_b;
                DMA8STBH = offset_h_b;
                break;

            case DMA_CH9:
                DMA9STAL = offset_l_a;
                DMA9STAH = offset_h_a;
                DMA9STBL = offset_l_b;
                DMA9STBH = offset_h_b;
                break;

            case DMA_CH10:
                DMA10STAL = offset_l_a;
                DMA10STAH = offset_h_a;
                DMA10STBL = offset_l_b;
                DMA10STBH = offset_h_b;
                break;

            case DMA_CH11:
                DMA11STAL = offset_l_a;
                DMA11STAH = offset_h_a;
                DMA11STBL = offset_l_b;
                DMA11STBH = offset_h_b;
                break;

            case DMA_CH12:
                DMA12STAL = offset_l_a;
                DMA12STAH = offset_h_a; 
                DMA12STBL = offset_l_b;
                DMA12STBH = offset_h_b;
                break;

            case DMA_CH13:
                DMA13STAL = offset_l_a;
                DMA13STAH = offset_h_a;
                DMA13STBL = offset_l_b;
                DMA13STBH = offset_h_b;
                break;

            case DMA_CH14:
                DMA14STAL = offset_l_a;
                DMA14STAH = offset_h_a;
                DMA14STBL = offset_l_b;
                DMA14STBH = offset_h_b;
                break;     

            default:
                break;
        }   
    }
}

void DMA_set_control_register (uint8_t channel, uint16_t DMAxCON)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMA0CON = 0xFFFF & DMAxCON;                    
                break;

            case DMA_CH1:
                DMA1CON = 0xFFFF & DMAxCON;                 
                break;

            case DMA_CH2:
                DMA2CON = 0xFFFF & DMAxCON;             
                break;

            case DMA_CH3:
                DMA3CON = 0xFFFF & DMAxCON;            
                break;

            case DMA_CH4:
                DMA4CON = 0xFFFF & DMAxCON;             
                break;

            case DMA_CH5:
                DMA5CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH6:
                DMA6CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH7:
                DMA7CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH8:
                DMA8CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH9:
                DMA9CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH10:
                DMA10CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH11:
                DMA11CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH12:
                DMA12CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH13:
                DMA13CON = 0xFFFF & DMAxCON;   
                break;

            case DMA_CH14:
                DMA14CON = 0xFFFF & DMAxCON;             
                break;     

            default:
                break;
        }    
    }
}

void DMA_set_peripheral_address (uint8_t channel, uint16_t DMAxPAD)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMA0PAD = DMAxPAD;                    
                break;

            case DMA_CH1:
                DMA1PAD = DMAxPAD;                  
                break;

            case DMA_CH2:
                DMA2PAD = DMAxPAD;              
                break;

            case DMA_CH3:
                DMA3PAD = DMAxPAD;            
                break;

            case DMA_CH4:
                DMA4PAD = DMAxPAD;              
                break;

            case DMA_CH5:
                DMA5PAD = DMAxPAD;    
                break;

            case DMA_CH6:
                DMA6PAD = DMAxPAD;    
                break;

            case DMA_CH7:
                DMA7PAD = DMAxPAD;    
                break;

            case DMA_CH8:
                DMA8PAD = DMAxPAD;    
                break;

            case DMA_CH9:
                DMA9PAD = DMAxPAD;    
                break;

            case DMA_CH10:
                DMA10PAD = DMAxPAD;   
                break;

            case DMA_CH11:
                DMA11PAD = DMAxPAD;    
                break;

            case DMA_CH12:
                DMA12PAD = DMAxPAD;    
                break;

            case DMA_CH13:
                DMA13PAD = DMAxPAD;    
                break;

            case DMA_CH14:
                DMA14PAD = DMAxPAD;              
                break;     

            default:
                break;
        }   
    }
}

void DMA_set_txfer_length(uint8_t channel, uint16_t length)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
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
}

void DMA_set_request_source (uint8_t channel, uint16_t req_source)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMA0REQ = req_source & 0x7FFF;                    
                break;

            case DMA_CH1:
                DMA1REQ = req_source & 0x7FFF;                 
                break;

            case DMA_CH2:
                DMA2REQ = req_source & 0x7FFF;             
                break;

            case DMA_CH3:
                DMA3REQ = req_source & 0x7FFF;            
                break;

            case DMA_CH4:
                DMA4REQ = req_source & 0x7FFF;             
                break;

            case DMA_CH5:
                DMA5REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH6:
                DMA6REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH7:
                DMA7REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH8:
                DMA8REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH9:
                DMA9REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH10:
                DMA10REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH11:
                DMA11REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH12:
                DMA12REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH13:
                DMA13REQ = req_source & 0x7FFF;   
                break;

            case DMA_CH14:
                DMA14REQ = req_source & 0x7FFF;             
                break;     

            default:
                break; 
        }
    }
}

uint8_t DMA_get_txfer_state (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {    
        if (DMA_struct[channel].txfer_state == DMA_TXFER_DONE)
        {
            DMA_struct[channel].txfer_state = DMA_TXFER_IDLE;
            return DMA_TXFER_DONE;
        }
        else if (DMA_struct[channel].txfer_state == DMA_TXFER_IN_PROGRESS)
        {
            return DMA_TXFER_IN_PROGRESS;
        }
        else
            return DMA_TXFER_IDLE;
    }
    else
        return DMA_TXFER_IDLE;
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
