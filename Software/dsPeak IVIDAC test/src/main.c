//****************************************************************************//
// File      :  main.c
//
// Includes  :  general.h
//              Project-related .h files
//
// Purpose   : dsPeak IVIDAC development project
//
// Intellitrol                   MPLab X v5.45                        16/11/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//

//----------------------------------------------------------------------------//
// dsPIC33EP512MU814 configuration fuses, do not remove
// Configuration settings can be changed in "Production . Set configuration bits"
// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Primary oscillator at POR is FRC
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select Bit (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function Bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock-switching enabled, Fail-safe clock monitor disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = ON               // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled )
#pragma config ALTI2C1 = ON             // Alternate I2C pins for I2C1 (ASDA1/ASCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = ON             // Alternate I2C pins for I2C2 (ASDA2/ASCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable Bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Aux Flash may be written)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)
// End of dsPIC33EP512MU814 configuration fuses ------------------------------//

void DSPIC_init (void);

#include "dspeak_generic.h"
#include "timer.h"
#include "uart.h"
#include "dma.h"
#include "i2c.h"
#include "ividac_driver.h"
#include "IVIDAC_test_bench.h"
#include "24LCxxx.h"

//#define SCREEN_ENABLE

STRUCT_IVIDAC IVIDAC_struct[2];
STRUCT_IVIDAC_MESSAGE IVIDAC_msg[1];
STRUCT_EXPANDER BENCH_io_expanders[2];
STRUCT_EEPROM EEPROM_struct[1];

uint8_t UART_tx_buffer[UART_MAX_TX] = {0};
uint8_t *UART_rx_data_ptr;
uint8_t UART_parse_result = 0;
uint8_t UART_rx_msg_flag = 0;
uint8_t uart_checksum = 0;
uint16_t dac_custom_code = 0;
uint8_t i = 0;
uint8_t sum = 0;
uint8_t eeprom_scan_adr = 0;
uint8_t eeprom_scanned_once = 0;
uint16_t relay_custom_code = 0;

int main() 
{
    DSPIC_init();  

    // Enable dsPeak USB-UART debug port on UART_3
    UART_init(UART_3, 115200, UART_MAX_RX);
   
    // Enable dsPeak MikroBus SPI port on SPI_4
    SPI_init(SPI_4, SPI_MODE0, PPRE_4_1, SPRE_8_1);     // Max sclk of 9MHz on SPI4 = divide by 8
    IVIDAC_init(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, IVIDAC_RESOLUTION_14BIT, AD5641_OUTPUT_NORMAL, IVIDAC_OUTPUT_DISABLE);    
    
    // Enable dsPeak MikroBus I2C port on Alternate I2C1 pins
    I2C_init(I2C_port_1, I2C_mode_master, 0); 
    
    // Initialize ALL io expanders (sets the output to a value of 0)
    EXPANDER_init(&BENCH_io_expanders[EXPANDER_BANK_0], EXPANDER_BANK_0_ADDRESS, EXPANDER_output_reset_value);    
    EXPANDER_init(&BENCH_io_expanders[EXPANDER_BANK_1], EXPANDER_BANK_1_ADDRESS, EXPANDER_output_reset_value);  
    
    //IVIDAC_set_output_state(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, IVIDAC_OUTPUT_ENABLE);
    
    IVIDAC_bench_com_init(&IVIDAC_msg[0]);

    while (1)
    {    
        
        // HW TESTING ONLY, TO BE REMOVED
        //__delay_ms(1);
        //IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, sine_dac_256p_14b[counter]);
        //if (++counter > 255){counter = 0;}
        //
        
        // UART data was received from external host
        if (UART_rx_done(UART_3) == UART_RX_COMPLETE)
        {
            UART_rx_data_ptr = UART_get_rx_buffer(UART_3);
            if (UART_rx_data_ptr != 0)
            {
                if (IVIDAC_parse_uart_rx_buffer(IVIDAC_msg, UART_rx_data_ptr) == 1)
                {
                    UART_rx_msg_flag = 1;
                }
            }
        }
        
        // Parsed UART message was received and confirmed good, process the command
        if (UART_rx_msg_flag == 1)
        {           
            if (IVIDAC_msg->IVIDAC_message_buf[ADR_OFFSET] == IVIDAC_I2C_EEPROM_ADR)
            {
                // If frame_buf[DATA_OFFSET] bit 7 == 1, command is read
                // If frame_buf[DATA_OFFSET] bit 7 == 0, command is write

                // If command is in write mode                
                if (((IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET] & 0x80)>>7) == 0)
                {
                    switch (IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET])
                    {
                        case CMD_TB_RESET_VARIABLES:
                            dac_custom_code = 0;
                            relay_custom_code = 0;
                            eeprom_scan_adr = 0;
                            eeprom_scanned_once = 0;
                            UART_rx_msg_flag = 0;  
                            break;
                        
                        case CMD_DAC_ZERO_CODE:
                            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, IVIDAC_ABSOLUTE_MINIMUM_CODE);
                            break;

                        case CMD_DAC_MAX_CODE:
                            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, IVIDAC_ABSOLUTE_MAXIMUM_CODE);
                            break;

                        case CMD_DAC_CAL_LOW_CODE: 
                            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, IVIDAC_REDUCED_CODE_LOW);   // Code extract from AD5641 datasheet
                            break;

                        case CMD_DAC_CAL_HIGH_CODE: 
                            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, IVIDAC_REDUCED_CODE_HIGH); // Code extract from AD5641 datasheet
                            break;

                        case CMD_DAC_CUSTOM_CODE:
                            dac_custom_code = ((IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] << 8) | IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET]);
                            IVIDAC_set_output_raw(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, AD5641_OUTPUT_NORMAL, dac_custom_code);        
                            break;
                            
                        case CMD_DAC_OE_STATE:
                            IVIDAC_set_output_state(&IVIDAC_struct[0], IVIDAC_ON_MIKROBUS2, IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET] & 0x01);
                            break;
                                                        
                        case CMD_TB_SGL_RELAY_BANK0_STATE: 
                            EXPANDER_write_bit(&BENCH_io_expanders[0], IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] & 0x0F, IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET] & 0x01);
                            break;
                            
                        case CMD_TB_SGL_RELAY_BANK1_STATE:
                            EXPANDER_write_bit(&BENCH_io_expanders[1], IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] & 0x0F, IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET] & 0x01);
                            break;
                            
                        case CMD_TB_MTL_RELAY_BANK0_STATE: 
                            relay_custom_code = ((IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] << 8) | IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET]);
                            EXPANDER_write_all(&BENCH_io_expanders[0], relay_custom_code);
                            break;
                            
                        case CMD_TB_MTL_RELAY_BANK1_STATE:
                            relay_custom_code = ((IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] << 8) | IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET]);
                            EXPANDER_write_all(&BENCH_io_expanders[1], relay_custom_code);
                            break;                            
                            
                        case CMD_TB_RELAY_SET_ALL:
                            EXPANDER_write_all(&BENCH_io_expanders[0], 0);
                            EXPANDER_write_all(&BENCH_io_expanders[1], 0);
                            break;
                            
                        case CMD_TB_RELAY_RESET_ALL:
                            EXPANDER_write_all(&BENCH_io_expanders[0], 0xFFFF);
                            EXPANDER_write_all(&BENCH_io_expanders[1], 0xFFFF);                            
                            break;
                            
                        case CMD_TB_I2C_EEPROM_SCAN:
                            eeprom_scan_adr = EEPROM_discover(&EEPROM_struct[0], IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET], IVIDAC_msg->IVIDAC_message_buf[DATA2_OFFSET]);
                            eeprom_scanned_once = 1;
                            break;                                                     
                    }
                }
                
                // Command is in read mode
                if (((IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET] & 0x80)>>7) == 1)
                {    
                    // Save actual frame values for response
                    UART_tx_buffer[ADR_OFFSET] = IVIDAC_I2C_EEPROM_ADR;
                    UART_tx_buffer[COMMAND_OFFSET] = IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET]; 
                    
                    // Remove read bit mask for parsing read command with write #defines only
                    IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET] = (IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET] - CMD_READ_BIT_MASK);
                    switch (IVIDAC_msg->IVIDAC_message_buf[COMMAND_OFFSET])
                    {
                        case CMD_DAC_ZERO_CODE:
                            dac_custom_code = IVIDAC_ABSOLUTE_MINIMUM_CODE;
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = ((dac_custom_code & 0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = dac_custom_code & 0x00FF;
                            break;

                        case CMD_DAC_MAX_CODE:                            
                            dac_custom_code = IVIDAC_ABSOLUTE_MAXIMUM_CODE;
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                            
                            UART_tx_buffer[DATA1_OFFSET] = ((dac_custom_code & 0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = dac_custom_code & 0x00FF;                            
                            break;

                        case CMD_DAC_CAL_LOW_CODE:                             
                            dac_custom_code = IVIDAC_REDUCED_CODE_LOW;
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                            
                            UART_tx_buffer[DATA1_OFFSET] = ((dac_custom_code & 0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = dac_custom_code & 0x00FF;                              
                            break;

                        case CMD_DAC_CAL_HIGH_CODE:
                            dac_custom_code = IVIDAC_REDUCED_CODE_HIGH;
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = ((dac_custom_code & 0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = dac_custom_code & 0x00FF;                              
                            break;

                        case CMD_DAC_CUSTOM_CODE:                          
                            dac_custom_code = IVIDAC_get_output_value(&IVIDAC_struct[0]);
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = ((dac_custom_code & 0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = dac_custom_code & 0x00FF;  
                            break;
                            
                        case CMD_DAC_OE_STATE:
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = 0;  // Not implemented 
                            UART_tx_buffer[DATA2_OFFSET] = IVIDAC_get_output_state(&IVIDAC_struct[0]);  
                            break;   
                            
                        case CMD_TB_SGL_RELAY_BANK0_STATE: 
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] & 0x0F;  // Return relay index
                            UART_tx_buffer[DATA2_OFFSET] = EXPANDER_get_output_single(&BENCH_io_expanders[0], IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] & 0x0F);                          
                            break;
                            
                        case CMD_TB_SGL_RELAY_BANK1_STATE:
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] & 0x0F;  // Return relay index
                            UART_tx_buffer[DATA2_OFFSET] = EXPANDER_get_output_single(&BENCH_io_expanders[1], IVIDAC_msg->IVIDAC_message_buf[DATA1_OFFSET] & 0x0F);                                 
                            break;
                            
                        case CMD_TB_MTL_RELAY_BANK0_STATE: 
                            relay_custom_code = EXPANDER_get_output_all(&BENCH_io_expanders[0]);
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = ((relay_custom_code&0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = relay_custom_code;                          
                            break;
                            
                        case CMD_TB_MTL_RELAY_BANK1_STATE:
                            relay_custom_code = EXPANDER_get_output_all(&BENCH_io_expanders[1]);
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = ((relay_custom_code&0xFF00)>>8);
                            UART_tx_buffer[DATA2_OFFSET] = relay_custom_code;                               
                            break;                                                  
                            
                        case CMD_TB_I2C_EEPROM_SCAN:
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = 0;  // Not implemented    
                            UART_tx_buffer[DATA2_OFFSET] = eeprom_scan_adr;                               
                            break;
                            
                        case CMD_TB_I2C_EEPROM_DISCOVERED:
                            UART_tx_buffer[DATA0_OFFSET] = 0;  // Not implemented                           
                            UART_tx_buffer[DATA1_OFFSET] = 0;  // Not implemented    
                            UART_tx_buffer[DATA2_OFFSET] = EEPROM_discovered(&EEPROM_struct[0]);                               
                            break;
                    }
                    
                    // After read operation received and completed, send response                  
                    sum = 0;
                    for (i=0; i < CHECKSUM_OFFSET; i++)
                    {
                        sum = sum + UART_tx_buffer[i];
                    }
                    UART_tx_buffer[CHECKSUM_OFFSET] = sum;                 
                    UART_fill_tx_buffer(UART_3, UART_tx_buffer, UART_MAX_TX);
                    UART_send_tx_buffer(UART_3);
                }
            }
            UART_rx_msg_flag = 0;
            UART_clear_rx_buffer(UART_3);
        }
    }
    return 0;
}


void DSPIC_init (void)
{ 
    INTCON1bits.NSTDIS = 0;                         // Nested interrupt enabled 
    RCONbits.SWDTEN=0;                              // Watchdog timer disabled 
    
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    // Input HS OSC 20MHz. Output required is 140MHz for 70MIPS  
    // Configure PLL prescaler (N1), PLL postscaler (N2), PLL divisor (M)
    // Where Fout = Fin * (M / (N1 x N2))
    // M = PLLDIV + 2
    // N1 = PLLPRE + 2
    // N2 = 2 x (PLLPOST + 1)
    // Refer to dsPIC33E FRM - Oscillator (DS70850C) p.23 figure 7-8 for PLL
    // limitations for the M, N1 and N2 values
    //
    // PLLPRE = 3, N1 = 5
    // PLLDIV = 68, M = 70
    // PLLPOST = 0, N2 = 2
    // Fout = 20MHz * (140 / (10*2)) = 140MHz = 70MIPS
    PLLFBDbits.PLLDIV = 68; 
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 3;
    
    // Initiate Clock Switch to Primary oscillator with PLL
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);    // While COSC doesn't read back Primary Oscillator config
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
    
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELG = 0;

    DSPEAK_LED1_DIR = 0;
    DSPEAK_LED1_STATE = 0;
    DSPEAK_LED2_DIR = 0;
    DSPEAK_LED2_STATE = 0;
    DSPEAK_LED3_DIR = 0;
    DSPEAK_LED4_STATE = 0;
    DSPEAK_LED4_DIR = 0;
    DSPEAK_LED4_STATE = 0;       
    
    DMA_struct_init(DMA_ALL_INIT);
}
