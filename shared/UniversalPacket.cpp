#include "UniversalPacket.h"

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
	case PT_INFO_REQUEST:
		return ConstructPacket( new InfoRequestPacket );
	case PT_MAP_DATA:
		return ConstructPacket( new MapDataPacket );
	case PT_MOVE:
		return ConstructPacket( new MovePacket );
	case PT_PLAYER_JOINED:
		return ConstructPacket( new PlayerJoinedPacket );
	case PT_PLAYER_DISCONNECTED:
		return ConstructPacket( new PlayerDisconnectedPacket );
	case PT_SYNC:
		return ConstructPacket( new SyncPacket );
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
