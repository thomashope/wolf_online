#include "HeartbeatPacket.h"

void HeartbeatPacket::SetID( Uint8 ID )
{
	data_[1] = ID;
}

Uint8 HeartbeatPacket::GetID()
{
	return data_[1];
}