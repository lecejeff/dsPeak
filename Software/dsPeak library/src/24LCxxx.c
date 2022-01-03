#include "24lcxxx.h"

uint8_t EEPROM_init (STRUCT_EEPROM *mem, uint8_t i2c_address)
{
    uint8_t i2c_buf[1];
    mem->address = i2c_address;
    
    i2c_buf[0] = i2c_address;
    I2C_master_write(I2C_port_1, i2c_buf, 1);
    while(I2C_wait(I2C_port_1)==1);
    if (I2C_get_ack_state(I2C_port_1) == 0)     // Did the I2C acknowledged?
    {
        mem->discovered = 0;
        return 0;
    }
    else
    {
        mem->discovered = 1;
        return 1;
    }
}

void EEPROM_write_byte (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t byte)
{
    
}

void EEPROM_write_buf (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t *buf, uint8_t length)
{
    
}

void EEPROM_write_page (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t *buf, uint8_t length)
{
    
}

uint8_t EEPROM_get_status (STRUCT_EEPROM *mem)
{
    return 0;
}

uint8_t EEPROM_discovered (STRUCT_EEPROM *mem)
{
    return mem->discovered;
}