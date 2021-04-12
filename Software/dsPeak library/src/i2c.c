//***************************************************************************//
// File      : I2C.c
//
// Functions :  void I2C_init (uint8_t port, uint8_t mode, uint8_t address);
//              void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length);
//              void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length);
//              void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length);
//              uint8_t I2C_wait (uint8_t port);
//              uint8_t I2C_read_state (uint8_t port);
//              uint8_t * I2C_get_rx_buffer (uint8_t port);
//              uint8_t I2C_rx_done (uint8_t port);
//              void I2C_change_address (uint8_t adr);
//              void I2C_clear_rx_buffer (uint8_t port);
//
// Includes  :  i2c.h
//           
// Purpose   :  I2C driver for the dsPIC33EP family
//              2x native channels on dsPeak :
//              I2C_1 : MikroBus I2C port
//              I2C_2 : On-board I2C rheostats for PWM RC-DAC function
//
// Intellitrol                   MPLab X v5.45                        10/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "i2c.h"

STRUCT_I2C i2c_struct[I2C_MODULE_QTY];

//void I2C_init (uint8_t port, uint8_t mode, uint8_t address)//
//Description : Function initializes the i2c module in slave or master mode
//              Since there are 2 I2C modules in the dsPIC, 2 instances of a
//              structure holding all the variables for each module are created
//
//Function prototype : void I2C_init (uint8_t port, uint8_t mode, uint8_t address)
//
//Enter params       : uint8_t port : Specify the I2C port to initialize
//                   : uint8_t mode : Specify the working mode for the I2C port, slave or master
//                   : uint8_t address : Specify the slave address if using the slave mode, otherwise put 0
//
//Exit params        : None
//
//Function call      : I2C_init(I2C_port_1, I2C_mode_slave, 0x22);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_init (uint8_t port, uint8_t mode, uint8_t address)
{   
    // Slave initialization
    if (port == I2C_port_1)
    {   
        // Initialize I2C port struct variables
        i2c_struct[port].i2c_tx_counter = 0;                  // Reset state machine variables
        i2c_struct[port].i2c_rx_counter = 0;                  //
        i2c_struct[port].i2c_write_length = I2C_MSG_LENGTH;   //
        i2c_struct[port].i2c_read_length = I2C_MSG_LENGTH;    //
        i2c_struct[port].i2c_int_counter = 0;                 //
        i2c_struct[port].i2c_done = 0;                        // Bus is free          
        if (mode == I2C_mode_slave)
        {
            // Module register initializations
            IEC1bits.SI2C1IE = 0;       // Disable I2C slave interrupt    
            I2C1CONbits.I2CEN = 0;      // Disable module if it was in use
            I2C1CONbits.DISSLW = 1;     // Disable slew rate control    
            I2C1CONbits.A10M = 0;       // 7 bit address
            I2C1ADD = address >> 1;     // Write I2C address
            I2C1CONbits.DISSLW = 1;     // Disable slew rate control
            I2C1CONbits.STREN = 1;      // Enable user software clock stretching

            // Module interrupt initialization and enable
            IFS1bits.SI2C1IF = 0;       // Clear slave I2C interrupt flag  
            IPC4bits.SI2C1IP = 4;       // Set default priority to 4 
            IEC1bits.SI2C1IE = 1;       // Enable I2C slave interrupt    
            I2C1CONbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function
        }
        
        if (mode == I2C_mode_master)
        {
            I2C1CONbits.DISSLW = 1;     // Disable slew rate control      
            I2C1BRG = I2C_FREQ_100k;    // Set I2C1 brg at 100kHz with Fcy = 66.33MIPS        
            IFS1bits.MI2C1IF = 0;       // Clear master I2C interrupt flag  
            IPC4bits.MI2C1IP = 1;       // Set default priority 
            IEC1bits.MI2C1IE = 0;       // Disable I2C master interrupt    
            I2C1CONbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function           
        }        
    }
    
    if (port == I2C_port_2)
    {        
        // Initialize I2C port struct variables
        i2c_struct[port].i2c_tx_counter = 0;                  // Reset state machine variables
        i2c_struct[port].i2c_rx_counter = 0;                  //
        i2c_struct[port].i2c_write_length = EEPROM_TX_LENGTH;   //
        i2c_struct[port].i2c_read_length = EEPROM_RX_LENGTH;    //
        i2c_struct[port].i2c_int_counter = 0;                 //
        i2c_struct[port].i2c_done = 0;                        // Bus is free          
        if (mode == I2C_mode_slave)
        {
            // Module register initializations
            IEC3bits.SI2C2IE = 0;       // Disable I2C slave interrupt    
            I2C2CONbits.I2CEN = 0;      // Disable module if it was in use
            I2C2CONbits.DISSLW = 1;     // Disable slew rate control    
            I2C2CONbits.A10M = 0;       // 7 bit address
            I2C2ADD = address >> 1;     // Write I2C address
            I2C2CONbits.DISSLW = 1;     // Disable slew rate control
            I2C2CONbits.STREN = 1;      // Enable user software clock stretching

            // Module interrupt initialization and enable
            IFS3bits.SI2C2IF = 0;       // Clear slave I2C interrupt flag  
            IPC12bits.SI2C2IP = 4;      // Set default priority to 4 
            IEC3bits.SI2C2IE = 1;       // Enable I2C slave interrupt    
            I2C2CONbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function
        }
                
        if (mode == I2C_mode_master)
        {
            I2C2CONbits.DISSLW = 1;     // Disable slew rate control      
            I2C2BRG = I2C_FREQ_100k;    // Set I2C1 brg at 400KHz with Fcy = 68.1984MIPS        
            IFS3bits.MI2C2IF = 0;       // Clear master I2C interrupt flag  
            IPC12bits.MI2C2IP = 4;      // Set default priority to 4 
            IEC3bits.MI2C2IE = 0;       // Disable I2C master interrupt    
            I2C2CONbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function           
        }
    }
    __delay_ms(100);
}


//************uint8_t I2C_rx_done (uint8_t port)******************//
//Description : Function returns I2C bus state
//
//Function prototype : uint8_t I2C_rx_done (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : uint8_t : bus state
//
//Function call      : int_state = I2C_rx_done(I2C_port_2);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t I2C_rx_done (uint8_t port)
{
    if (i2c_struct[port].i2c_done)      // A transaction was successfully managed
    {
        i2c_struct[port].i2c_done = 0;  // Clear flag
        return 1;
    }
    else return 0;
}

//****************void I2C_clear_rx_buffer (uint8_t port)******************//
//Description : Function clears I2C receive buffer from specified I2C port
//
//Function prototype : void I2C_clear_rx_buffer (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : None
//
//Function call      : I2C_clear_rx_buffer(I2C_port_1);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_clear_rx_buffer (uint8_t port)
{
    uint8_t i = 0;
    for (i=0; i<I2C_MSG_LENGTH; i++)
    {
        i2c_struct[port].i2c_rx_data[i] = 0;
    }   
}

//void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length)//
//Description : Function fills the I2C transmit buffer on SLAVE ONLY operations 
//
//Function prototype : void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length)
//
//Enter params       : uint8_t port : I2C port number
//                   : uint8_t *ptr : pointer to data array
//                   : uint8_t length : data length in bytes
//
//Exit params        : None
//
//Function call      : I2C_fill_transmit_buffer(I2C_port_1, data, 5);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length)
{
    // Use this function on slave-only transmission
    uint8_t i = 0;
    if (length <= I2C_MSG_LENGTH)       // - 1 since the 1st byte is always the address (slave-only)
    {
        ptr++;                          // skip the 1st byte since its the address (slave-only)
        for (i=0; i< (length - 1); i++)
        {
            i2c_struct[port].i2c_tx_data[i] = *ptr; // Fill the transmit buffer
            ptr++;
        }   
    }
}

//void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length)//
//Description : Function writes an I2C message on the specified port
//              This is an I2C MASTER-only function
//              This is a blocking function
//
//Enter params       : uint8_t port : I2C port number
//                   : uint8_t *data : pointer to data array
//                   : uint8_t length : data length in bytes
//
//Exit params        : None
//
//Function call      : I2C_master_write(I2C_port_2, data, 6);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length)
{
    uint8_t i = 0;
    while(I2C_wait(port));      // Check for disabled interrupt
    for (i=0; i<length; i++)
    {
        i2c_struct[port].i2c_tx_data[i] = *data;    // copy data to struct array
        data++;
    }
    i2c_struct[port].i2c_write_length = length;     // Set message length 
    i2c_struct[port].i2c_message_mode = I2C_WRITE;  // Set message type to write
    i2c_struct[port].i2c_int_counter = 0;           // Reset state machine variables
    i2c_struct[port].i2c_tx_counter = 0;            //
    i2c_struct[port].i2c_done = 1;                  // busy i2c flag set
    
    if (port == I2C_port_1)
    {
        IEC1bits.MI2C1IE = 1;                           // Enable I2C master interrupt 
        I2C1CONbits.SEN = 1;                            // Start I2C sequence
    }
    
    if (port == I2C_port_2)
    {
        IEC3bits.MI2C2IE = 1;                           // Enable I2C master interrupt 
        I2C2CONbits.SEN = 1;                            // Start I2C sequence        
    }
}

//void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length)//
//Description : Function reads an I2C message on the specified port
//              This is an I2C MASTER-only function
//              This is a blocking function
//
//Enter params       : uint8_t port : I2C port number
//                   : uint8_t *data : pointer to data array
//                   : uint8_t w_length : write length in bytes
//                   : uint8_t r_length : read length in bytes
//
//Exit params        : None
//
//Function call      : I2C_read(I2C_port_2, data, 6);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length)
{
    uint8_t i = 0;
    while(I2C_wait(port));                          // Wait until previous transaction is over
    for (i=0; i<w_length; i++)
    {
        i2c_struct[port].i2c_tx_data[i] = *data;// copy data to struct table
        data++;
    }
    i2c_struct[port].i2c_message_mode = I2C_READ;   //  
    i2c_struct[port].i2c_write_length = w_length;   // registers to write before reading
    i2c_struct[port].i2c_read_length = r_length;    // Read adr + number of bytes to read
    i2c_struct[port].i2c_int_counter = 0;           // 
    i2c_struct[port].i2c_tx_counter = 0;            //
    i2c_struct[port].i2c_rx_counter = 0;            //
    i2c_struct[port].i2c_done = 1;                  //busy i2c
    //if (port == I2C_port_1)
    //{
    //    IEC1bits.MI2C1IE = 1;                     // Enable I2C master interrupt 
    //    I2C1CONbits.SEN = 1;                      // Start I2C sequence
    //}
    
    if (port == I2C_port_2)
    {
        IEC3bits.MI2C2IE = 1;                       // Enable I2C master interrupt 
        I2C2CONbits.SEN = 1;                        // Start I2C sequence        
    }
    
    while(I2C_rx_done(port));                       // Wait until transaction over    
}


//************uint8_t I2C_wait (uint8_t port)*********************//
//Description : Wait for I2C interrupt to flag down (bus unused))
//
//Function prototype : uint8_t I2C_wait (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : uint8_t : bus state
//
//Function call      : int_state = I2C_wait(I2C_port_2);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t I2C_wait (uint8_t port)
{
    if (port == I2C_port_1)
    {
        return IEC1bits.MI2C1IE;    // Only used in master mode
    }
    
    else if (port == I2C_port_2)
    {
        return IEC3bits.MI2C2IE;    // Only used in master mode
    }
    
    else return 0;
}

//************uint8_t * I2C_get_rx_buffer (uint8_t port)***************//
//Description : Return I2C array address (1st element)
//
//Function prototype : uint8_t * I2C_get_rx_buffer (void)
//
//Enter params       : None
//
//Exit params        : uint8_t * : address of 1st element
//
//Function call      : ptr = I2C_get_rx_buffer();
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t * I2C_get_rx_buffer (uint8_t port)
{
    return &i2c_struct[port].i2c_rx_data[0];
}


// I2C1 slave interrupt routine 
// Uncomment this section if I2C1 is used as a slave
void __attribute__((__interrupt__, no_auto_psv)) _SI2C1Interrupt(void)
{   
    uint8_t temp = 0;

    // Address + W received, write data to slave
    if ((!I2C1STATbits.D_A) && (!I2C1STATbits.R_W))
    {
        i2c_struct[I2C_port_1].i2c_rx_data[i2c_struct[I2C_port_1].i2c_rx_counter] = I2C1RCV; // Receive address
        while(I2C1STATbits.RBF);    // Wait until receive buffer is empty
        i2c_struct[I2C_port_1].i2c_rx_counter++;// Increment data counter
        I2C1CONbits.SCLREL = 1;     // Release the serial clock     
    }

    // Address + R received, read data from slave
    if ((!I2C1STATbits.D_A) && (I2C1STATbits.R_W))
    {
        temp = I2C1RCV;
        while(I2C1STATbits.RBF);
        while(I2C1STATbits.TBF);
        I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_tx_counter];
        i2c_struct[I2C_port_1].i2c_tx_counter++;  
        I2C1CONbits.SCLREL = 1; // release the SCL line  
    }
    
    //Should send more data to master as long as he acks
    if ((I2C1STATbits.D_A) && (I2C1STATbits.R_W))
    {
        // The master responded with an ACK, transfer more data
        if (!I2C1STATbits.ACKSTAT)
        {
            if (i2c_struct[I2C_port_1].i2c_tx_counter < i2c_struct[I2C_port_1].i2c_write_length)
            {
                temp = I2C1RCV;
                while(I2C1STATbits.RBF);
                while(I2C1STATbits.TBF);
                I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_tx_counter];
                i2c_struct[I2C_port_1].i2c_tx_counter++; 
                I2C1CONbits.SCLREL = 1; // release the SCL line
            }
        } 
        else
        {
            i2c_struct[I2C_port_1].i2c_tx_counter = 0;
            i2c_struct[I2C_port_1].i2c_done = 1;
        }
    }        
       
    // Receiving data from master (data, adress is in write mode)
    if ((I2C1STATbits.D_A) && (!I2C1STATbits.R_W))
    {
        // Receive more data from master
        if (i2c_struct[I2C_port_1].i2c_rx_counter < i2c_struct[I2C_port_1].i2c_read_length)
        {
            i2c_struct[I2C_port_1].i2c_rx_data[i2c_struct[I2C_port_1].i2c_rx_counter] = I2C1RCV;
            while(I2C1STATbits.RBF);
            i2c_struct[I2C_port_1].i2c_rx_counter++;
            I2C1CONbits.SCLREL = 1; // release the SCL line                      
        }
        
        if (i2c_struct[I2C_port_1].i2c_rx_counter == i2c_struct[I2C_port_1].i2c_read_length)
        {
            i2c_struct[I2C_port_1].i2c_done = 1;
            i2c_struct[I2C_port_1].i2c_rx_counter = 0;
        }
    } 
    IFS1bits.SI2C1IF = 0;    
}

// I2C2 master interrupt routine 
// Uncomment these lines if the I2C2 is used as master
void __attribute__((__interrupt__, no_auto_psv)) _MI2C1Interrupt(void)
{
    if (i2c_struct[I2C_port_1].i2c_message_mode == I2C_WRITE) // write
    {
        if (i2c_struct[I2C_port_1].i2c_int_counter == 0)
        {
            IFS1bits.MI2C1IF = 0;   // Lower interrupt flag 
            I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_tx_counter]; //send adr
            i2c_struct[I2C_port_1].i2c_int_counter++;
            i2c_struct[I2C_port_1].i2c_tx_counter++;  
        }            

        else if (i2c_struct[I2C_port_1].i2c_int_counter == 1) // send data
        {
            if (i2c_struct[I2C_port_1].i2c_tx_counter < i2c_struct[I2C_port_1].i2c_write_length)//send data
            {  
                IFS1bits.MI2C1IF = 0;   // Lower interrupt flag  
                I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_tx_counter];
                i2c_struct[I2C_port_1].i2c_tx_counter++;
            }

            else //all the data was transmitted
            {
                IFS1bits.MI2C1IF = 0;                           // Lower interrupt flag 
                i2c_struct[I2C_port_1].i2c_int_counter++;     
                I2C1CONbits.PEN = 1;                            //enable stop sequence
            }
        }    

        else if (i2c_struct[I2C_port_1].i2c_int_counter == 2)   //end transmission 
        {
            i2c_struct[I2C_port_1].i2c_int_counter = 0;         // Reset interrupt counter
            i2c_struct[I2C_port_1].i2c_tx_counter = 0;          // Reset transmit counter
            i2c_struct[I2C_port_1].i2c_done = 0;                // Bus is free, transaction over
            IEC1bits.MI2C1IE = 0;                               // Disable SSP interrupt
            IFS1bits.MI2C1IF = 0;                               // Lower interrupt flag
        }
    }   // End of I2C_write interrupt
    
    // 24LCXX read scheme   : Write adr + read bit disabled
    //                      : Write memory location to read
    //                      : Repeated start on I2C bus
    //                      : Write adr + read bit enabled
    //                      : Read then store byte
    else if (i2c_struct[I2C_port_1].i2c_message_mode == I2C_READ) // read
    {
        if (i2c_struct[I2C_port_1].i2c_int_counter == 0)    // send adr 
        {
            IFS1bits.MI2C1IF = 0; 
            I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_tx_counter];
            i2c_struct[I2C_port_1].i2c_int_counter++;
            i2c_struct[I2C_port_1].i2c_tx_counter++;
        }    

        else if (i2c_struct[I2C_port_1].i2c_int_counter == 1) //send register to read
        {
            IFS1bits.MI2C1IF = 0; 
            if (i2c_struct[I2C_port_1].i2c_tx_counter < i2c_struct[I2C_port_1].i2c_write_length)
            {
                I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_tx_counter];
                i2c_struct[I2C_port_1].i2c_tx_counter++;
            }
            
            else   
            {
                I2C1CONbits.RSEN = 1; //enable restart
                i2c_struct[I2C_port_1].i2c_int_counter++;
            }   
        }

        // Send read address
        else if (i2c_struct[I2C_port_1].i2c_int_counter == 2)
        {
            IFS1bits.MI2C1IF = 0; 
            I2C1TRN = i2c_struct[I2C_port_1].i2c_tx_data[i2c_struct[I2C_port_1].i2c_rx_counter] + 1; //send adr + 1
            i2c_struct[I2C_port_1].i2c_int_counter++;             
        }

        // Enable data read on I2C bus
        else if (i2c_struct[I2C_port_1].i2c_int_counter == 3)
        {
            IFS1bits.MI2C1IF = 0; 
            I2C1CONbits.RCEN = 1;
            i2c_struct[I2C_port_1].i2c_int_counter++; 
        }    
        
        // Read data from i2c slave
        else if (i2c_struct[I2C_port_1].i2c_int_counter == 4)
        {
            IFS1bits.MI2C1IF = 0; 
            if (i2c_struct[I2C_port_1].i2c_rx_counter < i2c_struct[I2C_port_1].i2c_read_length)
            {   
                // Get byte and store in rx buffer
                i2c_struct[I2C_port_1].i2c_rx_data[i2c_struct[I2C_port_1].i2c_rx_counter] = I2C1RCV;
                
                // See if we wish to receive more data as set in i2c_read_length field
                if (++i2c_struct[I2C_port_1].i2c_rx_counter < i2c_struct[I2C_port_1].i2c_read_length)
                {          
                    I2C1CONbits.ACKDT = 0; //send ack
                    I2C1CONbits.ACKEN = 1; //continue data reception
                    i2c_struct[I2C_port_1].i2c_int_counter = 4;
                }   
                
                // No more data to receive, send NACK to slave device
                else
                {
                    I2C1CONbits.ACKDT = 1; //send nack
                    I2C1CONbits.ACKEN = 1;
                }   
            } 

            // Stop I2C transaction
            else
            {
                I2C1CONbits.PEN = 1;
                i2c_struct[I2C_port_1].i2c_int_counter++; 
            }            
        }

        else if (i2c_struct[I2C_port_1].i2c_int_counter == 5)
        {
            i2c_struct[I2C_port_1].i2c_int_counter = 0;
            i2c_struct[I2C_port_1].i2c_rx_counter = 0;
            i2c_struct[I2C_port_1].i2c_tx_counter = 0;
            i2c_struct[I2C_port_1].i2c_done = 0;
            IEC1bits.MI2C1IE = 0;
            IFS1bits.MI2C1IF = 0;
        }                             
    }
}

// I2C2 master interrupt routine 
// Uncomment these lines if the I2C2 is used as master
void __attribute__((__interrupt__, no_auto_psv)) _MI2C2Interrupt(void)
{
    if (i2c_struct[I2C_port_2].i2c_message_mode == I2C_WRITE) // write
    {
        if (i2c_struct[I2C_port_2].i2c_int_counter == 0)
        {
            IFS3bits.MI2C2IF = 0;   // Lower interrupt flag 
            I2C2TRN = i2c_struct[I2C_port_2].i2c_tx_data[i2c_struct[I2C_port_2].i2c_tx_counter]; //send adr
            i2c_struct[I2C_port_2].i2c_int_counter++;
            i2c_struct[I2C_port_2].i2c_tx_counter++;  
        }            

        else if (i2c_struct[I2C_port_2].i2c_int_counter == 1) // send data
        {
            if (i2c_struct[I2C_port_2].i2c_tx_counter < i2c_struct[I2C_port_2].i2c_write_length)//send data
            {  
                IFS3bits.MI2C2IF = 0;   // Lower interrupt flag  
                I2C2TRN = i2c_struct[I2C_port_2].i2c_tx_data[i2c_struct[I2C_port_2].i2c_tx_counter];
                i2c_struct[I2C_port_2].i2c_tx_counter++;
            }

            else //all the data was transmitted
            {
                IFS3bits.MI2C2IF = 0;                           // Lower interrupt flag 
                i2c_struct[I2C_port_2].i2c_int_counter++;     
                I2C2CONbits.PEN = 1;                            //enable stop sequence
            }
        }    

        else if (i2c_struct[I2C_port_2].i2c_int_counter == 2)   //end transmission 
        {
            i2c_struct[I2C_port_2].i2c_int_counter = 0;         // Reset interrupt counter
            i2c_struct[I2C_port_2].i2c_tx_counter = 0;          // Reset transmit counter
            i2c_struct[I2C_port_2].i2c_done = 0;                // Bus is free, transaction over
            IEC3bits.MI2C2IE = 0;                               // Disable SSP interrupt
            IFS3bits.MI2C2IF = 0;                               // Lower interrupt flag
        }
    }   // End of I2C_write interrupt
    
    // 24LCXX read scheme   : Write adr + read bit disabled
    //                      : Write memory location to read
    //                      : Repeated start on I2C bus
    //                      : Write adr + read bit enabled
    //                      : Read then store byte
    else if (i2c_struct[I2C_port_2].i2c_message_mode == I2C_READ) // read
    {
        if (i2c_struct[I2C_port_2].i2c_int_counter == 0)    // send adr 
        {
            IFS3bits.MI2C2IF = 0; 
            I2C2TRN = i2c_struct[I2C_port_2].i2c_tx_data[i2c_struct[I2C_port_2].i2c_tx_counter];
            i2c_struct[I2C_port_2].i2c_int_counter++;
            i2c_struct[I2C_port_2].i2c_tx_counter++;
        }    

        else if (i2c_struct[I2C_port_2].i2c_int_counter == 1) //send register to read
        {
            IFS3bits.MI2C2IF = 0; 
            if (i2c_struct[I2C_port_2].i2c_tx_counter < i2c_struct[I2C_port_2].i2c_write_length)
            {
                I2C2TRN = i2c_struct[I2C_port_2].i2c_tx_data[i2c_struct[I2C_port_2].i2c_tx_counter];
                i2c_struct[I2C_port_2].i2c_tx_counter++;
            }
            
            else   
            {
                I2C2CONbits.RSEN = 1; //enable restart
                i2c_struct[I2C_port_2].i2c_int_counter++;
            }   
        }

        // Send read address
        else if (i2c_struct[I2C_port_2].i2c_int_counter == 2)
        {
            IFS3bits.MI2C2IF = 0; 
            I2C2TRN = i2c_struct[I2C_port_2].i2c_tx_data[i2c_struct[I2C_port_2].i2c_rx_counter] + 1; //send adr + 1
            i2c_struct[I2C_port_2].i2c_int_counter++;             
        }

        // Enable data read on I2C bus
        else if (i2c_struct[I2C_port_2].i2c_int_counter == 3)
        {
            IFS3bits.MI2C2IF = 0; 
            I2C2CONbits.RCEN = 1;
            i2c_struct[I2C_port_2].i2c_int_counter++; 
        }    
        
        // Read data from i2c slave
        else if (i2c_struct[I2C_port_2].i2c_int_counter == 4)
        {
            IFS3bits.MI2C2IF = 0; 
            if (i2c_struct[I2C_port_2].i2c_rx_counter < i2c_struct[I2C_port_2].i2c_read_length)
            {   
                // Get byte and store in rx buffer
                i2c_struct[I2C_port_2].i2c_rx_data[i2c_struct[I2C_port_2].i2c_rx_counter] = I2C2RCV;
                
                // See if we wish to receive more data as set in i2c_read_length field
                if (++i2c_struct[I2C_port_2].i2c_rx_counter < i2c_struct[I2C_port_2].i2c_read_length)
                {          
                    I2C2CONbits.ACKDT = 0; //send ack
                    I2C2CONbits.ACKEN = 1; //continue data reception
                    i2c_struct[I2C_port_2].i2c_int_counter = 4;
                }   
                
                // No more data to receive, send NACK to slave device
                else
                {
                    I2C2CONbits.ACKDT = 1; //send nack
                    I2C2CONbits.ACKEN = 1;
                }   
            } 

            // Stop I2C transaction
            else
            {
                I2C2CONbits.PEN = 1;
                i2c_struct[I2C_port_2].i2c_int_counter++; 
            }            
        }

        else if (i2c_struct[I2C_port_2].i2c_int_counter == 5)
        {
            i2c_struct[I2C_port_2].i2c_int_counter = 0;
            i2c_struct[I2C_port_2].i2c_rx_counter = 0;
            i2c_struct[I2C_port_2].i2c_tx_counter = 0;
            i2c_struct[I2C_port_2].i2c_done = 0;
            IEC3bits.MI2C2IE = 0;
            IFS3bits.MI2C2IF = 0;
        }                             
    }
}
