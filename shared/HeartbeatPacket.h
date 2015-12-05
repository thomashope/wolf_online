#ifndef HEARTBEAT_PACKET_H
#define HEARTBEAT_PACKET_H

#include "BasePacket.h"

#define HEARTBEAT_PACKET_SIZE (1 + 1)

// Lets the recipient know the sender is alive

class HeartbeatPacket : public BasePacket
{
public:
	HeartbeatPacket()			: BasePacket( PT_HEARTBEAT, HEARTBEAT_PACKET_SIZE ) {}
	HeartbeatPacket( Uint8 ID ) : HeartbeatPacket() { SetID( ID ); }
	~HeartbeatPacket() {}

	void SetID( Uint8 ID ) { data_[1] = ID; }

	Uint8 GetID() const { return data_[1]; }

	void Print() const override {
		std::cout << "Player ID: " << (int)GetID() << " sent heartbeat" << std::endl;
	}
};

#endif
