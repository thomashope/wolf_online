#include "JoinRequestPacket.h"
#include <SDL2/SDL_net.h>

void JoinRequestPacket::SetPosition( Vec2 position )
{
	SDLNet_Write32( floatToUint32( position.x ), &data_[1] );
	SDLNet_Write32( floatToUint32( position.y ), &data_[5] );
}

Vec2 JoinRequestPacket::GetPosition( ) const
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[1] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[5] ) )
		);
}