
#include "Enemy.h"
#include "SDLsetup.h"
#include "player.h"

Enemy::Enemy( Vec2 pos ) :
pos_(pos.x, pos.y),
vel_(0, 0),
scale_(1.0f),
texture_(nullptr),
vOffset_(0)
{
}

void Enemy::SetTexture(SDL_Renderer* ren, std::string filePath, SDL_BlendMode blendmode)
{
	// delete the current texture if there is one
	if (texture_) delete texture_;
	texture_ = nullptr;
	
	// load the new texture
	texture_ = new Texture(ren, filePath, blendmode);
}

void Enemy::Update( float dt )
{
	
	pos_ += vel_ * dt;
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
				//TODO: make sprites scale past screen height as the player aproaches instead of just dissapearing
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

void Enemy::StoreMovePacket( std::unique_ptr<MovePacket> packet )
{
	// TODO: check the timestamp for when the move was sent
	newest_move_ = std::move( packet );

	SetPosition( newest_move_->GetPosition() );
	SetVelocity( newest_move_->GetVelocity() );
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