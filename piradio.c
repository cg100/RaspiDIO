#include <string.h>
#include <stdio.h>
#include <wiringPi.h>
#include "menu.h"
#include "helper.h"
#include "lcdpi.h"
#include "rotaryencoder.h"

void tmp_action(menu_item* item)
{
	printInfo(item->text);
}

int main(int argc, char **argv)
{
	struct encoder *encoder;
	int i = 0;

	printInfo("==================================================");
	printInfo("=====================PiRadio======================");
	printInfo("==================================================\n");

	globalDebugLevel = DEBUG_INFO;
	lcd_debug = 0;


	for(i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "debug") == 0)
		{
			globalDebugLevel = DEBUG_ALL;
			lcd_debug = 1;

			printDebug("DEBUG MODE...");
			printDebug("Press Any Key...");
			getchar();
		}
	}

	printDebug("wiringPiSetup...");
	wiringPiSetup();

	lcd_setup4b_schnatterente();

	delay(1000);

	lcd_line_print(LINE1, "RaspiDIO");

	delay(3000);

	encoder = setupencoder(9, 7, 8);


	//Hauptmenü starten
	printDebug("init Menue...");
	init_menu();
	selected_menu.active = 1;

	menu_item_add_subitem(item_main_menu, create_menu_item("Hallo", tmp_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Hallo2", tmp_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Hallo3", tmp_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Hallo4", tmp_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Hallo5", tmp_action, 0));

	printDebug("Zeichne Menue...");
	menu_draw();

	printDebug("Starte Hauptprogramm");
	while(1)
	{
		//Hauptprogramm


		//checkDirection
		//checkButtons

		direction dir = getDirection(encoder);

		if(dir==DIR_LEFT)
			menu_dec();
		if(dir==DIR_RIGHT)
			menu_inc();

		if(getButtonPressed(encoder)==1)
		{
			printf("Button pressed! \n");
			menu_action();
		}

		delay(100);
	}

	return 0;
}
