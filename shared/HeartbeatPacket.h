#ifndef HEARTBEAT_PACKET_H
#define HEARTBEAT_PACKET_H

#include "BasePacket.h"

#define HEARTBEAT_PACKET_SIZE (1 + 1)

// Lets the recipient know the sender is alive

class HeartbeatPacket : public BasePacket
{
public:
	HeartbeatPacket();
	HeartbeatPacket( Uint8 ID );

	~HeartbeatPacket() {}

	void SetID( Uint8 ID );

	Uint8 GetID();
};

#endif
