/*
 * PCD8544_graphics.cpp
 *
 *  Created on: 02.10.2015
 *      Author: harald
 *
 *      original c-code taken from adafruit
 */

#ifndef __x86_64
// FIXME make it working on X86

#include "PCD8544_graphics.h"


#ifdef USE_GRAPHICS

PCD8544_graphics::PCD8544_graphics(PCD8544_socket* socket):
PCD8544_basis(socket)
{
	textcolor = BLACK;

#ifdef enablePartialUpdate
	xUpdateMin = 0;
	xUpdateMax = 0;
	yUpdateMin = 0;
	yUpdateMax = 0;
#endif

	cursor_x   = 0;
	cursor_y   = 0;

	// clear();
}


void PCD8544_graphics::updateBoundingBox(uint8_t xmin,
		uint8_t ymin,
		uint8_t xmax,
		uint8_t ymax)
{
#ifdef enablePartialUpdate
	if (xmin < xUpdateMin) xUpdateMin = xmin;
	if (xmax > xUpdateMax) xUpdateMax = xmax;
	if (ymin < yUpdateMin) yUpdateMin = ymin;
	if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}


void PCD8544_graphics::setCursor(uint8_t x_px,
		uint8_t y_px)
{
	cursor_x = x_px;
	cursor_y = y_px;
}


uint8_t PCD8544_graphics::line_2_y_pix(uint8_t line)
{
	line++;

	return
			line * get_actual_fontheight() - get_actual_fontheight();
}


void PCD8544_graphics::write_string(char *str)
{
	while (*str != '\0')
	{
		write_char(*str);
		str++;
	}
}

void PCD8544_graphics::write_string(const char *str)
{
	write_string((char*) str);
}


void PCD8544_graphics::write_string(uint8_t x_px,
		uint8_t y_px,
		char *str)
{
	cursor_x = x_px;
	cursor_y = y_px;
	write_string(str);
}


void PCD8544_graphics::write_string(uint8_t x_px,
		uint8_t y_px,
		const char *str)
{
	write_string(x_px, y_px, (char*) str);
}




void PCD8544_graphics::write_char(char character)
{
	if (character == '\n')
	{
		cursor_y += get_actual_fontheight();
		cursor_x = 0;
	}
	else if (character == '\r')
	{
		// skip em
	}
	else
	{
		draw_char(cursor_x, cursor_y, character);
		cursor_x += get_actual_fontwidth();

		if (cursor_x >= (DISPLAY_WIDTH_px - get_actual_fontwidth()))
		{
			cursor_x =  0;
			cursor_y += get_actual_fontheight();
		}
		if (cursor_y >= DISPLAY_HEIGHT_px)
			cursor_y = 0;
	}
}


void PCD8544_graphics::draw_char(uint8_t x_px,
		uint8_t y_px,
		char ch)
{
	uint8_t pattern, i, j;

	if (y_px >= DISPLAY_HEIGHT_px)
		return;

	if ((x_px + get_actual_fontwidth()) >= DISPLAY_WIDTH_px)
		return;

	for (i =0; i < get_actual_fontwidth(); i++ )
	{
		pattern =
				*(get_font_ptr()+(ch * get_actual_fontwidth())+i);

		// Workaround for 4x6 font
		// The font contains two empty pixel at the upper side
		if (get_actual_fontheight() == 6)
			pattern = (pattern >> 2);

		for (j = 0; j < get_actual_fontheight(); j++)
		{
			if (pattern & _BV(j))
				set_pixel(x_px + i, y_px + j, textcolor);
			else
				set_pixel(x_px + i, y_px + j, !textcolor);
		}
	}
	updateBoundingBox(x_px, y_px,
			x_px + get_actual_fontwidth(),
			y_px + get_actual_fontheight());
}


// filled rectangle
void PCD8544_graphics::draw_filled_rectangle(uint8_t x,
		uint8_t y,
		uint8_t width,
		uint8_t height,
		uint8_t color)
{
	// stupidest version - just pixels - but fast with internal buffer!
	for (uint8_t i=x; i < x+width; i++) {
		for (uint8_t j=y; j < y+height; j++) {
			set_pixel(i, j, color);
		}
	}
	updateBoundingBox(x, y, x+width, y+height);
}


// draw a rectangle
void PCD8544_graphics::draw_rectangle(uint8_t x,
		uint8_t y,
		uint8_t width,
		uint8_t height,
		uint8_t color)
{
	// stupidest version - just pixels - but fast with internal buffer!
	for (uint8_t i=x; i < x + width; i++) {
		set_pixel(i, y, color);
		set_pixel(i, y + height - 1, color);
	}
	for (uint8_t i=y; i < y + height; i++) {
		set_pixel(x, i, color);
		set_pixel(x + width - 1, i, color);
	}
	updateBoundingBox(x, y, x+width, y+height);
}


// draw a circle outline
void PCD8544_graphics::draw_circle(uint8_t x0,
		uint8_t y0,
		uint8_t r,
		uint8_t color)
{
	updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	set_pixel(x0, y0+r, color);
	set_pixel(x0, y0-r, color);
	set_pixel(x0+r, y0, color);
	set_pixel(x0-r, y0, color);

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		set_pixel(x0 + x, y0 + y, color);
		set_pixel(x0 - x, y0 + y, color);
		set_pixel(x0 + x, y0 - y, color);
		set_pixel(x0 - x, y0 - y, color);

		set_pixel(x0 + y, y0 + x, color);
		set_pixel(x0 - y, y0 + x, color);
		set_pixel(x0 + y, y0 - x, color);
		set_pixel(x0 - y, y0 - x, color);
	}
}


void PCD8544_graphics::draw_filled_circle(uint8_t x0,
		uint8_t y0,
		uint8_t r,
		uint8_t color)
{
	updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	for (uint8_t i=y0-r; i<=y0+r; i++)
	{
		set_pixel(x0, i, color);
	}

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for (uint8_t i=y0-y; i<=y0+y; i++)
		{
			set_pixel(x0+x, i, color);
			set_pixel(x0-x, i, color);
		}
		for (uint8_t i=y0-x; i<=y0+x; i++)
		{
			set_pixel(x0+y, i, color);
			set_pixel(x0-y, i, color);
		}
	}
}


// bresenham's algorithm - thx wikpedia
void PCD8544_graphics::draw_line(uint8_t x0,
		uint8_t y0,
		uint8_t x1,
		uint8_t y1,
		uint8_t color)
{
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	// much faster to put the test here,
	//since we've already sorted the points
	updateBoundingBox(x0, y0, x1, y1);

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;}

	for (; x0<=x1; x0++) {
		if (steep) {
			set_pixel(y0, x0, color);
		} else {
			set_pixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}


void PCD8544_graphics::draw_bitmap(uint8_t x,
		uint8_t y,
		const uint8_t *bitmap,
		uint8_t width,
		uint8_t height,
		uint8_t color)
{
	for (uint8_t j=0; j<height; j++)
	{
		for (uint8_t i=0; i<width; i++ )
		{
			if (*(bitmap + i + (j/8)*width) & _BV(j%8))
			{
				set_pixel( x+i, y+j, color);
			}
		}
	}

	updateBoundingBox(x, y, x+width, y+height);
}


// clear everything
void PCD8544_graphics::clear(void)
{
	PCD8544_basis::clear();

	memset(graphics_buffer, 0, DISPLAY_WIDTH_px * DISPLAY_HEIGHT_px / 8);
	updateBoundingBox(0, 0, DISPLAY_WIDTH_px-1, DISPLAY_HEIGHT_px-1);
	cursor_y = cursor_x = 0;
}


// the most basic function, set a single pixel
void PCD8544_graphics::set_pixel(uint8_t x,
		uint8_t y,
		uint8_t color)
{
	if ((x >= DISPLAY_WIDTH_px) || (y >= DISPLAY_HEIGHT_px))
		return;

	// x is which column
	if (color)
		graphics_buffer[x+ (y/8)*DISPLAY_WIDTH_px] |= (uint8_t) _BV(y%8);
	else
		graphics_buffer[x+ (y/8)*DISPLAY_WIDTH_px] &= (uint8_t) ~_BV(y%8);

	updateBoundingBox(x,y,x,y);
}


void PCD8544_graphics::display(void)
{
	uint8_t col, maxcol, line;

	for(line = 0; line < (DISPLAY_HEIGHT_px / 8); line++)
	{
#ifdef enablePartialUpdate
		// check if this line is part of update
		if ( yUpdateMin >= ((line+1)*8) )
		{
			continue;   // nope, skip it!
		}
		if (yUpdateMax < line*8)
		{
			break;
		}
#endif

#ifdef enablePartialUpdate
		col 	 = xUpdateMin;
		maxcol = xUpdateMax;
#else
		// start at the beginning of the row
		col 	 = 0;
		maxcol = DISPLAY_WIDTH_px-1;
#endif

		set_line_n_column(col, line);

		for(; col <= maxcol; col++)
		{
			socket->write_byte
			(graphics_buffer[(DISPLAY_WIDTH_px * line) + col],
					PCD8544_DATA);
		}

	}

	// no idea why this is necessary but it is to finish the last byte?
	socket->write_byte(PCD8544_SETYADDR, PCD8544_COMMAND);

#ifdef enablePartialUpdate
	xUpdateMin = DISPLAY_WIDTH_px - 1;
	xUpdateMax = 0;
	yUpdateMin = DISPLAY_HEIGHT_px - 1;
	yUpdateMax = 0;
#endif

}

uint8_t* PCD8544_graphics::get_graphics_buffer(void)
{
	return graphics_buffer;
}

uint8_t PCD8544_graphics::get_chars_per_line(void)
{
	return (DISPLAY_WIDTH_px-1) / get_actual_fontwidth();
}

uint8_t PCD8544_graphics::get_dispLines(void)
{
	return DISPLAY_HEIGHT_px / get_actual_fontheight();
}


#endif
#endif








