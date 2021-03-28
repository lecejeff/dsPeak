#include "PMP.h"

STRUCT_PMP PMP_struct;

void PMP_init (unsigned char mode)
{   
    PMP_pins_init();        // Configure ALL PMP pins
    PMP_set_bus_output();
}

void PMP_write (unsigned char mode, unsigned int adr, unsigned int data)
{
    PMP_set_bus_output();
    switch (mode)
    {
        case PMP_MODE_SRAM:
            // Place address on bus
            PMP_WR_STROBE = 1;      // PMENB, set to 1, #OE inactive
            LATJ = (LATJ & 0xC000) | (adr & 0x3FFF); 
            
            // Place data(7..0) on data bus first
            PMP_RD_STROBE = 1;      // Direction of latches : dsPIC33E -> memory
            PMP_PMBE_STROBE = 0;    // Low byte output enable to memory 
            PMP_CS2_STROBE = 0;     // CS2 = 0       
            LATH = ((LATH & 0xFF00) | (data & 0x00FF));
            PMP_CS2_STROBE = 1;     // CS2 = 1
            
            // Place data(15..8) on data bus 
//            PMP_RD_STROBE = 1;      // Direction of latches : dsPIC33E -> memory
//            PMP_PMBE_STROBE = 1;    // High byte output enable to memory  
//            PMP_CS2_STROBE = 0;     // CS2 = 0                                  
//            LATH = ((LATH & 0xFF00) | ((data & 0xFF00)>>8));
//            __delay_us(1);
//            PMP_CS2_STROBE = 1;     // CS2 = 1            
            break;
            
        case PMP_MODE_TFT:
            PMP_CS1_STROBE = 0;    // CS1 = 0
            PMP_RD_STROBE = 1;    // RD = 1;
            PMP_WR_STROBE = 0;    // WR = 0;

            LATH = ((LATH & 0xFF00) | (data & 0x00FF));

            PMP_WR_STROBE = 1;    // WR = 1;
            PMP_CS1_STROBE = 1;    // CS1 = 1            
            break;
            
        default:
            break;
    }
}

unsigned int PMP_read (unsigned char mode, unsigned int adr)
{
    unsigned int result = 0, result_l = 0, result_h = 0;
    PMP_set_bus_input();      
    switch (mode)
    {
        case PMP_MODE_SRAM:
            // Place address on bus            
            LATJ = (LATJ & 0xC000) | (adr & 0x3FFF); 
            
//            // Place data(7..0) on data bus first
            PMP_CS2_STROBE = 0;     // CS2 = 0
            __delay_us(1);
            PMP_RD_STROBE = 0;      // Direction of latches : dsPIC33E <- memory
            __delay_us(1);
            PMP_PMBE_STROBE = 0;    // Low byte output enable from memory   
            __delay_us(1);
            PMP_WR_STROBE = 0;      // SRAM #OE = 0    
            __delay_us(1);
            result_l = PORTH & 0x00FF;
            PMP_WR_STROBE = 1;      // SRAM #OE = 1   
            __delay_us(1);            
            PMP_CS2_STROBE = 1;     // CS2 = 1
            __delay_us(1);
            
            // Place data(15..8) on data bus 
//            PMP_CS2_STROBE = 0;     // CS2 = 0
//            PMP_RD_STROBE = 0;      // Direction of latches : dsPIC33E <- memory
//            PMP_PMBE_STROBE = 1;    // High byte output enable to memory               
//            PMP_WR_STROBE = 0;      // SRAM #OE = 0
//            __delay_us(1);
//            PMP_WR_STROBE = 1;      // SRAM #OE = 1 
//            result_h = ((PORTH & 0x00FF)<<8);               
//            PMP_CS2_STROBE = 1;     // CS2 = 1      
                        
            result = result_l;
            break;
            
        case PMP_MODE_TFT:
            PMP_CS1_STROBE = 0;    // CS1 = 0
            PMP_WR_STROBE = 1;    // WR = 1;       

            PMP_RD_STROBE = 0;    // RD = 0;
            PMP_RD_STROBE = 1;    // RD = 1;
            PMP_RD_STROBE = 0;    // RD = 0;
            result = PORTH & 0x00FF; 
            PMP_RD_STROBE = 1;    // RD = 1;     

            PMP_CS1_STROBE = 1;    // CS1 = 1            
            break;
            
        default:
            result = 0;
            break;
    }
    
    return result;
}

void PMP_pins_init (void)
{    
    // PMP adress bus pins setup to output
    TRISJ &= 0xC000;
    TRISK &= 0x07FE;
    LATK |= 0xF801;
}

void PMP_set_bus_output (void)
{
    TRISH &= 0xFF00;   
}

void PMP_set_bus_input (void)
{
    TRISH |= 0x00FF;  
}