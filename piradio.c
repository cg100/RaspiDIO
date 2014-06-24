#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "menu.h"
#include "helper.h"
#include "lcdpi.h"
#include "rotaryencoder.h"
#include "radio.h"
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>

struct encoder *encoder;
int radiostation = 0;

char * getTime()
{
	time_t current_time;
	struct tm * time_info;
	char *cur_time = 0;
	char timeString[20];  // space for "HH:MM\0"

	time(&current_time);
	time_info = localtime(&current_time);


	strftime(timeString, sizeof(timeString), "%H:%M", time_info);

	cur_time = (char*)calloc(strlen(timeString), sizeof(char));
	strcpy(cur_time, timeString);

	return cur_time;
}



void info_action(menu_item* item)
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	/* display result */
	printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	lcd_display_clear();
	lcd_line_print(LINE1, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	while(1)
	{
		//Menü aufrufen
		if(getButtonPressed(encoder)==1)
		{
			menu_draw();
			break;
		}
		delay(100);
	}

}

void quit_action(menu_item* item)
{
	printInfo(item->text);
	lcd_display_clear();
	lcd_line_print(LINE1, "System wird");
	lcd_line_print(LINE2, "heruntergefahren!");
	stop_mpd();
	system("init 0");
}

void start_action(menu_item* item)
{
	printInfo(item->text);
	start_mpd();
	now_playing();
}

void change_action(menu_item* item)
{
	printInfo(item->text);
	if(radiostation==0)
	{
		radiostation=1;
		change_radiostation(radiostation);
		now_playing();
		return;
	}
	if(radiostation==1)
	{
		radiostation=2;
		change_radiostation(radiostation);
		now_playing();
		return;
	}
	if(radiostation==2)
	{
		radiostation=0;
		change_radiostation(radiostation);
		now_playing();
		return;
	}
}

void stop_action(menu_item* item)
{
	printInfo(item->text);
	stop_mpd();
}

void now_playing()
{
	delay(500);

	//LCD Clear
	lcd_display_clear();


	lcd_line_print(LINE1, get_current_station(radiostation));


	char *title = get_current_songtitle();

	while(!title)
	{
		delay(100);
		printf("zeiger nicht da");
		title = get_current_songtitle();
	}

	lcd_line_print(LINE2, title);



	char *timenow = getTime();
	printf("%s\n",timenow);
	lcd_line_print(LINE4, timenow);
	printf("%s\n", title);




	while(1)
	{

		//aktuellen Titel ausgeben, wenn er sich geändert hat:
		if((strcmp(title,get_current_songtitle()) != 0))
		{
			free(title);
			//lcd_display_clear();
			lcd_line_clear(LINE2);
			title = get_current_songtitle();
			printf("%s\n", title);
			while(!title)
			{
				delay(100);
				printf("zeiger nicht da-loop");
				title = get_current_songtitle();
			}
			lcd_line_print(LINE2, title);
		}


		//aktuelle Uhrzeit ausgeben, wenn sie sich geändert hat
		if((strcmp(timenow,getTime()) != 0))
		{
			free(timenow);
			lcd_line_clear(LINE4);
			timenow = getTime();
			printf("%s\n", timenow);
			lcd_line_print(LINE4, timenow);
		}




		direction dir = getDirection(encoder);

		//Leiser
		if(dir==DIR_LEFT)
			set_volume_rel(-5);

		//Lauter
		if(dir==DIR_RIGHT)
			set_volume_rel(5);

		//Menü aufrufen
		if(getButtonPressed(encoder)==1)
		{
			menu_draw();
			break;
		}
		delay(100);


	}
	free(title);
	free(timenow);
}

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

	lcd_setup4b_schnatterente();

	delay(1000);

	lcd_line_print(LINE1, "RaspiDIO");

	delay(3000);


	init_mpd();


	//Hauptmenü starten
	printDebug("init Menue...");
	init_menu();
	selected_menu.active = 1;

	menu_item_add_subitem(item_main_menu, create_menu_item("Start", start_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Stop", stop_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Sender wechseln", change_action, 0));
	//menu_item_add_subitem(item_main_menu, create_menu_item("Zurueck", now_playing, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Info", info_action, 0));
	menu_item_add_subitem(item_main_menu, create_menu_item("Beenden", quit_action, 0));

	printDebug("Zeichne Menue...");
	menu_draw();

	printDebug("Starte Hauptprogramm");


	encoder = setupencoder(9, 7, 8);
	while(1)
	{
		//Hauptprogramm


		//checkDirection
		direction dir = getDirection(encoder);

		if(dir==DIR_LEFT)
		{
			menu_dec();

		}
		if(dir==DIR_RIGHT)
		{
			menu_inc();
		}

		//checkButton
		if(getButtonPressed(encoder)==1)
		{
			printf("Button pressed! \n");
			menu_action();
		}



		delay(200);
	}

	close_mpd();
	return 0;
}
