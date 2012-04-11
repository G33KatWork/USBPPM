#include "Debug.h"

void Debug_Init()
{
#ifdef DEBUG
	Serial_Init(9600, false);
	Serial_CreateStream(NULL);
#endif
}
