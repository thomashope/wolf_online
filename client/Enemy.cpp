#include "Enemy.h"
#include "Defines.h"
#include "player.h"

Enemy::Enemy( Vec2 pos ) :
pos_(pos.x, pos.y),
vel_(0, 0),
scale_(1.0f),
texture_(nullptr),
vOffset_(0)
{
	newest_move_ = std::unique_ptr<MovePacket>( new MovePacket );
	newest_move_->SetPosition( Vec2(0, 0) );
	newest_move_->SetVelocity( Vec2(0, 0) );
}

void Enemy::SetTexture(SDL_Renderer* ren, std::string filePath, SDL_BlendMode blendmode)
{
	// delete the current texture if there is one
	if (texture_) delete texture_;
	texture_ = nullptr;

	// load the new texture
	texture_ = new Texture(ren, filePath, blendmode);
}

void Enemy::StoreMovePacket( std::unique_ptr<MovePacket> recvd_move, Uint32 globalTime )
{
	// if the packet is older than the current packet, throw it away
	if( recvd_move->GetTime() > newest_move_->GetTime() )
	{
		// else store packet is it is newer
		newest_move_ = std::move( recvd_move );

		// store the curent position and when that prediction was made
		old_predicted_pos_ = pos_;
		old_prediction_time_ = globalTime;
	}
}

void Enemy::Update( Uint32 globalTime )
{
	// move to the new position over a fixed time, milliseconds
	Uint32 interp_duration = 100;

	// seconds since the newest packet was 'accurate' according to sender
	float time_since_recv = ((globalTime - newest_move_->GetTime()) / 1000.0f);
	
	// expected positio nof the player with linear prediction
	Vec2 predicted_pos = newest_move_->GetPosition() + newest_move_->GetVelocity() * time_since_recv;

	// how are through interpolation should the player be
	float interp_time = ((globalTime - old_prediction_time_) / (float)interp_duration);

	// if the interpolation is not finished,
	if( interp_time < 1.0f )
	{
		// Lerp between the old position and the new prediction
		pos_ = old_predicted_pos_.lerpTo( predicted_pos, interp_time );
	}
	else
	// we are done interpolating, use the newest position only
	{
		pos_ = predicted_pos;
	}
}

void Enemy::Render( const Player& player, DepthBuffer* zBuffer)
{
	SetTransform( player );

	// spriteScreenX is the xCoord of the centre of the sprite
	int spriteScreenX = int((SCREEN_WIDTH / 2) * (1 + transform_.x / transform_.y));

	//calculate height of the sprite on screen
	int spriteHeight = abs(int(SCREEN_HEIGHT / (transform_.y))); //using "transformY" instead of the real distance prevents fisheye
	//sprite.SetOffset(0);
	spriteHeight = int(spriteHeight * scale_.y);
	//calculate lowest and highest pixel to fill in current stripe
	int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2 + vOffset_;
	//if (drawStartY < 0) drawStartY = 0;
	int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2 + vOffset_;
	//if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;

	//calculate width of the sprite
	int spriteWidth = int(abs(int(SCREEN_HEIGHT / (transform_.y))) * scale_.x);
	if( spriteWidth <= 0 ) spriteWidth = 1;
	int drawStartX = spriteScreenX - (spriteWidth / 2);
	if (drawStartX < 0) drawStartX = 0;
	int drawEndX = spriteScreenX + (spriteWidth / 2);
	if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

	//loop through every vertical stripe of the sprite on screen
	if (drawStartY >= 0)
	for (int stripe = drawStartX; stripe < drawEndX + 1; stripe++)
	{
		int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * 16 / spriteWidth) / 256;
		//the conditions in the if are:
		//1) it's in front of camera plane so you don't see things behind you
		//2) it's on the screen (left)
		//3) it's on the screen (right)
		//4) ZBuffer, with perpendicular distance
		if (transform_.y > 0 && stripe > 0 && stripe < SCREEN_WIDTH)
		{
			if (transform_.y < zBuffer[stripe].perpDist) {
				SDL_Rect samp{ texX, 0, 1, 16 };
				SDL_Rect dest{ stripe, drawStartY, 1, drawEndY - drawStartY };
				texture_->RenderRect(&samp, &dest);
			}
			else if (drawStartY < zBuffer[stripe].top) {
				int sampEnd = int(((zBuffer[stripe].top - drawStartY) / float(drawEndY - drawStartY)) * texture_->Width());
				SDL_Rect samp{ texX, 0, 1, sampEnd };
				SDL_Rect dest{ stripe, drawStartY, 1, zBuffer[stripe].top - drawStartY };
				texture_->RenderRect(&samp, &dest);
			}
		}
	}
}

void Enemy::SetTransform( const Player& player )
{
	//translate sprite position to relative to camera
	Vec2 transPos = pos_ - player.pos;

	//transform sprite with the inverse camera matrix
	// [ plane.x   dir.x ] -1                                      [ dir.y      -dir.x ]
	// [                 ] =  1 / (plane.x *dir.y-dir.x*plane.y) * [                   ]
	// [ plane.y   dir.y ]                                         [ -plane.y  plane.x ]

	float invDet = 1.0f / (player.plane.x * player.dir.y - player.dir.x * player.plane.y); //required for correct matrix multiplication

	transform_.x = invDet * (player.dir.y * transPos.x - player.dir.x * transPos.y);
	transform_.y = invDet * (-player.plane.y * transPos.x + player.plane.x * transPos.y); //this is actually the depth inside the screen, that what Z is in 3D
}

float Enemy::Distance( Vec2 point ) const
{
	return (pos_ - point).length();
}

void Enemy::SetScale( Vec2 s )
{
	// ensure scale is not negative
	if (s.x < 0) s.x = 0.0f;
	if (s.y < 0) s.y = 0.0f;

	scale_ = s;
}

void Enemy::SetOffset( int offset )
{
	vOffset_ = offset;
}
