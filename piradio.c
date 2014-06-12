#include <string.h>
#include <stdio.h>
#include <wiringPi.h>
#include "helper.h"
#include "lcdpi.h"

int main(int argc, char **argv)
{
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

	//Hauptmenü starten

	while(1)
	{
		//Hauptprogramm

		//checkDirection
		//checkButtons

		delay(10);
	}

	return 0;
}
