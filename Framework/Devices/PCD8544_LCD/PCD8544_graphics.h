/*
 * PCD8544_graphics.h
 *
 *  Created on: 02.10.2015
 *      Author: harald
 *
 *      original c-code for arduino taken from adafruit
 */

#ifndef DEVICES_PCD8544_LCD_PCD8544_GRAPHICS_H_
#define DEVICES_PCD8544_LCD_PCD8544_GRAPHICS_H_

#ifndef __x86_64
// FIXME make it working on X86

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "PCD8544_basis.h"


#define _BV(bit) \
	(1 << (bit))


// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
#define enablePartialUpdate

// colours for the graphical elements
#define BLACK 1
#define WHITE 0

#define swap(a, b) { uint8_t t = a; a = b; b = t; }


class PCD8544_graphics: public PCD8544_basis
{
#ifdef USE_GRAPHICS
public:

  PCD8544_graphics(PCD8544_socket* socket);
  uint8_t get_chars_per_line(void);
  uint8_t get_dispLines(void);

  void updateBoundingBox(uint8_t xmin,
						 uint8_t ymin,
						 uint8_t xmax,
						 uint8_t ymax);

  void draw_filled_circle(uint8_t x0_px,
						  uint8_t y0_px,
						  uint8_t r,
						  uint8_t color = BLACK);

  void draw_circle(uint8_t x0_px,
				   uint8_t y0_px,
				   uint8_t r_px,
				   uint8_t color = BLACK);

  void draw_filled_rectangle(uint8_t x_px,
							 uint8_t y_px,
							 uint8_t width,
							 uint8_t height,
							 uint8_t color = BLACK);

  void draw_rectangle(uint8_t x_px,
					  uint8_t y_px,
					  uint8_t width,
					  uint8_t height,
					  uint8_t color = BLACK);

  void draw_line(uint8_t x0_px,
				 uint8_t y0_px,
				 uint8_t x1_px,
				 uint8_t y1_px,
				 uint8_t color = BLACK);

  void draw_bitmap(uint8_t x_px,
				   uint8_t y_px,
				   const uint8_t *bitmap,
				   uint8_t width,
				   uint8_t height,
				   uint8_t color = BLACK);


  void write_string(char *str);

  void write_string(const char *str);

  void write_string(uint8_t x,
					uint8_t y,
					char *character);

  void write_string(uint8_t x_px,
					uint8_t y_px,
					const char *str);

  void write_char(char character);

  void setCursor(uint8_t x_px,
				 uint8_t y_px);

  uint8_t line_2_y_pix(uint8_t line);

  void clear(void);

  void set_pixel(uint8_t x_px,
				 uint8_t y_px,
				 uint8_t color = BLACK);

  void display(void);

  uint8_t* get_graphics_buffer(void);

private:
  void draw_char(uint8_t x_px,
				 uint8_t y_px,
				 char character);

  uint8_t graphics_buffer[DISPLAY_WIDTH_px * DISPLAY_HEIGHT_px / 8];
  uint8_t textcolor;
  uint8_t cursor_x,   cursor_y;

#ifdef enablePartialUpdate
uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

#endif
};

#endif
#endif /* DEVICES_PCD8544_LCD_PCD8544_GRAPHICS_H_ */



