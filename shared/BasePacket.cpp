#include "BasePacket.h"

// reinterperate the bits of a float as a Uint32
// Assumes sizeof(float) == 4 !!!!
// Assumes the recipient and host use the same type of float !!!!
// http://stackoverflow.com/questions/20762952/most-efficient-standard-compliant-way-of-reinterpreting-int-as-float
Uint32 floatToUint32(float f)
{
	Uint32 i = 0;
	char* iPtr = (char*)&i;
	char* fPtr = (char*)&f;
	memcpy(iPtr, fPtr, 4);
	return i;
}

// reinterperates the bits of a Uint32 as a float
// Assumes sizeof(float) == 4 !!!!
// Assumes the recipient and host use the same type of float !!!!
float Uint32toFloat(Uint32 i)
{
	float f = 0;
	char* fPtr = (char*)&f;
	char* iPtr = (char*)&i;
	memcpy(fPtr, iPtr, 4);
	return f;
}