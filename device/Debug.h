#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <LUFA/Drivers/Peripheral/Serial.h>

//#define DEBUG
#ifdef DEBUG
    #define dbg_P(...)   printf_P(__VA_ARGS__)
#else
	#define dbg_P(...)
#endif

void Debug_Init(void);

#endif
