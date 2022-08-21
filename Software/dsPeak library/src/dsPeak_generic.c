#include "dspeak_generic.h"
STRUCT_BUTTON BUTTON_struct[BTN_QTY];
STRUCT_LED LED_struct[LED_QTY];

void dsPeak_init(void)
{ 
    INTCON1bits.NSTDIS = 0;                         // Nested interrupt enabled 
    RCONbits.SWDTEN=0;                              // Watchdog timer disabled 
    
    dsPeak_posc_20MHz_init();
    dsPeak_auxosc_48MHz_init();
    
    // At power-up, disable all analog pins
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELG = 0;
}

// This function setups dsPeak to 140MHz (70MIPS) operation based on the 20MHz
// oscillator soldered on dsPeak.
// Note : the PLL input frequency is normally limited to F(in) < 13.6MHz based
// on the power-on reset values of the PLL pre and postscalers.
// Since the on-board oscillator is 20MHz, dsPeak must first use the FRC
// oscillator at start-up before configuring the PLL and then perform a clock
// switch operation.
// Make sure the configuration fuses for the POSC are as follow :
// FOSCSEL
// #pragma config FNOSC = FRC              // Primary oscillator at POR is FRC
// #pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)
// FOSC
// #pragma config POSCMD = HS              // Primary Oscillator Mode Select Bit (HS Crystal Oscillator Mode)
// Refer to dsPIC33E FRM - Oscillator (DS70850C) p.23 figure 7-8 for PLL
// limitations for the M, N1 and N2 values
void dsPeak_posc_20MHz_init (void)
{
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    // Input HS OSC 20MHz. Output required is 140MHz for 70MIPS  
    // Configure PLL prescaler (N1), PLL postscaler (N2), PLL divisor (M)
    // Where Fout = Fin * (M / (N1 x N2))
    // M = PLLDIV + 2
    // N1 = PLLPRE + 2
    // N2 = 2 x (PLLPOST + 1)
    //
    // For 140MHz operation, the PLL pre and postscaler values are :
    // PLLPRE = 3,  N1 = 5
    // PLLDIV = 68, M = 70
    // PLLPOST = 0, N2 = 2
    // Fout = 20MHz * (70 / (5*2)) = 140MHz = 70MIPS
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
}

// This function setups dsPeak to 132.66MHz operation based on the 7.37MHz FRC 
// oscillator internal to the dsPIC33EPXXXMU814 MCU
// Make sure the configuration fuses for FRC oscillator are as follows :
// FOSCSEL
// #pragma config FNOSC = FRCPLL           // Initial Oscillator Source Selection bits (Internal Fast RC with PLL (FRCPLL))
// #pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)
// FOSC
// #pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
// Refer to dsPIC33E FRM - Oscillator (DS70850C) p.23 figure 7-8 for PLL
// limitations for the M, N1 and N2 values
void dsPeak_posc_FRC_init (void)
{
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    // Input FRC 7.37MHz. Output required is ~140MHz for ~70MIPS  
    // Configure PLL prescaler (N1), PLL postscaler (N2), PLL divisor (M)
    // Where Fout = Fin * (M / (N1 x N2))
    // M = PLLDIV + 2
    // N1 = PLLPRE + 2
    // N2 = 2 x (PLLPOST + 1)
    //
    // For ~140MHz operation, the PLL pre and postscaler values are :
    // PLLPRE = 0,  N1 = 2
    // PLLDIV = 70, M = 72
    // PLLPOST = 0, N2 = 2
    // Fout = 7.37MHz * (72 / (2*2)) = 132.66MHz = 66.33MIPS    
    PLLFBDbits.PLLDIV = 70;     // M = 72
    CLKDIVbits.PLLPRE = 0;      // N1=2
    CLKDIVbits.PLLPOST = 0;     // N2=2
    
    // Initiate Clock Switch to FRC oscillator with PLL (NOSC=0b001)
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b001);
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);    
}

// This function enables the SOSC circuitry inside the dsPIC. It will use the
// 32.768kHz low-power crystal on dsPeak to drive the Real-time clock circuit
void dsPeak_sosc_32kHz_init (void)
{
    __builtin_write_OSCCONL(OSCCON | 0x02); // Enable secondary oscillator
    while(OSCCONbits.LPOSCEN != 1);         // Wait until oscillator ready
}

// This function enables the AUXOSC for USB operation @ 48MHz
void dsPeak_auxosc_48MHz_init (void)
{
    // Fosc = Fin * (M / (N1 x N2))
    // M = PLLDIV + 2
    // N1 = PLLPRE + 2
    // N2 = 2 x (PLLPOST + 1)
    //    
    // For 140MHz operation, the PLL pre and postscaler values are :
    // PLLPRE = 3,  N1 = 5
    // PLLDIV = 22, M = 24
    // PLLPOST = 0, N2 = 2
    // Fout = 20MHz * (72 / (7*4)) = 140MHz = 70MIPS
    
    ACLKCON3 = 0;
    ACLKCON3bits.APLLPRE = 3;       // Divide PLL input by 2
    ACLKCON3bits.APLLPOST = 0;      // Divide PLL output by 2
    ACLKCON3bits.ASRCSEL = 1;       // Aux PLL source is primary oscillator
    ACLKCON3bits.SELACLK = 1;       // Input to PLL divider is PLL output
    ACLKDIV3 = 22;                  // PLL multiply by 24
    ACLKCON3bits.ENAPLL = 1;        // Enable the Aux PLL
    while(ACLKCON3bits.APLLCK != 1);// Wait for it lock    
}

uint8_t dsPeak_button_init (STRUCT_BUTTON *btn, uint8_t channel, uint16_t debounce_thres)
{
    switch (channel)
    {
        case BTN_1:
            DSPEAK_BTN1_DIR = INPUT;
            break;
            
        case BTN_2:
            DSPEAK_BTN2_DIR = INPUT;
            break;
            
        case BTN_3:
            DSPEAK_BTN3_DIR = INPUT;
            break;
            
        case BTN_4:
            DSPEAK_BTN4_DIR = INPUT;
            break;
            
        default:
            return 0;
            break;
    }
    btn->channel = channel;
    btn->debounce_thres = debounce_thres;
    btn->debounce_cnt = 1;
    btn->state = HIGH;          // Default state is high because of the on-board pull-up
    btn->prev_state = HIGH;     // Default state is high because of the on-board pull-up
    btn->debounce_state = HIGH; // Default state is high because of the on-board pull-up
    btn->do_once = 0;
    return 1;
}

uint8_t dsPeak_button_debounce (STRUCT_BUTTON *btn)
{
    switch (btn->channel)
    {
        case BTN_1:
            btn->prev_state = btn->state;
            btn->state = DSPEAK_BTN1_STATE;
            break;
            
        case BTN_2:
            btn->prev_state = btn->state;
            btn->state = DSPEAK_BTN2_STATE;
            break;
            
        case BTN_3:
            btn->prev_state = btn->state;
            btn->state = DSPEAK_BTN3_STATE;
            break;
            
        case BTN_4:
            btn->prev_state = btn->state;
            btn->state = DSPEAK_BTN4_STATE;
            break;
            
        default: 
            return 0;
            break;
    }
    
    // Button is held low
    if ((btn->state == btn->prev_state) && (btn->state == LOW))
    {
        if (++btn->debounce_cnt > btn->debounce_thres)
        {
            btn->debounce_cnt = btn->debounce_thres;
            btn->debounce_state = LOW;
        }
    }
    else
    {
        if (--btn->debounce_cnt < 1)
        {
            btn->debounce_cnt = 1;
            btn->debounce_state = HIGH;
        }
    }
    return 1;
}

uint8_t dsPeak_button_get_state (STRUCT_BUTTON *btn)
{
    if ((btn->channel >= 0) && (btn->channel < BTN_QTY))
    {
        return btn->debounce_state;
    }
    else
        return HIGH;
}

uint8_t dsPeak_led_init (STRUCT_LED *led, uint8_t channel, uint8_t value)
{
    switch (channel)
    {
        case LED_1:
            DSPEAK_LED1_DIR = OUTPUT;
            DSPEAK_LED1_STATE = value;
            break;
            
        case LED_2:
            DSPEAK_LED2_DIR = OUTPUT;
            DSPEAK_LED2_STATE = value;
            break;
            
        case LED_3:
            DSPEAK_LED3_DIR = OUTPUT;
            DSPEAK_LED3_STATE = value;
            break;
            
        case LED_4:
            DSPEAK_LED4_DIR = OUTPUT;
            DSPEAK_LED4_STATE = value;
            break;
            
        default:
            return 0;
            break;
    }
    led->channel = channel;
    led->state = value;          // Default state is high because of the on-board pull-up
    led->prev_state = value;     // Default state is high because of the on-board pull-up    
    return 1;
}

uint8_t dsPeak_led_write (STRUCT_LED *led, uint8_t value)
{
    switch (led->channel)
    {
        case LED_1:
            DSPEAK_LED1_STATE = value;
            break;
            
        case LED_2:
            DSPEAK_LED2_STATE = value;
            break;
            
        case LED_3:
            DSPEAK_LED3_STATE = value;
            break;
            
        case LED_4:
            DSPEAK_LED4_STATE = value;
            break;
            
        default:
            return 0;
            break;
    }    
    led->prev_state = led->state;
    led->state = value;
    return 1;
}

void hex_to_ascii (uint8_t in, uint8_t *out_h, uint8_t *out_l)
{
    *out_h = ((in & 0xF0)>>4);	   
    *out_l = (in & 0x0F);
    if ((*out_h < 10) && (*out_h >= 0)) 
    {
        *out_h = *out_h + 0x30;			
    }
    
    else if ((*out_h >= 0x0A) && (*out_h <= 0x0F))
    {
        *out_h = (*out_h + 0x37);		
    }
    
    if ((*out_l < 10) && (*out_l >= 0))	
    {
        *out_l = (*out_l + 0x30);		
    }
    
    else if ((*out_l >= 0x0A) && (*out_l <= 0x0F))
    {
        *out_l = (*out_l + 0x37);
    }   
}

int16_t bcd_to_decimal(uint8_t bcd)
{
    return bcd - 6 * (bcd >> 4);
}

uint16_t dec_to_bcd(uint16_t dec)
{
    uint16_t ones = 0;
    uint16_t tens = 0;
    uint16_t temp = 0;

    ones = dec%10;
    temp = dec/10;
    tens = temp<<4;
    return (tens + ones);
}

uint8_t hex_to_dec (uint8_t hex)
{
    uint8_t val=0;
    uint8_t decimal=0;
    if(hex>='0' && hex<='9')  
    {  
        val = hex - 48;  
    }  
    else if(hex>='a' && hex<='f')  
    {  
        val = hex - 97 + 10;  
    }  
    else if(hex>='A' && hex<='F')  
    {  
        val = hex - 65 + 10;  
    }
    decimal = val * pow(16, 1);  
    return decimal;    
}

void uint16_to_byte8 (uint16_t *ptr_16, uint8_t *ptr_8)
{
    uint16_t i = 0;
    for (; i < 128; i++)
    {
        *ptr_8++ = ((*ptr_16 & 0xFF00) >> 8);
        *ptr_8++ = (*ptr_16++ & 0x00FF);
    }
}

void byte8_to_uint16 (uint8_t *ptr_8, uint16_t *ptr_16)
{
    uint16_t i = 0;
    //uint16_t temp = 0;
    for (; i<128; i++)
    { 
        *ptr_16 = ((*ptr_8++ << 8) & 0xFF00);
        *ptr_16 = (*ptr_16 | *ptr_8++);
        ptr_16++;
    }
}