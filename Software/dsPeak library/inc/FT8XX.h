/*
 * File:   FT8XX.h
 * Author: Intellitrol
 *
 * Created on 28 juin 2016, 09:13
 */

#ifndef __FT8XX_H_
#define __FT8XX_H_

#include "FT_Gpu.h"
#include "FT8XX_user_definition.h"

//Structure definition for FT8XX primitives. Define a number for each of them in
//FT8XX_user_define.h. 

//Module initializations functions
void FT8XX_init (void); 
void FT8XX_set_touch_tag (unsigned char prim_type, unsigned char prim_num, unsigned char tag_num);
unsigned char FT8XX_read_touch_tag (void);
unsigned char FT8XX_get_touch_tag (void);
void FT8XX_clear_touch_tag (void);
void FT8XX_touchpanel_init (void);
STTouch FT8XX_touchpanel_read (STTouch touch_read);
void FT8XX_start_new_dl (void);
void FT8XX_update_screen_dl (void);  
void FT8XX_write_dl_char (unsigned char data);
void FT8XX_write_dl_int (unsigned int data);
void FT8XX_write_dl_long (unsigned long data);
void FT8XX_host_command (unsigned char cmd);
void FT8XX_wr8 (unsigned long adr, unsigned char data);
void FT8XX_wr16(unsigned long adr, unsigned int data);
void FT8XX_wr32(unsigned long adr, unsigned long data);
unsigned char FT8XX_rd8 (unsigned long adr);
unsigned int FT8XX_rd16 (unsigned long adr);
unsigned long FT8XX_rd32 (unsigned long adr);
unsigned int FT8XX_inc_cmd_offset (unsigned int cur_off, unsigned char cmd_size);
unsigned int FT8XX_get_cmd_offset_value (void);

void FT8XX_clear_screen (unsigned long color);
void FT8XX_set_context_color (unsigned long color);
void FT8XX_set_context_bcolor (unsigned long color);
void FT8XX_set_context_fcolor (unsigned long color);

// 
void FT8XX_draw_line_h (unsigned int x1, unsigned int x2, unsigned int y, unsigned int w);
void FT8XX_draw_line_v (unsigned int y1, unsigned int y2, unsigned int x, unsigned int w);
void FT8XX_draw_point (unsigned int x, unsigned int y, unsigned int r);
void FT8XX_modify_element_string (unsigned char number, unsigned char type, char *str);  
void FT8XX_write_bitmap (const unsigned char *img_ptr, const unsigned char *lut_ptr, unsigned long img_length, unsigned long base_adr);    

void FT8XX_CMD_tracker(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char tag);

#if MAX_GRADIENT_NB > 0
 void FT8XX_CMD_gradient(unsigned char number, unsigned int x0, unsigned int y0, unsigned long rgb0, unsigned int x1, unsigned int y1, unsigned long rgb1);
 void FT8XX_draw_gradient (STGradient *st_Gradient);
 void FT8XX_modify_gradient (STGradient *st_Gradient, unsigned char type, unsigned long value);
 unsigned char FT8XX_get_gradient_nb (void);
#endif

#if MAX_KEYS_NB > 0
 void FT8XX_CMD_keys(unsigned char number, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int f, unsigned int opt, char *str);
 void FT8XX_draw_keys(STKeys *st_Keys);
 unsigned char FT8XX_get_keys_nb (void);
#endif

#if MAX_WINDOW_NB > 0
 unsigned char FT8XX_check_window (STWindow *st_Window, STTouch touch_data);
 unsigned char FT8XX_check_touch (STWindow *st_Window, STTouch touch_data);
 void FT8XX_CMD_window (unsigned char number, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
 unsigned char FT8XX_heck_screen_press (STTouch touch_data);
#endif

#if MAX_SLIDER_NB > 0
 void FT8XX_CMD_slider (unsigned char number, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int opt, unsigned int v, unsigned int r);
 //unsigned int slider_update (STTouch touch_read, STSlider *st_Slider, STWindow *st_Window);
 void FT8XX_draw_slider (STSlider *st_Slider);
 void FT8XX_modify_slider (STSlider *st_Slider, unsigned char type, unsigned int value);
 unsigned char FT8XX_get_slider_nb (void);
#endif

#if MAX_TEXT_NB > 0
 void FT8XX_CMD_text (unsigned char number, unsigned int x, unsigned int y, unsigned int f, unsigned int o, const char *str);
 void FT8XX_draw_text (STText *st_Text);
 unsigned char FT8XX_get_text_nb (void);
#endif

#if MAX_NUMBER_NB > 0
 void FT8XX_CMD_number (unsigned char number, unsigned int x, unsigned int y, unsigned int f, unsigned int o, unsigned long n);
 void FT8XX_draw_number (STNumber *st_Number);
 void FT8XX_modify_number (STNumber *st_Number, unsigned char type, unsigned long value);
 unsigned char FT8XX_get_number_nb (void);
#endif

#if MAX_BUTTON_NB > 0
 void FT8XX_CMD_button (unsigned char number, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int f, unsigned int o, const char *str);
 void FT8XX_draw_button (STButton *st_Button);
 void FT8XX_modify_button (STButton *st_Button, unsigned char type, unsigned int value);
 void FT8XX_modify_button_string (STButton *st_Button, char *str, unsigned char length);
 unsigned char FT8XX_get_button_nb (void);
#endif

#if MAX_TOGGLE_NB > 0
 void FT8XX_CMD_toggle (unsigned char number, unsigned int x, unsigned int y, unsigned int w, unsigned int f, unsigned int o, unsigned char state, const char *str);
 void FT8XX_draw_toggle (STToggle *st_Toggle);
 void FT8XX_change_toggle_state (STToggle *st_Toggle, unsigned char state);
 unsigned char FT8XX_get_toggle_nb (void);
#endif

#if MAX_CLOCK_NB > 0
 void FT8XX_CMD_clock (unsigned char number, unsigned int x, unsigned int y, unsigned int r, unsigned int opt, unsigned char h, unsigned char m, unsigned char s, unsigned char ms);
 void FT8XX_modify_clock_hms (STClock *st_Clock, unsigned char h, unsigned char m, unsigned char s);
 void FT8XX_draw_clock (STClock *st_Clock);
 unsigned char FT8XX_get_clock_nb (void);
#endif

#if MAX_SCROLLBAR_NB > 0
 void FT8XX_CMD_scrollbar (unsigned char number, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int opt, unsigned int val, unsigned int size, unsigned int range);
 void FT8XX_draw_scrollbar  (STScrollbar *st_Scrollbar);
 void FT8XX_modify_scrollbar (STScrollbar *st_Scrollbar, unsigned char type, unsigned int value);
 unsigned char FT8XX_get_scrollbar_nb (void);
#endif

#if MAX_PROGRESS_NB > 0
 void FT8XX_CMD_progress (unsigned char number, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int opt, unsigned int val, unsigned int range);
 void FT8XX_draw_progress (STProgress *st_Progress);
 void FT8XX_modify_progress (STProgress *st_Progress, unsigned char val);
 unsigned char FT8XX_get_progress_nb (void);
#endif

#if MAX_GAUGE_NB > 0
 void FT8XX_CMD_gauge (unsigned char number, unsigned int x, unsigned int y, unsigned int r, unsigned int opt, unsigned int maj, unsigned int min, unsigned int val, unsigned int range);
 void FT8XX_draw_gauge (STGauge *st_Gauge);
 void FT8XX_modify_gauge (STGauge *st_Gauge, unsigned char type, unsigned int value);
 unsigned char FT8XX_get_gauge_nb (void);
#endif

#if MAX_DIAL_NB > 0
 void FT8XX_CMD_dial (unsigned char number, unsigned int x, unsigned int y, unsigned int r, unsigned int opt, unsigned int val);
 void FT8XX_draw_dial (STDial *st_Dial);
 unsigned char FT8XX_get_dial_nb (void);
#endif

#if MAX_RECT_NB > 0
 void FT8XX_CMD_rectangle (unsigned char number, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int w);
 void FT8XX_draw_rectangle (STRectangle *st_Rectangle);
 unsigned char FT8XX_get_rectangle_nb (void);
#endif  

#endif
