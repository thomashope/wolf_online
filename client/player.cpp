#include "player.h"
#include "world.h"
#include "input.h"
#include "../shared/MovePacket.h"

Player::Player() :
pos( 2.0f, 2.0f ),
dir( -1.0f, 0.0f ),
plane( 0.0f, 0.66f ),
moveSpeed_( 5.0f ),
rotSpeed_( 90.0f ),
send_move_( false )
{
}

void Player::Update( const World& world, const Input& input, float deltaTime )
{
	float speed = moveSpeed_ * deltaTime;
	float rotation = rotSpeed_ * deltaTime;
	if( input.XMotion() > 0 ) rotation *= input.XMotion() *  0.2f;
	else if( input.XMotion() < 0 ) rotation *= input.XMotion() * -0.2f;

	Vec2 OldPos = pos;
	Vec2 oldVel = vel_;

	//strafe to the right
	if( input.KeyDown( SDL_SCANCODE_D ) )
	{
		if( world.GetGrid( int( pos.x + dir.y * speed ), int( pos.y ) ) == false ) pos.x += dir.y * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y + -dir.x * speed ) ) == false ) pos.y += -dir.x * speed;
	}
	//strafe to the left
	if( input.KeyDown( SDL_SCANCODE_A ) )
	{
		if( world.GetGrid( int( pos.x + -dir.y * speed ), int( pos.y ) ) == false ) pos.x += -dir.y * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y + dir.x * speed ) ) == false )  pos.y += dir.x * speed;
	}
	//move forward if no wall in front of you
	if( input.KeyDown( SDL_SCANCODE_W ) )
	{
		if( world.GetGrid( int( pos.x + dir.x * speed ), int( pos.y ) ) == false ) pos.x += dir.x * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y + dir.y * speed ) ) == false ) pos.y += dir.y * speed;
		vel_.x = dir.x * moveSpeed_;
		vel_.y = dir.y * moveSpeed_;
	}
	else vel_ = Vec2( 0, 0 );
	//move backwards if no wall behind you
	if( input.KeyDown( SDL_SCANCODE_S ) )
	{
		if( world.GetGrid( int( pos.x - dir.x * speed ), int( pos.y ) ) == false ) pos.x -= dir.x * speed;
		if( world.GetGrid( int( pos.x ), int( pos.y - dir.y * speed ) ) == false ) pos.y -= dir.y * speed;
	}

	// The velocity if the difference between the new and old position
	// per second
	vel_ = pos - OldPos;
	vel_ *= (1.0f / deltaTime);

	if( (vel_.x > 0 && oldVel.x < 0) ||	// send a move packet
		(vel_.x < 0 && oldVel.x > 0) ||	// if there was a sudden change of direction
		(vel_.y > 0 && oldVel.y < 0) ||
		(vel_.y < 0 && oldVel.y > 0) || 
		(vel_.x == 0 && oldVel.x != 0) || // or if the player stopped suddenly
		(vel_.y == 0 && oldVel.y != 0) || 
		(oldVel.x == 0 && vel_.x != 0) || // or if the player started suddenly
		(oldVel.y == 0 && vel_.y != 0) )
	{
		send_move_ = true;
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

bool Player::MovedSignificantly( Uint32 globalTime )
{
	static Vec2 lastPos = pos;

	if( send_move_ || globalTime - time_move_sent_ > 200 )
	{
		send_move_ = false;
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

BasePacket* Player::GetMovePacket( Uint32 globalTime )
{
	time_move_sent_ = globalTime;

	MovePacket* mp = new MovePacket();
	mp->SetID( ID );
	mp->SetTime( globalTime );
	mp->SetPosition( pos );
	mp->SetVelocity( vel_ );
	//TODO: mp->SetAngle(

	return (BasePacket*)mp;
}
