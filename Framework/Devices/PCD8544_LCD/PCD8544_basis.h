/*
 * PCD8544_basis.h
 *
 *  Created on: 19.09.2015
 *      Author: harald
 */

#ifndef DEVICES_PCD8544_LCD_PCD8544_BASIS_H_
#define DEVICES_PCD8544_LCD_PCD8544_BASIS_H_

#ifndef __x86_64
// FIXME make it working on X86

#include "PCD8544_socket.h"
#include <stdlib.h>



#define USE_FONT_5x8 // default!
//#define USE_FONT_4x6
//#define USE_FONT_BIG
//#define USE_FONT_6x8
//#define USE_FONT_8x8
//#define USE_FONT_16x8
#define USE_GRAPHICS


#ifdef USE_FONT_BIG
#include "fonts/font_big.h"
#endif
#ifdef USE_FONT_5x8
#include "fonts/5x8_vertikal_LSB_1.h"
#define FONT_5x8 0
#endif
#ifdef USE_FONT_4x6
#include "fonts/4x6_vertikal_LSB_1.h"
#define FONT_4x6 1
#endif
#ifdef USE_FONT_6x8
#include "fonts/6x8_vertikal_LSB_1.h"
#define FONT_6x8 2
#endif
#ifdef USE_FONT_8x8
#include "fonts/8x8_vertikal_LSB_1.h"
#define FONT_8x8 3
#endif
#ifdef USE_FONT_16x8
#include "fonts/16x8_vertikal_LSB_1.h"
#define FONT_16x8 4
#endif

#define DISPLAY_WIDTH_px  84
#define DISPLAY_HEIGHT_px 48
#define MENU_NORMAL	0
#define MENU_HIGHLIGHT 1

#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80


#ifdef __cplusplus

class PCD8544_basis
{

public:

  PCD8544_basis(PCD8544_socket *socket);

  void draw_bitmap(uint8_t column,
					uint8_t line,
					const uint8_t *ptr_to_bitmap,
					uint8_t size_in_pixels_x,
					uint8_t size_in_pixels_y);

  void write_string(uint8_t column,
					 uint8_t line,
					 char 	 *ptr_to_text,
					 uint8_t mode = MENU_NORMAL);

  void write_string (uint8_t 	  column,
					 uint8_t 	  line,
					 const char *const_ptr_to_text,
					 uint8_t 	  mode = MENU_NORMAL);

  //TODO Vereinheitlichung der write_string methoden mit graphics
  //TODO replace mode with color

  void write_char(uint8_t character,
				   uint8_t mode = MENU_NORMAL);

  void set_line_n_column(uint8_t column,
						  uint8_t line);

  void clear(void);

  void 		switch_font(uint8_t font_id);
  uint8_t 	get_actual_fontwidth(void);
  uint8_t 	get_actual_fontheight(void);
  void 		backlight_on(void);
  void 		backlight_off(void);

  inline uint8_t* get_font_ptr(void)
  {
    return font_ptr;
  }

#ifdef USE_FONT_BIG
  void write_string_big (uint8_t column,
						 uint8_t line,
						 char 	 *ptr_to_text,
						 uint8_t mode = MENU_NORMAL);
  void write_char_big (uint8_t column,
					   uint8_t line,
					   uint8_t character,
					   uint8_t mode = MENU_NORMAL);
#endif

protected:
  PCD8544_socket 	*socket;

private:
  uint8_t 			*font_ptr;
  uint8_t 			actual_fontwidth, actual_fontheight;
};

#endif
#endif
#endif /* DEVICES_PCD8544_LCD_PCD8544_BASIS_H_ */
