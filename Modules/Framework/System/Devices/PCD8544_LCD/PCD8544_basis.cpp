/*
 * Nokia_3310_service.cpp
 *
 *  Created on: 19.09.2015
 *      Author: harald
 */

#include "PCD8544_basis.h"
#include <System/Devices/PCD8544_LCD/PCD8544_socket.h>
#include <System/OsHelpers.h>
//#include "../../Instances/Common.h"

#ifndef __x86_64
// FIXME make it working on X86

namespace lcd {
PCD8544_basis::PCD8544_basis() {
}

void PCD8544_basis::initHardware(void) {
	PCD8544_socket::do_reset();
	PCD8544_socket::backlight_on();

#define DLY 10
	OsHelpers::delay(DLY);
	PCD8544_socket::write_byte(0x21, PCD8544_COMMAND); // switch to extended commands
	OsHelpers::delay(DLY);

	PCD8544_socket::write_byte(0xc0, PCD8544_COMMAND); // set Vop (contrast)
	OsHelpers::delay(DLY);

	PCD8544_socket::write_byte(0x06, PCD8544_COMMAND); // set temp-coefficient
	OsHelpers::delay(DLY);

	PCD8544_socket::write_byte(0x13, PCD8544_COMMAND); // bias mode 1:48
	OsHelpers::delay(DLY);

	PCD8544_socket::write_byte(0x20, PCD8544_COMMAND); // horizontal addressing
	OsHelpers::delay(DLY);

	PCD8544_socket::write_byte(0x0c, PCD8544_COMMAND); // enable normal display
	OsHelpers::delay(DLY);

	PCD8544_socket::write_byte(0x09, PCD8544_COMMAND); // turn all segments on
	OsHelpers::delay(1000); // Wait for device to settle

	PCD8544_socket::write_byte(0x08, PCD8544_COMMAND); // display blank

	// if no font is defined, initialize to empty
	switch_font(0);
}

// doesn't work with glcd bitmaps, use graphics
void PCD8544_basis::draw_bitmap(uint8_t column, uint8_t line,
		const uint8_t *ptr_to_bitmap, uint8_t size_in_pixels_x,
		uint8_t size_in_pixels_y) {
	unsigned int col_counter, line_counter;
	uint8_t row;

	if (size_in_pixels_y % 8 == 0)
		row = size_in_pixels_y / 8;
	else
		row = size_in_pixels_y / 8 + 1;

	for (line_counter = 0; line_counter < row; line_counter++) {
		set_line_n_column(column, line);
		for (col_counter = 0; col_counter < size_in_pixels_x; col_counter++) {
			PCD8544_socket::write_byte(
					*(ptr_to_bitmap + col_counter
							+ line_counter * size_in_pixels_x), 1);
		}
		line++;
	}
}

void PCD8544_basis::write_string(uint8_t column, uint8_t line,
		char *ptr_to_text, uint8_t mode) {
	set_line_n_column(column, line);
	while (1) {
		if (*ptr_to_text == '\0')
			return;
		write_char(*ptr_to_text, mode);
		ptr_to_text++;
	}
}

void PCD8544_basis::write_string(uint8_t column, uint8_t line,
		const char *const_ptr_to_text, uint8_t mode) {
	write_string(column, line, (char*) const_ptr_to_text, mode);
}

#ifdef USE_FONT_BIG
void PCD8544_basis::write_string_big(uint8_t column,
		uint8_t line,
		char 	 *ptr_to_text,
		uint8_t mode )
{
	while ( *ptr_to_text ){

		write_char_big( column, line, *ptr_to_text , mode );

		if(*ptr_to_text++ == '.')
			column += 5;
		else
			column += 12;
	}
}

/* write char in big font */
void PCD8544_basis::write_char_big(uint8_t column,
		uint8_t line,
		uint8_t character,
		uint8_t mode)
{
	uint8_t i, j;
	uint8_t *pFont;
	uint8_t ch_dat;

	pFont = (uint8_t *) big_number;

	if(character == '.')
		character = 10;
	else if (character == '+')
		character = 11;
	else if (character == '-')
		character = 12;
	else
		character = character & 0x0f;

	for(i=0;i<3;i++)
	{
		PCD8544_basis::set_line_n_column ( column, line+i);

		for(j=0; j<16; j++){
			ch_dat =  *(pFont+character*48 + i*16 +j);
			PCD8544_socket::write_byte( (mode == MENU_NORMAL)? ch_dat : (ch_dat^0xff), 1);
		}
	}
}
#endif

void PCD8544_basis::write_char(uint8_t character, uint8_t mode) {
	uint8_t line;
	uint8_t pattern;

	// no font was selected
	if (actual_fontwidth == 0)
		return;

	for (line = 0; line < actual_fontwidth; line++) {
		pattern = *(font_ptr + character * actual_fontwidth + line);

		if (mode != MENU_NORMAL)
			pattern ^= 0xff;

		PCD8544_socket::write_byte(pattern, 1);
	}
}

void PCD8544_basis::switch_font(uint8_t font_id) {
	switch (font_id) {
#ifdef USE_FONT_5x8
	case FONT_5x8:
		font_ptr = (uint8_t*) font_5x8_vertical_lsb;
		actual_fontwidth = font_5x8_width;
		actual_fontheight = font_5x8_height;
		break;
#endif
#ifdef USE_FONT_4x6
	case FONT_4x6:
		font_ptr = (uint8_t *)font_4x6_vertical_lsb;
		actual_fontwidth  = font_4x6_width;
		actual_fontheight = font_4x6_height;
		break;
#endif
#ifdef USE_FONT_6x8
	case FONT_6x8:
		font_ptr = (uint8_t *)font_6x8_vertical_lsb;
		actual_fontwidth  = font_6x8_width;
		actual_fontheight = font_6x8_height;
		break;
#endif
#ifdef USE_FONT_8x8
	case FONT_8x8:
		font_ptr = (uint8_t *) font_8x8_vertical_lsb;
		actual_fontwidth  = font_8x8_width;
		actual_fontheight = font_8x8_height;
		break;
#endif
#ifdef USE_FONT_16x8
	case FONT_16x8:
		font_ptr = (uint8_t *)font_16x8_vertical_lsb;
		actual_fontwidth  = font_16x8_width;
		actual_fontheight = font_16x8_height;
		break;
#endif
	default:
		font_ptr = NULL;
		actual_fontwidth = 0;
		actual_fontheight = 0;
	}
}

uint8_t PCD8544_basis::get_actual_fontwidth(void) {
	return actual_fontwidth;
}

uint8_t PCD8544_basis::get_actual_fontheight(void) {
	return actual_fontheight;
}

void PCD8544_basis::set_line_n_column(uint8_t column, uint8_t line) {
	PCD8544_socket::write_byte(0x40 | line, 0); // 0 <= line <= 5
	PCD8544_socket::write_byte(0x80 | column, 0); // 0 <= X <= 83
}

void PCD8544_basis::clear(void) {
	uint16_t i;

	PCD8544_socket::write_byte(0x0c, 0); // Normal Mode
	PCD8544_socket::write_byte(0x80, 0); // set x address to 0

	for (i = 0; i < (DISPLAY_WIDTH_px * DISPLAY_HEIGHT_px / 8); i++)
		PCD8544_socket::write_byte(0, 1);
}

void PCD8544_basis::backlight_on(void) {
	PCD8544_socket::backlight_on();
}

void PCD8544_basis::backlight_off(void) {
	PCD8544_socket::backlight_off();
}

} // namespace lcd
#endif
