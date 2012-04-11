#ifndef _PPM_H_
#define _PPM_H_

#include <inttypes.h>

#define RC_MAX_VALUE 		100
#define RC_MIN_VALUE 		-100
#define TIMER_VALUE_MAX 	65535
#define RC_MAX_CHANNEL 		7
#define RC_REPETITION_TIME 	25000 //25 ms
#define MS0_5 				500
#define RC_OFFSET_TIME 		1000

#define PPM_PORT			PORTE
#define PPM_DDR				DDRE
#define PPM_PIN				PE0

extern volatile int8_t rc_ch[RC_MAX_CHANNEL];

void PPM_Init(void);

#endif
