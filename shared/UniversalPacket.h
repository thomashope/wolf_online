#ifndef UNIVERSAL_PACKET_H
#define UNIVERSAL_PACKET_H

#include <memory>
#include <SDL2/SDL.h>

#include "../shared/JoinRequestPacket.h"
#include "../shared/JoinResponsePacket.h"
#include "../shared/InfoRequestPacket.h"
#include "../shared/MapDataPacket.h"
#include "../shared/MovePacket.h"
#include "../shared/HeartbeatPacket.h"
#include "../shared/PlayerJoinedPacket.h"
#include "../shared/SyncPacket.h"

// Must be larger than the largest packet that will be recvd
#define UNIVERSAL_PACKET_SIZE 640

class UniversalPacket
{
public:
	UniversalPacket() :
		size_(UNIVERSAL_PACKET_SIZE),
		data_(new Uint8[UNIVERSAL_PACKET_SIZE]())
	{}

	~UniversalPacket() {}

	// returns the contents of the packet ready for sending
	Uint8* Data() const { return data_.get(); }

	// return the number of bytes of data in the packet
	unsigned int Size() const { return size_; }

	// creates a packet based on the data in the universal packet
	std::unique_ptr<BasePacket> CreateFromContents();

private:
	std::unique_ptr<Uint8[]> data_;

	// size of the pakcet in bytes
	unsigned int size_;

	std::unique_ptr<BasePacket> ConstructPacket(BasePacket* packet);
};

#endif
