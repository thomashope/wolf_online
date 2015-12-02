#include "MovePacket.h"
#include <SDL2/SDL_net.h>

void MovePacket::SetID( Uint8 ID )
{
	data_[1] = ID;
}

void MovePacket::SetPosition(Vec2 position)
{
	SDLNet_Write32( floatToUint32( position.x ), &data_[2] );
	SDLNet_Write32( floatToUint32( position.y ), &data_[6] );
}

void MovePacket::SetVelocity(Vec2 velocity)
{
	SDLNet_Write32( floatToUint32( velocity.x ), &data_[10] );
	SDLNet_Write32( floatToUint32( velocity.y ), &data_[14] );
}

void MovePacket::SetAngle(float degrees)
{
	SDLNet_Write32( floatToUint32( degrees ), &data_[18] );
}

Uint8 MovePacket::GetID()
{
	return data_[1];
}

Vec2 MovePacket::GetPosition()
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[2] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[6] ) )
		);
}

Vec2 MovePacket::GetVelocity()
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[10] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[14] ) )
		);
}

float MovePacket::GetAngle()
{
	return Uint32toFloat( SDLNet_Read32( &data_[17] ) );
}