#include "HeartbeatPacket.h"

HeartbeatPacket::HeartbeatPacket() :
BasePacket( PT_HEARTBEAT, HEARTBEAT_PACKET_SIZE )
{
}

HeartbeatPacket::HeartbeatPacket( Uint8 ID ) :
HeartbeatPacket()
{
	SetID( ID );
}

void HeartbeatPacket::SetID( Uint8 ID )
{
	data_[1] = ID;
}

Uint8 HeartbeatPacket::GetID()
{
	return data_[1];
}
