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
void FT8XX_set_touch_tag (uint8_t prim_type, uint8_t prim_num, uint8_t tag_num);
uint8_t FT8XX_read_touch_tag (void);
uint8_t FT8XX_get_touch_tag (void);
void FT8XX_clear_touch_tag (void);
void FT8XX_touchpanel_init (void);
STTouch FT8XX_touchpanel_read (STTouch touch_read);
void FT8XX_start_new_dl (void);
void FT8XX_update_screen_dl (void);  
void FT8XX_write_dl_char (uint8_t data);
void FT8XX_write_dl_int (uint16_t data);
void FT8XX_write_dl_long (uint32_t data);
void FT8XX_host_command (uint8_t cmd);
void FT8XX_wr8 (uint32_t adr, uint8_t data);
void FT8XX_wr16(uint32_t adr, uint16_t data);
void FT8XX_wr32(uint32_t adr, uint32_t data);
uint8_t FT8XX_rd8 (uint32_t adr);
uint16_t FT8XX_rd16 (uint32_t adr);
uint32_t FT8XX_rd32 (uint32_t adr);
uint16_t FT8XX_inc_cmd_offset (uint16_t cur_off, uint8_t cmd_size);
uint16_t FT8XX_get_cmd_offset_value (void);

void FT8XX_clear_screen (uint32_t color);
void FT8XX_set_context_color (uint32_t color);
void FT8XX_set_context_bcolor (uint32_t color);
void FT8XX_set_context_fcolor (uint32_t color);

// 
void FT8XX_draw_line_h (uint16_t x1, uint16_t x2, uint16_t y, uint16_t w);
void FT8XX_draw_line_v (uint16_t y1, uint16_t y2, uint16_t x, uint16_t w);
void FT8XX_draw_point (uint16_t x, uint16_t y, uint16_t r);
void FT8XX_modify_element_string (uint8_t number, uint8_t type, char *str);  
void FT8XX_write_bitmap (const uint8_t *img_ptr, const uint8_t *lut_ptr, uint32_t img_length, uint32_t base_adr);    

void FT8XX_CMD_tracker(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t tag);

#if MAX_GRADIENT_NB > 0
 void FT8XX_CMD_gradient(uint8_t number, uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1);
 void FT8XX_draw_gradient (STGradient *st_Gradient);
 void FT8XX_modify_gradient (STGradient *st_Gradient, uint8_t type, uint32_t value);
 uint8_t FT8XX_get_gradient_nb (void);
#endif

#if MAX_KEYS_NB > 0
 void FT8XX_CMD_keys(uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t opt, char *str);
 void FT8XX_draw_keys(STKeys *st_Keys);
 uint8_t FT8XX_get_keys_nb (void);
#endif

#if MAX_WINDOW_NB > 0
 uint8_t FT8XX_check_window (STWindow *st_Window, STTouch touch_data);
 uint8_t FT8XX_check_touch (STWindow *st_Window, STTouch touch_data);
 void FT8XX_CMD_window (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
 uint8_t FT8XX_heck_screen_press (STTouch touch_data);
#endif

#if MAX_SLIDER_NB > 0
 void FT8XX_CMD_slider (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t v, uint16_t r);
 //uint16_t slider_update (STTouch touch_read, STSlider *st_Slider, STWindow *st_Window);
 void FT8XX_draw_slider (STSlider *st_Slider);
 void FT8XX_modify_slider (STSlider *st_Slider, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_slider_nb (void);
#endif

#if MAX_TEXT_NB > 0
 void FT8XX_CMD_text (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, const char *str);
 void FT8XX_draw_text (STText *st_Text);
 uint8_t FT8XX_get_text_nb (void);
#endif

#if MAX_NUMBER_NB > 0
 void FT8XX_CMD_number (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, uint32_t n);
 void FT8XX_draw_number (STNumber *st_Number);
 void FT8XX_modify_number (STNumber *st_Number, uint8_t type, uint32_t value);
 uint8_t FT8XX_get_number_nb (void);
#endif

#if MAX_BUTTON_NB > 0
 void FT8XX_CMD_button (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t o, const char *str);
 void FT8XX_draw_button (STButton *st_Button);
 void FT8XX_modify_button (STButton *st_Button, uint8_t type, uint16_t value);
 void FT8XX_modify_button_string (STButton *st_Button, char *str, uint8_t length);
 uint8_t FT8XX_get_button_nb (void);
#endif

#if MAX_TOGGLE_NB > 0
 void FT8XX_CMD_toggle (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t f, uint16_t o, uint8_t state, const char *str);
 void FT8XX_draw_toggle (STToggle *st_Toggle);
 void FT8XX_change_toggle_state (STToggle *st_Toggle, uint8_t state);
 uint8_t FT8XX_get_toggle_nb (void);
#endif

#if MAX_CLOCK_NB > 0
 void FT8XX_CMD_clock (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint8_t h, uint8_t m, uint8_t s, uint8_t ms);
 void FT8XX_modify_clock_hms (STClock *st_Clock, uint8_t h, uint8_t m, uint8_t s);
 void FT8XX_draw_clock (STClock *st_Clock);
 uint8_t FT8XX_get_clock_nb (void);
#endif

#if MAX_SCROLLBAR_NB > 0
 void FT8XX_CMD_scrollbar (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t size, uint16_t range);
 void FT8XX_draw_scrollbar  (STScrollbar *st_Scrollbar);
 void FT8XX_modify_scrollbar (STScrollbar *st_Scrollbar, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_scrollbar_nb (void);
#endif

#if MAX_PROGRESS_NB > 0
 void FT8XX_CMD_progress (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t range);
 void FT8XX_draw_progress (STProgress *st_Progress);
 void FT8XX_modify_progress (STProgress *st_Progress, uint8_t val);
 uint8_t FT8XX_get_progress_nb (void);
#endif

#if MAX_GAUGE_NB > 0
 void FT8XX_CMD_gauge (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t maj, uint16_t min, uint16_t val, uint16_t range);
 void FT8XX_draw_gauge (STGauge *st_Gauge);
 void FT8XX_modify_gauge (STGauge *st_Gauge, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_gauge_nb (void);
#endif

#if MAX_DIAL_NB > 0
 void FT8XX_CMD_dial (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t val);
 void FT8XX_draw_dial (STDial *st_Dial);
 uint8_t FT8XX_get_dial_nb (void);
#endif

#if MAX_RECT_NB > 0
 void FT8XX_CMD_rectangle (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t w);
 void FT8XX_draw_rectangle (STRectangle *st_Rectangle);
 uint8_t FT8XX_get_rectangle_nb (void);
#endif  

#endif
