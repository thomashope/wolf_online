#include "PlayerJoinedPacket.h"

void PlayerJoinedPacket::SetID( Uint8 ID )
{
	data_[1] = ID;
}

Uint8 PlayerJoinedPacket::GetID() const
{
	return data_[1];
}

void PlayerJoinedPacket::Print() const
{
	std::cout << "Player online: " << (int)GetID() << std::endl;
}
