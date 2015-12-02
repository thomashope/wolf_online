#ifndef HEARTBEAT_PACKET_H
#define HEARTBEAT_PACKET_H

#include "BasePacket.h"

#define HEARTBEAT_PACKET_SIZE (1 + 1)

// Lets the recipient know the sender is alive

class HeartbeatPacket : public BasePacket
{
public:
	HeartbeatPacket() : BasePacket( PT_HEARTBEAT, HEARTBEAT_PACKET_SIZE ) {}
	~HeartbeatPacket() {}

	void SetID( Uint8 ID );

	Uint8 GetID();
};

#endif