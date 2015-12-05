#include "player.h"
#include "world.h"
#include "input.h"
#include "../shared/MovePacket.h"

Player::Player() :
pos( 2.0f, 2.0f ),
dir( -1.0f, 0.0f ),
plane( 0.0f, 0.66f ),
moveSpeed_( 5.0f ),
rotSpeed_( 90.0f )
{
}

void Player::Update( const World& world, const Input& input, float deltaTime )
{
	float speed = moveSpeed_ * deltaTime;
	float rotation = rotSpeed_ * deltaTime;
	if( input.XMotion( ) > 0 ) rotation *= input.XMotion( ) *  0.2f;
	else if( input.XMotion( ) < 0 ) rotation *= input.XMotion( ) * -0.2f;

	//strafe to the right
	if( input.KeyDown( SDL_SCANCODE_D ) )
	{
		if( world.GetGrid( int( pos.x + dir.y * speed ), int( pos.y ) ) == false ) pos.x += dir.y * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y + -dir.x * speed ) ) == false ) pos.y += -dir.x * speed;
		//sendPosition = true;
	}
	//strafe to the left
	if( input.KeyDown( SDL_SCANCODE_A ) )
	{
		if( world.GetGrid( int( pos.x + -dir.y * speed ), int( pos.y ) ) == false ) pos.x += -dir.y * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y + dir.x * speed ) ) == false )  pos.y += dir.x * speed;
		//sendPosition = true;
	}
	//move forward if no wall in front of you
	if( input.KeyDown( SDL_SCANCODE_W ) )
	{
		if( world.GetGrid( int( pos.x + dir.x * speed ), int( pos.y ) ) == false ) pos.x += dir.x * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y + dir.y * speed ) ) == false ) pos.y += dir.y * speed;
		//sendPosition = true;
	}
	//move backwards if no wall behind you
	if( input.KeyDown( SDL_SCANCODE_S ) )
	{
		if( world.GetGrid( int( pos.x - dir.x * speed ), int( pos.y ) ) == false ) pos.x -= dir.x * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y - dir.y * speed ) ) == false ) pos.y -= dir.y * speed;
		//sendPosition = true;
	}
	//rotate to the right
	if( input.XMotion() > 0 )
	{
		dir = dir.rotate( -rotation );
		plane = plane.rotate( -rotation );
	}
	//rotate to the left
	if( input.XMotion() < 0 )
	{
		dir = dir.rotate( rotation );
		plane = plane.rotate( rotation );
	}
}

bool Player::MovedSignificantly()
{
	static Vec2 lastPos = pos;

	if( timeSinceLastMove_ - SDL_GetTicks() > 1000 &&
		pos.x != lastPos.x )
	{
		lastPos = pos;
		return true;
	}
	else
	{
		return false;
	}

	//TODO
	// if enough time has passed min, max
	// they have moved a bit
	// they have rotated quite a bit (45* at least ?) 
}

BasePacket* Player::GetMovePacket()
{
	timeSinceLastMove_ = SDL_GetTicks();

	MovePacket* mp = new MovePacket();
	mp->SetID( ID );
	mp->SetPosition( pos );
	//TODO: mp->SetVelocity(
	//TODO: mp->SetAngle(

	return (BasePacket*)mp;
}