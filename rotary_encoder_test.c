#include "rotaryencoder.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void main()
{
wiringPiSetup();
struct encoder *encoder = setupencoder(9, 7, 8);



while(1)
{
	direction dir = getDirection(encoder);
	
	if(dir==DIR_LEFT)
		printf("<-\n");
	if(dir==DIR_RIGHT)
		printf("->\n");
	
	if(getButtonPressed(encoder)==1)
		printf("Button pressed! \n");
	delay(100);
}



return ;
}
