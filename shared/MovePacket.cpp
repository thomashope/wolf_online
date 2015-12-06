#include "MovePacket.h"
#include <SDL2/SDL_net.h>

void MovePacket::SetID( Uint8 ID )
{
	data_[1] = ID;
}

void MovePacket::SetTime( Uint32 time )
{
	SDLNet_Write32( time, &data_[2] );
}

void MovePacket::SetPosition(Vec2 position)
{
	SDLNet_Write32( floatToUint32( position.x ), &data_[6] );
	SDLNet_Write32( floatToUint32( position.y ), &data_[10] );
}

void MovePacket::SetVelocity(Vec2 velocity)
{
	SDLNet_Write32( floatToUint32( velocity.x ), &data_[14] );
	SDLNet_Write32( floatToUint32( velocity.y ), &data_[18] );
}

void MovePacket::SetAngle(float degrees)
{
	SDLNet_Write32( floatToUint32( degrees ), &data_[22] );
}

Uint8 MovePacket::GetID() const
{
	return data_[1];
}

Uint32 MovePacket::GetTime() const
{
	return SDLNet_Read32( &data_[2] );
}

Vec2 MovePacket::GetPosition() const
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[6] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[10] ) )
		);
}

Vec2 MovePacket::GetVelocity() const
{
	return Vec2(
		Uint32toFloat( SDLNet_Read32( &data_[14] ) ),
		Uint32toFloat( SDLNet_Read32( &data_[18] ) )
		);
}

float MovePacket::GetAngle() const
{
	return Uint32toFloat( SDLNet_Read32( &data_[22] ) );
}

void MovePacket::Print() const
{
	std::cout << "ID: " << (int)GetID()
		<< " Pos: " << GetPosition().x << " " << GetPosition().y
		<< " Vel: " << GetVelocity().x << " " << GetVelocity().y
		<< " Deg: " << GetAngle() << std::endl;
}
