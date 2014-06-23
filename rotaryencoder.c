//rotaryencoder.c
#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rotaryencoder.h"

int numberofencoders = 0;

void updateEncoders()
{
	struct encoder* encoder = &default_encoder;
        int MSB = digitalRead(encoder->pin_a);
        int LSB = digitalRead(encoder->pin_b);

        int encoded = (MSB << 1) | LSB;
        int sum = (encoder->lastEncoded << 2) | encoded;

	encoder->lastEncoded = encoded;
        if(sum == 0b1101 /**|| sum == 0b0100 || sum == 0b0010 || sum == 0b1011**/) encoder->direction=DIR_RIGHT;
        if(sum == 0b1110 /**|| sum == 0b0111 || sum == 0b0001 || sum == 0b0001**/) encoder->direction=DIR_LEFT;
}

void updateButton()
{
	printf("Interrupt Button\n");
	default_encoder.button_pressed=1;
}


struct encoder *setupencoder(int pin_a, int pin_b, int pin_button)
{
    struct encoder *newencoder = &default_encoder;
    newencoder->pin_a = pin_a;
    newencoder->pin_b = pin_b;
	newencoder->pin_button = pin_button;
	


    newencoder->lastEncoded = 0;
	newencoder->direction = DIR_NONE;
	newencoder->button_pressed = 0;

    pinMode(pin_a, INPUT);
    pinMode(pin_b, INPUT);
	pinMode(pin_button, INPUT);
    pullUpDnControl(pin_a, PUD_UP);
    pullUpDnControl(pin_b, PUD_UP);
    wiringPiISR(pin_a,INT_EDGE_BOTH, updateEncoders);
    wiringPiISR(pin_b,INT_EDGE_BOTH, updateEncoders);
	wiringPiISR(pin_button, INT_EDGE_FALLING, updateButton);

    return newencoder;
}



direction getDirection(struct encoder *encoder)
{
	direction ret=encoder->direction;
	if (ret)
	{

		encoder->direction= DIR_NONE;
	}
	return ret;
}


int getButtonPressed(struct encoder *encoder)
{
	int ret = encoder->button_pressed;
	if(ret)
	{
		encoder->button_pressed=0;
	}
	return ret;
}
