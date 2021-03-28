#ifndef __PMP_H_
#define	__PMP_H_

#include "general.h"

#define PMP_WRITE           0
#define PMP_READ            1

#define PMP_MODE_TFT        0
#define PMP_MODE_SRAM       1

#define PMP_CS1_TFT         0
#define PMP_CS2_SRAM        1

#define LCD_RESET_LATCH     LATKbits.LATK0
#define LCD_DAT_CMD_LATCH   LATJbits.LATJ0
#define PMP_CS1_STROBE      LATKbits.LATK11
#define PMP_CS2_STROBE      LATKbits.LATK12
#define PMP_PMBE_STROBE     LATKbits.LATK13
#define PMP_WR_STROBE       LATKbits.LATK14
#define PMP_RD_STROBE       LATKbits.LATK13

typedef struct
{
    unsigned char old_cmd;
    unsigned char new_cmd;
    unsigned int address;
    unsigned char state;
    unsigned char chip_select;
}STRUCT_PMP;

void PMP_init (unsigned char mode);
void PMP_write (unsigned char mode, unsigned int adr, unsigned int data);
unsigned int PMP_read (unsigned char mode, unsigned int adr);
void PMP_pins_init (void);
void PMP_set_bus_output (void);
void PMP_set_bus_input (void);

#endif	

