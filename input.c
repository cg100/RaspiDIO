#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "lirc/lirc_client.h"
#include "rotaryencoder.h"

struct encoder *encoder;
struct lirc_config *lirc_config;



void initInputs()
{
	printf("INIT DREH");
	//Drehencoder
	wiringPiSetup();
	encoder = setupencoder(9, 7, 8);

	printf("INIT Infrarot");
	//Infrarot
	lirc_init("irexec",1);
	lirc_readconfig(NULL,&lirc_config, NULL);
	//Socket non blocking
	fcntl(lirc_socket.F_SETOWN,getpid());
	flags=fcntl(lirc_socket,F_GETFL,0);
	fcntl(lirc_socket,F_SETFL,flags|O_NONBLOCK);

}

button getButton(struct lirc_config *lirc_config)
{
	char *code;
	char *c;
	int ret;


	lirc_nextcode(&code);
	ret=lirc_code2char(lirc_config,code,&c);

	printf("%d", ret);
	return BUTTON_NONE;
}


void disposeInputs()
{
	lirc_freeconfig(lirc_config);
	lirc_deinit();
}

void main()
{
	printf("Main");
	initInputs();
	printf("MAIN nach init\n");
	//Hauptschleife
	while(1)
	{
		//printf("Schleife");
		direction dir = getDirection(encoder);
		button but = getButton(lirc_config);

		if(dir==DIR_LEFT)
			printf("<-\n");
		if(dir==DIR_RIGHT)
			printf("->\n");

		if(getButtonPressed(encoder)==1)
			printf("Button pressed! \n");
		delay(100);
	}


	//disposeInputs();
	return ;
}




