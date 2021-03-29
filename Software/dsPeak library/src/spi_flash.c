#include "spi_flash.h"
#include "uart.h"

STRUCT_FLASH flash_state;

void SPI_flash_init (void)
{
    SPI_init(SPI_2, SPI_MODE0, 3, 0);   // Set SPI2 to 8.75MHz
 
    flash_state.state = SPI_FLASH_STATE_INIT;  
    flash_state.prev_state = SPI_FLASH_STATE_INIT;    
    
    TRISJbits.TRISJ15 = 0;              // Set flash WP pin to output
    TRISAbits.TRISA0 = 0;               // Set flash HOLD pin to output
    FLASH_WP_PIN = 1;                   // Write protect is inactive
    FLASH_HOLD_PIN = 1;                 // HOLD is inactive    
}

void SPI_flash_write (uint32_t adr, uint8_t *ptr, uint8_t length)
{
    uint8_t buf[length + 4];
    uint8_t i=0;

    flash_state.prev_state = flash_state.state;
    flash_state.state = SPI_FLASH_WRITE; 
    
    // If previous FLASH operation is an ERASE operation, the WEL bit must be
    // reset to 1 to perform a write
    if (flash_state.prev_state == SPI_FLASH_ERASE)
    {
        SPI_flash_write_enable();
    }
       
    buf[0] = CMD_PAGE_PROGRAM;
    buf[1] = ((adr & 0xFF0000)>>16);
    buf[2] = ((adr & 0x00FF00)>>8);
    buf[3] = adr;
    for (; i < length; i++)
    {
        buf[i+4] = *ptr++;
    }
    SPI_master_write(SPI_2, buf, (length+4), FLASH_MEMORY_CS);
}

uint8_t * SPI_flash_read (uint32_t adr, uint8_t length)
{
    uint8_t buf[length + 4];
    uint8_t i=0;

    flash_state.prev_state = flash_state.state;
    flash_state.state = SPI_FLASH_READ;  
    
    buf[0] = CMD_NORMAL_READ;
    buf[1] = ((adr & 0xFF0000)>>16);
    buf[2] = ((adr & 0x00FF00)>>8);
    buf[3] = adr;
    for (; i < length; i++)
    {
        buf[i+4] = 0;
    }
    SPI_master_write(SPI_2, buf, (length+4), FLASH_MEMORY_CS); 
    while(!SPI_txfer_done(SPI_2));   
    return (SPI_get_rx_buffer(SPI_2) + 4);
}

void SPI_flash_erase (uint8_t type, uint32_t adr)
{
    uint8_t buf[4] = {type, ((adr & 0xFF0000)>>16), ((adr & 0x00FF00)>>8), adr};
    
    flash_state.prev_state = flash_state.state;
    flash_state.state = SPI_FLASH_ERASE;     
    
    SPI_master_write(SPI_2, buf, 4, FLASH_MEMORY_CS); 
    
    while (SPI_flash_busy() == 1);
}

// Function reads the FLASH busy flag. If flag == 0, device is ready for operation
// If flag == 1, device is busy with internal operation
uint8_t SPI_flash_busy (void)
{
    uint8_t buf[2] = {CMD_READ_STATUS1, 0};
    uint8_t *status_reg;
    SPI_master_write(SPI_2, buf, 2, FLASH_MEMORY_CS);   
    while(!SPI_txfer_done(SPI_2));
    status_reg = SPI_get_rx_buffer(SPI_2);          // +1 since result from read is byte #2
    if ((*status_reg & 0x01) == 0)
    {               
        return 0;
    }
    else
    {            
        return 1;
    }     
}

void SPI_flash_write_enable(void)
{
    uint8_t buf[1] = {CMD_WRITE_ENABLE};
    FLASH_WP_PIN = 1;
    __delay_us(10);     
    SPI_master_write(SPI_2, buf, 1, FLASH_MEMORY_CS);
    flash_state.prev_state = flash_state.state;
    flash_state.state = SPI_FLASH_WRITE_ENABLE;  
}

void SPI_flash_write_disable(void)
{
    uint8_t buf[1] = {CMD_WRITE_DISABLE};
    SPI_master_write(SPI_2, buf, 1, FLASH_MEMORY_CS);
    FLASH_WP_PIN = 0;
    __delay_us(10);
    flash_state.prev_state = flash_state.state;
    flash_state.state = SPI_FLASH_WRITE_DISABLE;  
}

uint8_t SPI_flash_get_state (void)
{
    return flash_state.state;
}