/*
 * lcdpi.h
 *
 *  Created on: Apr 24, 2014
 *      Author: psanetra
 */

#ifndef LCDPI_H_
#define LCDPI_H_

#include <stdint.h>

typedef enum _lcd_line {
	LINE1 = 1,
	LINE2 = 2,
	LINE3 = 4,
	LINE4 = 8
} lcd_line;

int8_t lcd_debug;

extern void lcd_setup4b();

extern void lcd_setup4b_schnatterente();

extern void lcd_setup4b_ks0066();

extern void lcd_print(char* c);

extern void lcd_line_print(lcd_line line, char* c);

extern void lcd_display_clear();

extern void lcd_line_clear(lcd_line line);

extern void lcd_return_home();

extern void lcd_entry_mode_set(int incOrDecCursor, int doDisplayShift);

extern void lcd_display_on_off(int displayOn, int cursorOn, int cursorBlink);

extern void lcd_cursor_or_display_shift(int displayOrCursor, int rightOrLeft);

extern void lcd_display_shift(int rightOrLeft);

extern void lcd_cursor_shift(int rightOrLeft);

extern void lcd_function_set(int use8bit, int use2Lines, int use5x11Font);

extern void lcd_set_display_data_ram_address(int8_t address);

extern void lcd_write_data(int8_t data);

extern void lcd_send_data(int rs, uint8_t data);

extern void lcd_send_data4b(int rs, uint8_t data);

extern void lcd_flush();

#endif /* LCDPI_H_ */
