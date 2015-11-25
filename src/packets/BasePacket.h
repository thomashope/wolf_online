#ifndef BASEPACKET_H
#define BASEPACKET_H

#include <iostream>
#include <SDL2/SDL.h>

// First byte of a packet should always be the type
enum PacketType {
	PT_HEARTBEAT = 0,
	PT_JOIN_REQUEST = 1,
	PT_JOIN_RESPONSE = 2,
	PT_MAP_REQUEST = 3,
	PT_MAP_RESPONSE = 4
};

class BasePacket
{
protected:
	BasePacket( int size ) :data_( nullptr ), size_( size )
	{
		// allocates size bytes for packet data
		// initalise array to zero thanks to []();
		data_ = new Uint8[size_]();
	};

	virtual ~BasePacket()
	{
		if( data_ ) {
			delete[] data_;
			data_ = nullptr;
		}
	};

	// The contents of the packet
	Uint8* data_;

	// the number of bytes of data in the packet
	int size_;

public:
	// returns the contents of the packet ready for sending
	virtual Uint8* Data() final { return data_; }

	// return the number of bytes of data in the packet
	virtual int Size() final { return size_; }
};

// Helper functions for packing

// reinterperate the bits of a float as a Uint32
// Assumes sizeof(float) == 4 !!!!
// Assumes the recipient and host use the same type of float !!!!
// http://stackoverflow.com/questions/20762952/most-efficient-standard-compliant-way-of-reinterpreting-int-as-float
Uint32 floatToUint32( float f )
{
	Uint32 i = 0;
	char* iPtr = (char*)&i;
	char* fPtr = (char*)&f;
	memcpy( iPtr, fPtr, 4 );
	return i;
}

// reinterperates the bits of a Uint32 as a float
// Assumes sizeof(float) == 4 !!!!
// Assumes the recipient and host use the same type of float !!!!
float Uint32toFloat( Uint32 i )
{
	float f = 0;
	char* fPtr = (char*)&f;
	char* iPtr = (char*)&i;
	memcpy( fPtr, iPtr, 4 );
	return f;
}

#endif