/*
 * helper.h
 *
 *  Created on: May 8, 2014
 *      Author: psanetra
 */

#ifndef HELPER_H_
#define HELPER_H_

typedef enum _debug_level
{
	DEBUG_ALL = 0x00,
	DEBUG_DEBUG = 0x01,
	DEBUG_INFO = 0x02,
	DEBUG_WARNING = 0x04,
	DEBUG_ERROR = 0x08,
	DEBUG_FATAL = 0x10,
	DEBUG_OFF = 0xFF
} debug_level;

debug_level globalDebugLevel;

extern void debugPrint(debug_level level, char* text);

extern void debugPrintLn(debug_level level, char* text);

extern void debugPrintHex(debug_level level, int integer);

extern void printDebug(char* text);

extern void printDebugHex(int integer);

extern void printInfo(char* text);

extern void printError(char* text);

extern void printFatal(char* text);

#endif /* HELPER_H_ */
