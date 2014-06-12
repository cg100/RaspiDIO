/*
 * helper.c
 *
 *  Created on: May 8, 2014
 *      Author: psanetra
 */

#include <stdio.h>
#include "helper.h"

void debugPrint(debug_level level, char* text)
{
	if(!text || globalDebugLevel > level) return;

	printf("%s", text);
}

void debugPrintLn(debug_level level, char* text)
{
	if(!text || globalDebugLevel > level) return;

	printf("%s\n", text);
}

void debugPrintHex(debug_level level, int integer)
{
	if(globalDebugLevel > level) return;

	printf("%x", integer);
}

void printDebug(char* text)
{
	debugPrintLn(DEBUG_DEBUG, text);
}

void printDebugHex(int integer)
{
	debugPrintHex(DEBUG_DEBUG, integer);
}

void printInfo(char* text)
{
	debugPrintLn(DEBUG_INFO, text);
}

void printError(char* text)
{
	debugPrintLn(DEBUG_ERROR, text);
}

void printFatal(char* text)
{
	debugPrintLn(DEBUG_FATAL, text);
}
