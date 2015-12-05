
#include "sprite.h"
#include "SDLsetup.h"

Sprite::Sprite(float x, float y) :
pos_(x, y),
scale_(1.0f),
texture_(nullptr),
vOffset_(0)
{
}

void Sprite::SetTexture(SDL_Renderer* ren, std::string filePath, SDL_BlendMode blendmode)
{
	// delete the current texture if there is one
	if (texture_) delete texture_;
	texture_ = nullptr;
	
	// load the new texture
	texture_ = new Texture(ren, filePath, blendmode);
}

void Sprite::Render(Vec2 cameraPos, Vec2 cameraDir, Vec2 cameraPlane, DepthBuffer* zBuffer)
{
	SetTransform(cameraPos, cameraDir, cameraPlane);

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

void Sprite::SetTransform(Vec2 cameraPos, Vec2 cameraDir, Vec2 camerPlane)
{
	//translate sprite position to relative to camera
	//float spriteX = sprite.x - cameraPos.x;
	//float spriteY = sprite.y - cameraPos.y;
	Vec2 transPos = pos_ - cameraPos;

	//transform sprite with the inverse camera matrix
	// [ plane.x   dir.x ] -1                                       [ dir.y      -dir.x ]
	// [               ]       =  1/(plane.x*dir.y-dir.x*plane.y) *   [                 ]
	// [ plane.y   dir.y ]                                          [ -plane.y  plane.x ]

	float invDet = 1.0f / (camerPlane.x * cameraDir.y - cameraDir.x * camerPlane.y); //required for correct matrix multiplication

	transform_.x = invDet * (cameraDir.y * transPos.x - cameraDir.x * transPos.y);
	transform_.y = invDet * (-camerPlane.y * transPos.x + camerPlane.x * transPos.y); //this is actually the depth inside the screen, that what Z is in 3D   
}

float Sprite::Distance(Vec2 point) const
{
	return (pos_ - point).length();
}


void Sprite::SetScale(float x, float y)
{
	// ensure scale is not negative
	if (x < 0) x = 0.0f;
	if (y < 0) y = 0.0f;

	scale_.set(x, y);
}

void Sprite::SetOffset(int offset)
{
	vOffset_ = offset;
}