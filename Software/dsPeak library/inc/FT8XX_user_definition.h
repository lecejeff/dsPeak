/*
 * File:   __FT8XX_USER_DEFINITION_H__
 * Author: Intellitrol
 *
 * Created on 28 juin 2016, 09:13
 */

#ifndef __FT8XX_user_definition_H_
#define __FT8XX_user_definition_H_

// Physical mapping of the FT8XX GPIOs excluding the SPI port
#define FT8XX_nPD_PIN LATBbits.LATB10
#define FT8XX_nINT_PIN PORTBbits.RB9

//#define MODE_I2C          //define physical layer
#define MODE_SPI

#define RIVERDI_480x272_RESISTIVE_FT800
#ifdef RIVERDI_480x272_RESISTIVE_FT800
    #define TOUCH_PANEL_RESISTIVE
#endif

//#define RIVERDI_800x480_CAPACITIVE_FT813
#ifdef RIVERDI_800x480_CAPACITIVE_FT813
    #define TOUCH_PANEL_CAPACITIVE 
#endif

// Definitions of the amount of primitives to compile
#define MAX_STR_LEN 99
#define MAX_RECT_NB 0
#define MAX_GRADIENT_NB 1
#define MAX_WINDOW_NB 0
#define MAX_SLIDER_NB 0
#define MAX_BUTTON_NB 18
#define MAX_TEXT_NB 32
#define MAX_NUMBER_NB 17
#define MAX_TOGGLE_NB 5
#define MAX_DIAL_NB 0
#define MAX_PROGRESS_NB 0
#define MAX_SCROLLBAR_NB 5
#define MAX_CLOCK_NB 1
#define MAX_GAUGE_NB 1
#define MAX_KEYS_NB 0

#if MAX_GRADIENT_NB > 0
#define GRADIENT_X0     0
#define GRADIENT_Y0     1
#define GRADIENT_RGB0   2
#define GRADIENT_X1     3
#define GRADIENT_Y1     4
#define GRADIENT_RGB1   5
typedef struct
{
    unsigned int x0;
    unsigned int y0;
    unsigned long rgb0;
    unsigned int x1;
    unsigned int y1;
    unsigned long rgb1;
    unsigned char touch_tag;
}STGradient;
extern STGradient st_Gradient[MAX_GRADIENT_NB];
#endif

#if MAX_KEYS_NB > 0
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    unsigned int f;
    unsigned int opt;
    unsigned char len;
    unsigned char touch_tag;
    char str[MAX_STR_LEN];
}STKeys;
extern STKeys st_Keys[MAX_KEYS_NB];
#endif

#if MAX_SCROLLBAR_NB > 0
#define SCROLLBAR_X         0
#define SCROLLBAR_Y         1
#define SCROLLBAR_WIDTH     2
#define SCROLLBAR_HEIGHT    3
#define SCROLLBAR_OPT       4
#define SCROLLBAR_VAL       5
#define SCROLLBAR_SIZE      6
#define SCROLLBAR_RANGE     7
typedef struct
{
   unsigned int x;
   unsigned int y;
   unsigned int w;
   unsigned int h;
   unsigned int opt;
   unsigned int val;
   unsigned int size;
   unsigned int range;
   unsigned char touch_tag;
}STScrollbar;
extern STScrollbar st_Scrollbar[MAX_SCROLLBAR_NB]; // scroller struct initialization
#endif

#if MAX_DIAL_NB > 0
typedef struct
{
   unsigned int x;
   unsigned int y;
   unsigned int r;
   unsigned int opt;
   unsigned int val;
   unsigned char touch_tag;
}STDial;
extern STDial st_Dial[MAX_DIAL_NB]; // dial struct initialization
#endif

#if MAX_GAUGE_NB > 0
#define GAUGE_X         0
#define GAUGE_Y         1
#define GAUGE_RADIUS    2
#define GAUGE_OPT       3
#define GAUGE_MAJ       4
#define GAUGE_MIN       5
#define GAUGE_VAL       6
#define GAUGE_RANGE     7
typedef struct
{
   unsigned int x;
   unsigned int y;
   unsigned int r;
   unsigned int opt;
   unsigned int maj;
   unsigned int min;
   unsigned int val;
   unsigned int range;
   unsigned char touch_tag;
}STGauge;
extern STGauge st_Gauge[MAX_GAUGE_NB]; // gauge struct initialization
#endif

#if MAX_PROGRESS_NB > 0
typedef struct
{
   unsigned int x;
   unsigned int y;
   unsigned int w;
   unsigned int h;
   unsigned int opt;
   unsigned int val;
   unsigned int range;
   unsigned char touch_tag;
}STProgress; 
extern STProgress st_Progress[MAX_PROGRESS_NB]; // progress struct initialization
#endif

#if MAX_CLOCK_NB > 0
typedef struct
{
   unsigned int x;
   unsigned int y;
   unsigned int r;
   unsigned int opt;
   unsigned char h;
   unsigned char m;
   unsigned char s;
   unsigned char ms;
   unsigned char touch_tag;
}STClock;
extern STClock st_Clock[MAX_CLOCK_NB]; // clock struct initialization 
#endif   

#if MAX_TOGGLE_NB > 0
typedef struct
{
   unsigned int x1;
   unsigned int y1;
   unsigned int w;
   unsigned int f;
   unsigned int opt;
   unsigned char len;
   char str[MAX_STR_LEN];
   unsigned int state;
   unsigned char touch_tag;
}STToggle;
extern STToggle st_Toggle[MAX_TOGGLE_NB]; // toggle struct initialization
#endif  

#if MAX_RECT_NB > 0
typedef struct
{
    unsigned int x1;
    unsigned int y1;
    unsigned int x2;
    unsigned int y2;
    unsigned int w;
    unsigned char touch_tag;
}STRectangle;
extern STRectangle st_Rectangle[MAX_RECT_NB]; // rectangle struct initialization 
#endif

#if MAX_BUTTON_NB > 0
#define BUTTON_X        0
#define BUTTON_Y        1
#define BUTTON_W        2
#define BUTTON_H        3
#define BUTTON_FONT     4
#define BUTTON_OPT      5
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    unsigned int font;
    unsigned int opt;
    unsigned char len;
    unsigned char state;
    char str[MAX_STR_LEN];
    unsigned char touch_tag;
}STButton;
extern STButton st_Button[MAX_BUTTON_NB]; // button struct initialization
#endif

#if MAX_NUMBER_NB > 0
#define NUMBER_X        0
#define NUMBER_Y        1
#define NUMBER_FONT     2
#define NUMBER_OPT      3
#define NUMBER_VAL      4
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int font;
    unsigned int opt;
    unsigned long num;
    unsigned char touch_tag;
}STNumber;
extern STNumber st_Number[MAX_NUMBER_NB]; // number struct initialization
#endif

#if MAX_TEXT_NB > 0
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int font;
    unsigned int opt;
    unsigned char len;
    char str[MAX_STR_LEN];
    unsigned char touch_tag;
}STText;
extern STText st_Text[MAX_TEXT_NB]; // text struct initialization
#endif

#if MAX_SLIDER_NB > 0
#define SLIDER_X        0
#define SLIDER_Y        1
#define SLIDER_W        2
#define SLIDER_H        3
#define SLIDER_OPT      4
#define SLIDER_VAL      5
#define SLIDER_RANGE    6
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    unsigned int opt;
    unsigned int val;
    unsigned int range;
    unsigned char touch_tag;
}STSlider;
extern STSlider st_Slider[MAX_SLIDER_NB]; // slider struct initialization
#endif

#if MAX_WINDOW_NB > 0
typedef struct
{
   unsigned char ucNewState;
   unsigned char ucOldState;
   unsigned char ucCntr;
   unsigned char ucTouchGood;
   unsigned int x1;
   unsigned int y1;
   unsigned int x2;
   unsigned int y2; 
   unsigned char ucReadOK; 
   unsigned char one_touch;
   unsigned char touch_tag;
}STWindow;
extern STWindow st_Window[MAX_WINDOW_NB]; // window struct initialization
#endif

typedef struct
{
    unsigned int X0;
    unsigned int Y0;
    unsigned int X1;
    unsigned int Y1;
    unsigned int X2;
    unsigned int Y2;
    unsigned int X3;
    unsigned int Y3;
    unsigned int X4;
    unsigned int Y4;    
}STTouch;
extern STTouch Touch_data;
//

#define FONT_16 16
#define FONT_17 17
#define FONT_18 18
#define FONT_19 19
#define FONT_20 20
#define FONT_21 21
#define FONT_22 22
#define FONT_23 23
#define FONT_24 24
#define FONT_25 25
#define FONT_26 26
#define FONT_27 27
#define FONT_28 28
#define FONT_29 29
#define FONT_30 30
#define FONT_31 31

#define TOGGLE_ON 0xFFFF
#define TOGGLE_OFF 0
#define FT_TP_TEXT 0
#define NO_RANGE 0
#define MID_RANGE 50
#define FULL_RANGE 100

//
#define FT_PRIM_TOGGLE 0
#define FT_PRIM_BUTTON 1
#define FT_PRIM_TEXT 2
#define FT_PRIM_NUMBER 3
#define FT_PRIM_SLIDER 4
#define FT_PRIM_GRADIENT 5
#define FT_PRIM_KEYS 6
#define FT_PRIM_PROGRESS 7
#define FT_PRIM_SCROLLBAR 8
#define FT_PRIM_GAUGE 9
#define FT_PRIM_CLOCK 10
#define FT_PRIM_DIAL 11

// Colors - fully saturated colors defined here
#define RED       0xFF0000		// Red
#define GREEN	0x00FF00		// Green
#define BLUE	0x0000FF		// Blue
#define WHITE	0xFFFFFF		// White
#define BLACK      0x000000		// Black
#define PURPLE    0xFF00FF        // PURPLE
#define YELLOW    0xFFFF00
#define ORANGE    0xFF8000
#define GREY      0x6E6E6E
#define FOREST    0x25844C
#define BURNT     0xDFA44C
#define BROWN     0x5F4B2F
#define MAROON    0x6D1313
#define SKIN      0xF8BFBF
#define PINK      0xFF04C9
#define CORAL     0x42DB96

#endif
