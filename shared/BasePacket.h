#ifndef BASEPACKET_H
#define BASEPACKET_H

#include <iostream>
#include <memory>
#include <SDL_net.h>

// First byte of a packet should always be the type
enum PacketType : Uint8 {
	PT_HEARTBEAT = 0,
	PT_JOIN_REQUEST = 1,
	PT_JOIN_RESPONSE = 2,
	PT_PLAYER_JOINED = 3,
	PT_PLAYER_DISCONNECTED = 4,
	PT_INFO_REQUEST = 5,
	PT_MAP_DATA = 6,
	PT_MOVE = 7,
	PT_SYNC = 8,
	PT_UNKNOWN = 0xff
};

class BasePacket
{
protected:
	BasePacket( PacketType type, unsigned int size ) :
		size_((size > 1) ? size : 1)	// enforce size as always greater than 1
	{
		data_ = std::unique_ptr<Uint8[]>(new Uint8[size_]());	// allocates size bytes for packet data
																// initalise array to zero thanks to []();

		// set the first byte of the packet to the type
		data_[0] = type;
	}

	// The contents of the packet
	// first byte is type
	// additional bytes are defined in the sub class
	std::unique_ptr<Uint8[]> data_;

	// the number of bytes of data in the packet
	unsigned int size_;

public:
	virtual ~BasePacket() {}

	// returns the type of the packet
	// packet types cannoct be changed after construction ...
	// ... but could be PT_UNKNOWN
	virtual PacketType Type() const final { return (PacketType)data_[0]; }

	// returns the contents of the packet ready for sending
	virtual Uint8* Data() const final { return data_.get(); }

	// return the number of bytes of data in the packet
	virtual unsigned int Size() const final { return size_; }

	// Print the contents of the packe to cout
	virtual void Print() const { std::cout << "Packet print not defined" << std::endl; }
};

// Helper functions for packing

// reinterperate the bits of a float as a Uint32
// Assumes sizeof(float) == 4 !!!!
// Assumes the recipient and host use the same type of float !!!!
// http://stackoverflow.com/questions/20762952/most-efficient-standard-compliant-way-of-reinterpreting-int-as-float
Uint32 floatToUint32(float f);

// reinterperates the bits of a Uint32 as a float
// Assumes sizeof(float) == 4 !!!!
// Assumes the recipient and host use the same type of float !!!!
float Uint32toFloat(Uint32 i);

#endif
