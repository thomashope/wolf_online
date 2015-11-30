#include "MovePacket.h"
#include <SDL2/SDL_net.h>

void MovePacket::SetPosition(Vec2 position)
{
	SDLNet_Write32( floatToUint32( position.x ), &data_[1] );
	SDLNet_Write32( floatToUint32( position.y ), &data_[5] );
}

void MovePacket::SetVelocity(Vec2 velocity)
{
	SDLNet_Write32( floatToUint32( velocity.x ), &data_[9] );
	SDLNet_Write32( floatToUint32( velocity.y ), &data_[13] );
}

void MovePacket::SetAngle(float degrees)
{
	SDLNet_Write32( floatToUint32( degrees ), &data_[17] );
}

Vec2 MovePacket::GetPosition()
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[1] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[5] ) )
		);
}

Vec2 MovePacket::GetVelocity()
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[9] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[13] ) )
		);
}

float MovePacket::GetAngle()
{
	return Uint32toFloat( SDLNet_Read32( &data_[17] ) );
}