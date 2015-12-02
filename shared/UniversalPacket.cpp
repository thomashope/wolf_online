#include "UniversalPacket.h"
#include "HeartbeatPacket.h"
#include "JoinRequestPacket.h"
#include "JoinResponsePacket.h"
#include "MapRequestPacket.h"
#include "MapResponsePacket.h"
#include "MovePacket.h"

std::unique_ptr<BasePacket> UniversalPacket::CreateFromContents()
{
	// detect the packet type
	// construct the packet based on type
	switch (data_[0])
	{
	case PT_HEARTBEAT:
		return ConstructPacket( new HeartbeatPacket );
	case PT_JOIN_REQUEST:
		return ConstructPacket( new JoinRequestPacket );
	case PT_JOIN_RESPONSE:
		return ConstructPacket( new JoinResponsePacket );
	case PT_MAP_REQUEST:
		return ConstructPacket( new MapRequestPacket );
	case PT_MAP_RESPONSE:
		return ConstructPacket( new MapResponsePacket );
	case PT_MOVE:
		return ConstructPacket( new MovePacket );
	default:
		return nullptr;
	}

}

std::unique_ptr<BasePacket> UniversalPacket::ConstructPacket(BasePacket* packet)
{
	// copy the data into the packet
	memcpy(packet->Data(), data_.get(), packet->Size());

	// move out the packet
	return std::unique_ptr<BasePacket>(packet);
}

