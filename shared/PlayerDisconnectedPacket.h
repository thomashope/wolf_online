#ifndef PLAYERDISCONNECTED_PACKET_H
#define PLAYERDISCONNECTED_PACKET_H

#include "vec2.h"
#include "BasePacket.h"

#define PLAYERDISCONNECTED_PACKET_SIZE (1 + 1)

// Tells other clients

class PlayerDisconnectedPacket : public BasePacket
{
public:
	PlayerDisconnectedPacket() : BasePacket( PT_PLAYER_DISCONNECTED, PLAYERDISCONNECTED_PACKET_SIZE ) {}
	PlayerDisconnectedPacket( Uint8 ID ) : PlayerDisconnectedPacket() { SetID( ID ); }
	~PlayerDisconnectedPacket() {}

	void SetID( Uint8 ID ) { data_[1] = ID; }

	Uint8 GetID() const { return data_[1]; }

	void Print() const override {
		std::cout << "Player ID: " << (int)GetID( ) << " disconnected" << std::endl;
	}
};

#endif
