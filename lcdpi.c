#include <stdio.h>
#include <wiringPi.h>
#include "helper.h"
#include "lcdpi.h"

#define LCD_RS	11
#define LCD_E	10
#define LCD_D4	6
#define LCD_D5	5
#define LCD_D6	4
#define LCD_D7	1

/*
GPIO Mapping:
 -   -
 8   -
 9   -
 7  15
 -  15
 0   1
 2   -
 3   4
 -   5
12   -
13   6
14  10
 -  11
 ===S-Video Port===
 */

void lcd_setup_pins()
{
	printDebug("lcd_setup_pins...");

	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_E, OUTPUT);
	pinMode(LCD_D7, OUTPUT);
	pinMode(LCD_D6, OUTPUT);
	pinMode(LCD_D5, OUTPUT);
	pinMode(LCD_D4, OUTPUT);
}

void lcd_setup4b()
{
	printDebug("lcd_setup4b...");

	lcd_setup_pins();

	//zur Sicherheit Enable mal auf 0 setzen
	digitalWrite(LCD_E, LOW);

	//nach POWER ON >40ms warten
	delay(50);

	//DB5, !DB4
	//DB7, DB6
	lcd_function_set(0, 1, 1);

	//>4.1ms warten
	delay(50);

	//DB5, !DB4
	//DB7, DB6
	lcd_function_set(0, 1, 1);

	//>100us warten
	delay(1);

	//DB5, !DB4
	//DB7, DB6
	lcd_function_set(0, 1, 1);

	//DB5, !DB4
	//DB7, DB6
	lcd_function_set(0, 1, 1);

	//0
	//DB7, DB6, DB5, !DB4
	lcd_display_on_off(0, 0, 0);

	//0
	//DB4
	lcd_display_clear();

	//Direction right
	//0
	//DB6, DB5,!DB4
	lcd_entry_mode_set(1, 0);

	//Diesplay wieder einschalten
	lcd_display_on_off(1, 1, 0);
}

/*alt
void lcd_setup4b()
{
	printDebug("lcd_setup4b...");

	lcd_setup_pins();

	//zur Sicherheit Enable mal auf 0 setzen
	digitalWrite(LCD_E, LOW);

	//nach POWER ON >40ms warten
	delay(50);

	//halbes Function Set DB5, DB4
	lcd_send_data4b(LOW, 0x03);

	//>37us warten
	delayMicroseconds(1000);

	//DB5, !DB4
	//DB7, DB6
	lcd_function_set(0, 1, 1);

	//DB5, !DB4
	//DB7, DB6
	lcd_function_set(0, 1, 1);

	//0
	//DB7, DB6, DB5, !DB4
	lcd_display_on_off(1, 1, 0);

	//0
	//DB4
	lcd_display_clear();

	//Direction right
	//0
	//DB6, DB5,!DB4
	lcd_entry_mode_set(1, 0);
}
*/

void lcd_setup4b_schnatterente()
{
	printDebug("lcd_setup4b_schnatterente...");

	lcd_setup_pins();

	lcd_send_data(0, 0x33);
	lcd_send_data(0, 0x32);
	lcd_send_data(0, 0x28);
	lcd_send_data(0, 0x0C);
	lcd_send_data(0, 0x06);
	lcd_send_data(0, 0x01);
}

void lcd_setup4b_ks0066()
{
	printDebug("lcd_setup4b_ks0066...");

	lcd_setup_pins();

	//nach POWER ON >30ms warten
	delay(50);

	//Function_Set
	lcd_send_data4b(0, 0x02);
	lcd_send_data4b(0, 0x02);
	lcd_send_data4b(0, 0x08 | 0x04);	//2-Line, Display On

	delayMicroseconds(50);

	//Display On/Off Control
	lcd_send_data4b(0, 0);
	lcd_send_data4b(0, 0x08 | 0x04 | 0x02);	//Display on, cursor on

	delayMicroseconds(50);

	//Display Clear
	lcd_send_data4b(0, 0);
	lcd_send_data4b(0, 1);

	//> 1.53ms
	delay(5);

	//Entry Mode Set
	lcd_send_data4b(0, 0x04 | 0x02);	//Increment, shift on

	delay(2);
}

void lcd_print(char* c)
{
	if(c == 0) return;

	printDebug("lcd_print:");
	printDebug(c);
	printDebug("");

	for(;*c;c++) lcd_write_data(*c);

	//lcd_cursor_shift(1);
}

void lcd_line_print(lcd_line line, char* c)
{
	int8_t address = 0;

	switch(line)
	{
		case LINE1: address = 0; break;
		case LINE2: address = 60; break;
		case LINE3: address = 20; break;
		case LINE4: address = 84; break;
		default: break;
	}

	lcd_set_display_data_ram_address(address);

	lcd_print(c);
}

void lcd_display_clear()
{
	uint8_t data = 0x01;

	lcd_send_data(0, data);

	//warte >1,52ms
	delay(10);
}

void lcd_line_clear(lcd_line line)
{
	lcd_line_print(line, "                    ");
}

void lcd_return_home()
{
	uint8_t data = 0x02;

	lcd_send_data(0, data);

	//warte >1,52ms
	delay(10);
}

/**
 *
 * Wenn doDisplayShift == 1
 * --> Wenn i/d == 1 wird nach links geshiftet
 * --> Wenn i/d == 0 wird nach rechts geshiftet
 */
void lcd_entry_mode_set(int incOrDecCursor, int doDisplayShift)
{
	uint8_t data = 0x04;

	data |= (incOrDecCursor != 0) << 1;
	data |= (doDisplayShift != 0);

	lcd_send_data(0, data);

	//warte >37us
	delayMicroseconds(100);
}

void lcd_display_on_off(int displayOn, int cursorOn, int cursorBlink)
{
	uint8_t data = 0x08;

	data |= (displayOn != 0) << 2;
	data |= (cursorOn != 0) << 1;
	data |= (cursorBlink != 0);

	lcd_send_data(0, data);

	//warte >37us
	delayMicroseconds(100);
}

void lcd_cursor_or_display_shift(int displayOrCursor, int rightOrLeft)
{
	uint8_t data = 0x10;

	data |= (displayOrCursor != 0) << 8;
	data |= (rightOrLeft != 0) << 4;

	lcd_send_data(0, data);

	//warte >1,52ms
	delayMicroseconds(100);
}

void lcd_display_shift(int rightOrLeft)
{
	lcd_cursor_or_display_shift(1, rightOrLeft);
}

void lcd_cursor_shift(int rightOrLeft)
{
	lcd_cursor_or_display_shift(0, rightOrLeft);
}

void lcd_function_set(int use8bit, int use2Lines, int use5x11Font)
{
	uint8_t data = 0x20;

	data |= (use8bit != 0) << 4;
	data |= (use2Lines != 0) << 3;
	data |= (use5x11Font != 0) << 2;

	lcd_send_data(0, data);

	//warte >37us
	delayMicroseconds(100);
}

/**
 * min_address: 0
 * max_address: 7f
 */
void lcd_set_display_data_ram_address(int8_t address)
{
	uint8_t data = 0x80;

	data |= address;

	lcd_send_data(0, data);

	//warte >37us
	delayMicroseconds(100);
}

void lcd_write_data(int8_t data)
{
	lcd_send_data(1, data);

	//warte >37us
	delayMicroseconds(100);
}

void lcd_send_data(int rs, uint8_t data)
{
	/*
	printDebug("lcd_send_data:");

	debugPrint(DEBUG_DEBUG, "RS: ");
	printDebugHex(rs);
	printDebug("");

	debugPrint(DEBUG_DEBUG, "DATA: ");
	printDebugHex(data);
	printDebug("\n");

	printDebug("{");
	*/

	lcd_send_data4b(rs, (data & 0xf0) >> 4);
	lcd_send_data4b(rs, data & 0x0f);

	//printDebug("}");
}

void lcd_send_data4b(int rs, uint8_t data)
{
	/*
	printDebug("lcd_send_data4b:");

	debugPrint(DEBUG_DEBUG, "RS: ");
	printDebugHex(rs);
	printDebug("");

	debugPrint(DEBUG_DEBUG, "DATA: ");
	printDebugHex(data);
	printDebug("");
	*/

	digitalWrite(LCD_RS, rs != 0);

	digitalWrite(LCD_D7, (data & 0x08) != 0);
	digitalWrite(LCD_D6, (data & 0x04) != 0);
	digitalWrite(LCD_D5, (data & 0x02) != 0);
	digitalWrite(LCD_D4, (data & 0x01) != 0);

	lcd_flush();

	/*
	if(lcd_debug == 1)
	{
		printDebug("Druecke eine beliebige Taste...\n");
		getchar();
	}
	*/
}

void lcd_flush()
{
	delayMicroseconds(50); //hier Address Setup Time (0ns)

	digitalWrite(LCD_E, HIGH);

	delayMicroseconds(50);	//PulseWidth eigentlich 460ns

	digitalWrite(LCD_E, LOW);

	delayMicroseconds(50);	//auch nochmal ähnlich lange wie PuleWidth warten
}
