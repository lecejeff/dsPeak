//****************************************************************************//
// File      :  main.c
//
// Includes  :  general.h
//              Project-related .h files
//
// Purpose   : dsPeak development project
//
// Intellitrol                   MPLab X v5.45                        06/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "dspeak_generic.h"

//----------------------------------------------------------------------------//
// dsPIC33EP512MU814 configuration fuses, do not remove
// Configuration settings can be changed in "Production -> Set configuration bits"
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

// dsPeak libraries
#include "rc_dac.h"
#include "adc.h"
#include "rtcc.h"
#include "timer.h"
#include "uart.h"
#include "codec.h"
#include "pwm.h"
#include "qei.h"
#include "spi_flash.h"
#include "mcontrol.h"
#include "CAN.h"
#include "image_file.h"
#include "ST7735s.h"
#include "dma.h"
#include "i2c.h"
#include "rot_encoder.h"
#include "ividac_driver.h"
#include "ft8xx.h"
#include "bno080.h"

// Access to CAN struct members
extern STRUCT_CAN CAN_struct[CAN_QTY];
STRUCT_CAN *CAN1_struct = &CAN_struct[CAN_1];
STRUCT_CAN_MSG CAN_MSG_DSPEAK;

// Access to RCDAC struct members
extern STRUCT_RCDAC RCDAC_struct[RCDAC_QTY];
STRUCT_RCDAC *RCDAC1_struct = &RCDAC_struct[RCDAC_1];
STRUCT_RCDAC *RCDAC2_struct = &RCDAC_struct[RCDAC_2];

// Access to PWM struct members
extern STRUCT_PWM PWM_struct[PWM_QTY];
STRUCT_PWM *PWM1L_struct = &PWM_struct[PWM_1L];
STRUCT_PWM *PWM1H_struct = &PWM_struct[PWM_1H];
STRUCT_PWM *PWM2L_struct = &PWM_struct[PWM_2L];
STRUCT_PWM *PWM2H_struct = &PWM_struct[PWM_2H];
STRUCT_PWM *PWM5H_struct = &PWM_struct[PWM_5H];
STRUCT_PWM *PWM6L_struct = &PWM_struct[PWM_6L];
STRUCT_PWM *PWM6H_struct = &PWM_struct[PWM_6H];

// Access to I2C struct members
extern STRUCT_I2C i2c_struct[I2C_QTY];
STRUCT_I2C *I2C1_struct = &i2c_struct[I2C_1];           // MikroBus I2C port, 100kHz
STRUCT_I2C *I2C2_struct = &i2c_struct[I2C_2];           // RC-DAC I2C port, 400kHz

// Access to UART struct members
extern STRUCT_UART UART_struct[UART_QTY];
STRUCT_UART *UART_485_struct = &UART_struct[UART_1];
STRUCT_UART *UART_MKB_struct = &UART_struct[UART_2];
STRUCT_UART *UART_DEBUG_struct = &UART_struct[UART_3];

// Access to TIMER struct members
extern STRUCT_TIMER TIMER_struct[TIMER_QTY];
STRUCT_TIMER *TIMER1_struct = &TIMER_struct[TIMER_1];
STRUCT_TIMER *TIMER2_struct = &TIMER_struct[TIMER_2];
STRUCT_TIMER *TIMER3_struct = &TIMER_struct[TIMER_3];
STRUCT_TIMER *TIMER4_struct = &TIMER_struct[TIMER_4];
STRUCT_TIMER *TIMER5_struct = &TIMER_struct[TIMER_5];
STRUCT_TIMER *TIMER6_struct = &TIMER_struct[TIMER_6];
STRUCT_TIMER *TIMER7_struct = &TIMER_struct[TIMER_7];
STRUCT_TIMER *TIMER8_struct = &TIMER_struct[TIMER_8];
STRUCT_TIMER *TIMER9_struct = &TIMER_struct[TIMER_9];

// Access to SPI struct members
extern STRUCT_SPI SPI_struct[SPI_QTY];
STRUCT_SPI *EVE_spi = &SPI_struct[SPI_1];       // Assign EVE_spi to SPI_1
STRUCT_SPI *SPI_codec = &SPI_struct[SPI_3];     // Assign CODEC to SPI_3

// Access to CODEC struct members
extern STRUCT_CODEC CODEC_struct[CODEC_QTY];
STRUCT_CODEC *CODEC_sgtl5000 = &CODEC_struct[DCI_0];

// Access to FLASH struct members
extern STRUCT_FLASH FLASH_struct[FLASH_QTY];
STRUCT_SPI *SPI_flash = &SPI_struct[SPI_2];

// Access to BUTTON struct members
extern STRUCT_BUTTON BUTTON_struct[BTN_QTY];
STRUCT_BUTTON *BTN1_struct = &BUTTON_struct[BTN_1];
STRUCT_BUTTON *BTN2_struct = &BUTTON_struct[BTN_2];
STRUCT_BUTTON *BTN3_struct = &BUTTON_struct[BTN_3];
STRUCT_BUTTON *BTN4_struct = &BUTTON_struct[BTN_4];

// Access to LED struct members
extern STRUCT_LED LED_struct[BTN_QTY];
STRUCT_LED *LED1_struct = &LED_struct[LED_1];
STRUCT_LED *LED2_struct = &LED_struct[LED_2];
STRUCT_LED *LED3_struct = &LED_struct[LED_3];
STRUCT_LED *LED4_struct = &LED_struct[LED_4];

// Access to ENCODER struct members
extern STRUCT_ENCODER ENCODER_struct[ENCODER_QTY];
STRUCT_ENCODER *ENC1_struct = &ENCODER_struct[ENC_1];

// Access to RTC struct member
extern STRUCT_RTCC RTCC_struct[RTC_QTY];
STRUCT_RTCC *RTC1_struct = &RTCC_struct[RTC_1];

// Access to Bridgetek EVE struct member
extern STRUCT_BT8XX BT8XX_struct[BT8XX_QTY];
STRUCT_BT8XX *eve = &BT8XX_struct[BT8XX_1];

// Access to Bridgetek EVE struct member
extern STRUCT_BNO08X BNO08X_struct[BNO08X_QTY];
STRUCT_BNO08X *BNO_struct = &BNO08X_struct[BNO08X_1];

void pid_func (void);

// Debug variables related to functions under development
uint8_t new_pid_out1 = 0, new_pid_out2 = 0;
uint8_t direction_1 = 0, direction_2 = 0;
uint8_t state = 0;
uint16_t counter_5sec  = 0;
uint16_t speed_rpm_table[5] = {15, 20, 25, 30, 35};
uint8_t motor_debug_buf[18] = {0};
int error_rpm;
uint16_t setpoint_rpm;
uint16_t actual_rpm;

uint16_t i = 0;

uint8_t *u485_rx_buf1;
uint8_t *u485_rx_buf2;
uint8_t u485_1_data_flag = 0;
uint8_t u485_2_data_flag = 0;
uint8_t counter_485_native = 0;
uint8_t u485_2_rx = 0;

uint8_t record_flag = 0, playback_flag = 0, erase_flag = 0;
uint8_t record_cnt = 0, playback_cnt = 0;

uint8_t flash_state_machine = 0;

uint8_t spi_release = 0;
uint8_t DCI_tx_flag = 0;
uint8_t DCI_rx_flag = 0;

uint16_t codec_wr_ptr[CODEC_BLOCK_TRANSFER];
uint8_t spi2_buf[260];

uint16_t * dci_rx_ptr;
uint8_t * spi_rd_ptr;
uint32_t flash_wr_adr = 0;  // 32Mbit = 4Mbyte / 4Byte/sample = 1Msample
uint32_t flash_rd_adr = 0;
uint8_t last_txfer = 0;

uint8_t flash_status1_reg = 0;

uint16_t color;

uint16_t encoder_rpm = 0;
uint8_t encoder_direction = 0;
uint16_t encoder_old_position = 0;
uint16_t encoder_new_position = 0;

uint8_t hp_vol_l = 0x18;
uint8_t hp_vol_r = 0x18;

uint8_t spi_codec_state = 0;

uint8_t codec_volume_flag = 0;
uint8_t codec_spi_tx_flag = 0;

uint8_t buf_test[9] = {0};

uint8_t tag = 0;
uint16_t track_upd = 0;
uint32_t tracker = 0;
uint8_t track_counter = 0;
uint8_t key = 0;

uint16_t bno_rd_length = 0;
uint8_t *i2c_buf;
uint8_t BNO08X_read = 0;
uint8_t BNO08X_flag = 0;
uint8_t BNO08X_state = 0;

uint8_t dummy = 0;
uint8_t bno08x_advertise[2] = {BNO08X_CHANNEL0_SHTP, BNO08X_SHTP_ADVERTISE_HOST};
uint8_t data[1] = {BNO08X_DEFAULT_ADDRESS};
uint8_t set_feature_acc[21] = {0};
uint8_t UART_debug_flag = 0;
uint8_t y_axis_flag = 0;
int16_t y_axis = 0;
int16_t y_axis_setpoint = 0;
double p_gain = 0.03;
double i_gain = 0.0005;
double d_gain = 0;

double p_value;
double i_value;
double d_value;

uint16_t pid;
uint16_t pid_out = 0;
uint8_t pid_flag = 0;
uint8_t direction = 0;

int16_t y_error = 0;

int main() 
{
    dsPeak_init(); 
    
    dsPeak_button_init(BTN1_struct, BTN_1, 10);
    dsPeak_button_init(BTN2_struct, BTN_2, 10);
    dsPeak_button_init(BTN3_struct, BTN_3, 10);
    dsPeak_button_init(BTN4_struct, BTN_4, 10);
 
    dsPeak_led_init(LED1_struct, LED_1, LOW);
    dsPeak_led_init(LED2_struct, LED_2, LOW);
    dsPeak_led_init(LED3_struct, LED_3, LOW);
    dsPeak_led_init(LED4_struct, LED_4, LOW);    
    
    RTCC_init(RTC1_struct, RTC_1);   
    RTCC_write_time(RTC1_struct);

#ifdef UART_DEBUG_ENABLE
    UART_init(UART_DEBUG_struct, UART_3, 115200, UART_MAX_TX, UART_MAX_RX, DMA_CH14);
    while(UART_putstr_dma(UART_DEBUG_struct, "dsPeak UART debug port with DMA is enabled\r\n") != 1);   
#endif

    MOTOR_init(PWM1H_struct, PWM1L_struct, MOTOR_1, 30);
    MOTOR_init(PWM2H_struct, PWM2L_struct, MOTOR_2, 30);    
    
    //Physical rotary encoder initialization
    //ENCODER_init(ENC1_struct, ENC_1, 30); 

    BNO08X_init(BNO_struct, I2C1_struct, BNO08X_MKB2);  
    while(BNO08X_has_reset(BNO_struct) == 0);
       
    // Timers init / start should be the last function calls made before while(1) 
    TIMER_init(TIMER1_struct, TIMER_1, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER2_struct, TIMER_2, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    TIMER_init(TIMER3_struct, TIMER_3, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);   
    TIMER_init(TIMER4_struct, TIMER_4, TIMER_MODE_16B, TIMER_PRESCALER_1, 900000);
    TIMER_init(TIMER5_struct, TIMER_5, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);   
    TIMER_init(TIMER6_struct, TIMER_6, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);   
    TIMER_init(TIMER7_struct, TIMER_7, TIMER_MODE_16B, TIMER_PRESCALER_256, 30);    // Motor driver refresh timer     
    TIMER_init(TIMER8_struct, TIMER_8, TIMER_MODE_16B, TIMER_PRESCALER_1, 900000);      // 
    TIMER_init(TIMER9_struct, TIMER_9, TIMER_MODE_16B, TIMER_PRESCALER_1, 900000);  // BNO08X state machine   

    TIMER_start(TIMER1_struct);
    TIMER_start(TIMER2_struct);
    TIMER_start(TIMER3_struct);   
    TIMER_start(TIMER4_struct);
    TIMER_start(TIMER5_struct);
    TIMER_start(TIMER6_struct);
    TIMER_start(TIMER7_struct);
    TIMER_start(TIMER8_struct);
    TIMER_start(TIMER9_struct);
    
    while (1)
    {      
        // BNO08X is ready for a new data transfer
        if (BNO08X_int_state(BNO_struct) == 1)
        {
            BNO08X_flag = 1;
        }
        
        // dsPeak on-board button debouncer state machine
        if (TIMER_get_state(TIMER1_struct, TIMER_INT_STATE) == 1)
        { 
            dsPeak_button_debounce(BTN1_struct);
            dsPeak_button_debounce(BTN2_struct);
            dsPeak_button_debounce(BTN3_struct);
            dsPeak_button_debounce(BTN4_struct); 
            
            if (dsPeak_button_get_state(BTN1_struct) == LOW)
            {
                if (BTN1_struct->do_once == 0) 
                {
                    BTN1_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN1 pressed\r\n");
                }
            }
            else
            {
                BTN1_struct->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN2_struct) == LOW)
            {
                if (BTN2_struct->do_once == 0) 
                {
                    BTN2_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN2 pressed\r\n");
                }
            }
            else
            {
                BTN2_struct->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN3_struct) == LOW)
            {
                if (BTN3_struct->do_once == 0) 
                {
                    BTN3_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN3 pressed\r\n");
                }
            }
            else
            {
                BTN3_struct->do_once = 0;
            }

            if (dsPeak_button_get_state(BTN4_struct) == LOW)
            {
                if (BTN4_struct->do_once == 0) 
                {                   
                    BTN4_struct->do_once = 1;
                    UART_putstr_dma(UART_DEBUG_struct, "BTN4 pressed\r\n");
                }
            }
            else
            {
                BTN4_struct->do_once = 0;
            }               
        }   
        
        if (TIMER_get_state(TIMER2_struct, TIMER_INT_STATE) == 1)
        {
            if (++counter_5sec > 20)
            {
                counter_5sec = 0;               
            }

        } 
        
        if (TIMER_get_state(TIMER3_struct, TIMER_INT_STATE) == 1)
        {
            dummy = 1;
        } 
        
        // Motor debug port
        if (TIMER_get_state(TIMER4_struct, TIMER_INT_STATE) == 1)
        {
            if (UART_debug_flag == 1)
            {
                UART_debug_flag = 0;
                i2c_buf+=13;
                // Send BNO08X data to serial port
                motor_debug_buf[0] = 0xAE;
                motor_debug_buf[1] = 0xAE;
                motor_debug_buf[2] = *i2c_buf++;    // X axis LSB   
                motor_debug_buf[3] = *i2c_buf++;    // X axis MSB
                y_axis = *i2c_buf;
                motor_debug_buf[4] = *i2c_buf++;    // Y axis LSB  
                y_axis = *i2c_buf << 8 | y_axis;
                y_axis_flag = 1;
                motor_debug_buf[5] = *i2c_buf++;    // Y axis MSB
                motor_debug_buf[6] = *i2c_buf++;    // Z axis LSB   
                motor_debug_buf[7] = *i2c_buf;      // Z axis MSB
                motor_debug_buf[8] = pid_out;
                motor_debug_buf[9] = ((pid_out & 0xFF00) >> 8);
                motor_debug_buf[10] = y_error;
                motor_debug_buf[11] = ((y_error & 0xFF00) >> 8);
                UART_putbuf_dma(UART_DEBUG_struct, motor_debug_buf, 12);
            }
        }  
      
        if (TIMER_get_state(TIMER5_struct, TIMER_INT_STATE) == 1)
        {
            pid_func();
        } 
        
        // QEI velocity refresh rate
        if (TIMER_get_state(TIMER6_struct, TIMER_INT_STATE) == 1)
        {   
            QEI_calculate_velocity(QEI_2);  
            new_pid_out2 = MOTOR_drive_pid(MOTOR_2); 
            MOTOR_drive_perc(MOTOR_2, MOTOR_get_direction(MOTOR_2), pid_out);
        }        
        
        // QEI velocity refresh rate
        if (TIMER_get_state(TIMER7_struct, TIMER_INT_STATE) == 1)
        {              
            QEI_calculate_velocity(QEI_1);  
            new_pid_out1 = MOTOR_drive_pid(MOTOR_1);
            MOTOR_drive_perc(MOTOR_1, MOTOR_get_direction(MOTOR_1), pid_out);
        }

        if (TIMER_get_state(TIMER9_struct, TIMER_INT_STATE) == 1)
        { 
            dummy = 1;
        }

        if (TIMER_get_state(TIMER8_struct, TIMER_INT_STATE) == 1)
        {   
            switch (BNO08X_state)
            {
                case 0: // Reset procedure, shtp advertisement from BNO to host
                    if (BNO08X_flag == 1)
                    {
                        // Is BNO I2C bus free?
                        if (I2C_rx_done(BNO_struct->i2c_ref) == 1)
                        {
                            // Send 1st request to BNO08x, determine the amount of bytes to read
                            I2C_master_read(BNO_struct->i2c_ref, I2C_READ_MODE_STOP_START, data, 1, 4);
                            BNO08X_state = 1;
                            BNO08X_flag = 0;
                        }                        
                    }
                    break;

                case 1: // Get SHTP advertisement length and process message
                    if (I2C_rx_done(BNO_struct->i2c_ref) == 1)
                    {
                        i2c_buf = I2C_get_rx_buffer(I2C1_struct);
                        BNO08X_parse_shtp(BNO_struct, i2c_buf);
                        bno_rd_length = BNO08X_get_cargo_length(BNO_struct);
                        if (bno_rd_length > 0)
                        {
                            BNO08X_state = 2;                           
                        }
                        else
                        {
                            // Error 
                        }
                    }
                    break;

                case 2:
                    if (BNO08X_flag == 1)
                    {
                        I2C_master_read(I2C1_struct, I2C_READ_MODE_STOP_START, data, 1, bno_rd_length);
                        BNO08X_flag = 0;
                        BNO08X_state = 3;
                    }
                    break;
                    
                case 3: 
                    if (I2C_rx_done(BNO_struct->i2c_ref) == 1)
                    {
                        i2c_buf = I2C_get_rx_buffer(I2C1_struct);
                        BNO08X_parse_shtp(BNO_struct, i2c_buf);
                        bno_rd_length = BNO08X_get_cargo_length(BNO_struct);
                        if (BNO_struct->channel == 0x01)    // executable channel
                        {
                            i2c_buf += 4;
                            if (*i2c_buf == 0x01)
                            {
                                BNO08X_state = 4;   // Device idle
                            }
                        }
                        
                        else 
                        {
                            if (bno_rd_length > 0)
                            {
                                BNO08X_state = 2;
                            } 
                            else
                            {
                                BNO08X_state = 4;
                            }
                        }
                    }                    
                    break;
                    
                case 4:
                    // Idle after reset SHTP advertisement completed
                    dsPeak_led_write(LED1_struct, HIGH);
                    
                    // Issue set feature command to SHCC
                    set_feature_acc[0] = BNO08X_SHCC_SET_FEAT_COM;      // Report ID
                    set_feature_acc[1] = BNO08X_SENSOR_ID_GRAVITY;         // Feature report ID
                    set_feature_acc[2] = 0x00;                          // Feature flags (0x04 -> wake-up enabled)
                    set_feature_acc[3] = 0;        // Change sensitivity (abs / rel) LSB
                    set_feature_acc[4] = 0;        // Change sensitivity (abs / rel) MSB
                    set_feature_acc[5] = 0xA0;        // Report interval LSB  (in uSec) (1s = 100000us = 0x186A0
                    set_feature_acc[6] = 0x86;        // Report interval      (in uSec)
                    set_feature_acc[7] = 0x01;        // Report interval      (in uSec)
                    set_feature_acc[8] = 0x00;        // Report interval MSB  (in uSec)
                    set_feature_acc[9] = 0xFF;        // Batch interval LSB   (in uSec)
                    set_feature_acc[10] = 0xFF;       // Batch interval       (in uSec)
                    set_feature_acc[11] = 0xFF;       // Batch interval       (in uSec)
                    set_feature_acc[12] = 0xFF;       // Batch interval MSB   (in uSec)
                    set_feature_acc[13] = 0;       // Sensor-specific config LSB
                    set_feature_acc[14] = 0;       // Sensor-specific config
                    set_feature_acc[15] = 0;       // Sensor-specific config
                    set_feature_acc[16] = 0;       // Sensor-specific config MSB
                    
                    BNO08X_write(BNO_struct, BNO08X_CHANNEL2_SHCC, set_feature_acc, 17);
                    BNO08X_state = 5;
                    bno_rd_length = 4;
                    break;
                    
                case 5:
                    if (BNO08X_flag == 1)
                    {
                        I2C_master_read(I2C1_struct, I2C_READ_MODE_STOP_START, data, 1, bno_rd_length); // See response need how many bytes
                        BNO08X_flag = 0;
                        BNO08X_state = 6;
                    }
                    break;
                    
                case 6: 
                    if (I2C_rx_done(BNO_struct->i2c_ref) == 1)
                    {
                        i2c_buf = I2C_get_rx_buffer(I2C1_struct);
                        BNO08X_parse_shtp(BNO_struct, i2c_buf);
                        bno_rd_length = BNO08X_get_cargo_length(BNO_struct);
                        if (BNO_struct->channel == 0x03)    // sensor report channel
                        {
                            i2c_buf = I2C_get_rx_buffer_index(I2C1_struct, 9);
                            if (*i2c_buf == 0x06) // Magnetometer report
                            {
                                UART_debug_flag = 1;
                                i2c_buf = I2C_get_rx_buffer(I2C1_struct);
                            }
                        }
                        
                        if (bno_rd_length > 0)
                        {
                            BNO08X_state = 5;
                        } 
                        else
                        {
                            BNO08X_state = 7;
                        }
                    } 
                    break;
                    
                case 7:
                    break;
                    
                default:
                    break;
            }
        } 
    }
    return 0;
}

void pid_func (void)
{
    // Error = setpoint - input
    y_error = (y_axis_setpoint - y_axis);
    
    if (y_error >= 0)
    {
        MOTOR_set_direction(MOTOR_1, DIRECTION_FORWARD);
        MOTOR_set_direction(MOTOR_2, DIRECTION_BACKWARD);
    }
    else
    {
        MOTOR_set_direction(MOTOR_1, DIRECTION_BACKWARD);
        MOTOR_set_direction(MOTOR_2, DIRECTION_FORWARD);            
    }
    y_error = abs(y_error);
    
    if (y_error < 100)
    {
        pid_out = 0;
    }    
    else
    {   
        // Calculate proportional term
        p_value = (double)p_gain * (double)y_error;
        //m_control[channel].p_value = m_control[channel].p_calc_gain * m_control[channel].error_rpm;

        // Calculate integral term
        i_value += (double)((i_gain * 0.33) * (double)y_error);
        if (i_value > QEI_MOT1_MAX_RPM){i_value = i_value;}

        // Calculate derivative term
        // Compute PID output
        pid = (uint16_t)(p_value + i_value);
        if (pid > QEI_MOT1_MAX_RPM){pid = QEI_MOT1_MAX_RPM;}
        if (pid < 0){pid = 0;}
        pid_out = pid;
        pid_flag = 1;
    }
}