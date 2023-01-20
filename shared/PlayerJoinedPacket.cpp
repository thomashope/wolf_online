#include "PlayerJoinedPacket.h"
#include <SDL_net.h>

void PlayerJoinedPacket::SetPosition( Vec2 position )
{
	SDLNet_Write32( floatToUint32( position.x ), &data_[2] );
	SDLNet_Write32( floatToUint32( position.y ), &data_[6] );
}

Vec2 PlayerJoinedPacket::GetPosition( ) const
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[2] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[6] ) )
		);
}