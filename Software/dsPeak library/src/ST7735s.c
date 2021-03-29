#include "ST7735s.h"

void ST7735_init (void)
{
    LCD_RESET_LATCH = 1;
    __delay_ms(50);
    LCD_RESET_LATCH = 0;
    __delay_ms(50);
    LCD_RESET_LATCH = 1;
    __delay_ms(50);
    
	ST7735_write_command(0x11); //Sleep out 
	__delay_ms(120);               //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
//------------------------------------ST7715R Frame rate-----------------------------------------//	       	
 	ST7735_write_command(0xb1);		//Frame rate 80Hz
	ST7735_write_data(0x02);	       
	ST7735_write_data(0x35);
	ST7735_write_data(0x36);
	
	ST7735_write_command(0xb2);		//Frame rate 80Hz
	ST7735_write_data(0x02);	         
	ST7735_write_data(0x35);
	ST7735_write_data(0x36);
	
	ST7735_write_command(0xb3);		//Frame rate 80Hz
	ST7735_write_data(0x02);	         
	ST7735_write_data(0x35);
	ST7735_write_data(0x36);
	ST7735_write_data(0x02);	         
	ST7735_write_data(0x35);
	ST7735_write_data(0x36);
//------------------------------------End ST7715R Frame rate-----------------------------------------//	
	ST7735_write_command(0xb4);		//Column inversion
	ST7735_write_data(0x07);	         
//------------------------------------ST7715R Power Sequence-----------------------------------------//	
	ST7735_write_command(0xc0);		//
	ST7735_write_data(0xa2);	         
	ST7735_write_data(0x02);//02
	ST7735_write_data(0x84);//84
	
	ST7735_write_command(0xc1);		//
	ST7735_write_data(0x45);	         
	
	ST7735_write_command(0xc2);		//
	ST7735_write_data(0x0a);	         
	ST7735_write_data(0x00);
	
	ST7735_write_command(0xc3);		//
	ST7735_write_data(0x8a);	         
	ST7735_write_data(0x2a);
	
	ST7735_write_command(0xc4);		//
	ST7735_write_data(0x8a);	         
	ST7735_write_data(0xee);
//---------------------------------End ST7715R Power Sequence-------------------------------------//
	
	ST7735_write_command(0xc5);		//VCOM
	ST7735_write_data(0xf);	         
	
	ST7735_write_command(0x36);		//
	ST7735_write_data(0xC8);	         
//------------------------------------ST7715R Gamma Sequence-----------------------------------------//
	
	ST7735_write_command(0xe0);		//
	ST7735_write_data(0x12);	         
	ST7735_write_data(0x1c);
	ST7735_write_data(0x10);
	ST7735_write_data(0x18);	         
	ST7735_write_data(0x33);
	ST7735_write_data(0x2c);
	ST7735_write_data(0x25);	         
	ST7735_write_data(0x28);
	ST7735_write_data(0x28);
	ST7735_write_data(0x27);	         
	ST7735_write_data(0x2f);
	ST7735_write_data(0x3c);
	ST7735_write_data(0x00);	         
	ST7735_write_data(0x03);
	ST7735_write_data(0x03);
	ST7735_write_data(0x10);	         
	
	ST7735_write_command(0xe1);		//
	ST7735_write_data(0x12);	         
	ST7735_write_data(0x1c);
	ST7735_write_data(0x10);
	ST7735_write_data(0x18);	         
	ST7735_write_data(0x2d);
	ST7735_write_data(0x28);
	ST7735_write_data(0x23);	         
	ST7735_write_data(0x28);
	ST7735_write_data(0x28);
	ST7735_write_data(0x26);	         
	ST7735_write_data(0x2f);
	ST7735_write_data(0x3d);
	ST7735_write_data(0x00);	         
	ST7735_write_data(0x03);
	ST7735_write_data(0x03);
	ST7735_write_data(0x10);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	ST7735_write_command(0x3A); //65k mode 
	ST7735_write_data(0x05); 
	ST7735_write_command(0x29); //Display on 
	ST7735_write_command(0x2C);    
}

void ST7735_write_data (uint8_t data)
{
    LCD_DAT_CMD_LATCH = 1;
    PMP_write_single(PMP_MODE_TFT, 0, data);
}

void ST7735_write_command (uint8_t command)
{
    LCD_DAT_CMD_LATCH = 0;
    PMP_write_single(PMP_MODE_TFT, 0, command);
}

void ST7735_write_two_data (uint16_t y)
{
    ST7735_write_data((y & 0xFF00)>>8);
    ST7735_write_data(y);
}

void ST7735_SetPos(uint8_t x0,uint8_t x1,uint8_t y0,uint8_t y1)
{
	x0+=2;x1+=2;y0+=67;y1+=67;

 	ST7735_write_command(0x2A);
	ST7735_write_data (0x00);
	ST7735_write_data (x0);
	ST7735_write_data (0x00);
	ST7735_write_data (x1);
	ST7735_write_command(0x2B);
	ST7735_write_data (0x00);
	ST7735_write_data (y0);
	ST7735_write_data (0x00);
	ST7735_write_data (y1);
	ST7735_write_command(0x2C);//LCD_WriteCMD(GRAMWR);    
}

void ST7735_Clear(uint16_t bColor)
{
    uint8_t i, j;
    ST7735_SetPos(0, 127, 0, 63);
    for (i=0;i<64;i++)
    {
        for (j=0;j<128;j++)
        {
            ST7735_write_two_data(bColor);
        }
    }
}    

uint16_t RGB888_to_RGB565 (uint32_t color)
{
    //uint8_t red = ((color & 0xFF0000) >> 16);
    //uint8_t green = ((color & 0x00FF00) >> 8);
    //uint8_t blue = color;
    //uint16_t RGB565 = 0;
    //RGB565 = (((((color & 0xFF0000) >> 16) & 0xf8)<<8) + ((((color & 0x00FF00) >> 8) & 0xfc)<<3)+(color>>3));
    return (uint16_t)((((color & 0xF80000) >> 16)<<8) + (((color & 0x00FC00) >> 8)<<3)+(color>>3));
    //return RGB565;
}