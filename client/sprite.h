#ifndef SPRITE_H
#define SPRITE_H

#include "../shared/vec2.h"
#include "texture.h"
#include "screen.h"

class Sprite {
public:
	Sprite(float x, float y);
	virtual ~Sprite() { if (texture_) delete texture_; }

	void SetTexture(SDL_Renderer* ren, std::string filePath, SDL_BlendMode blendmode = SDL_BLENDMODE_NONE);
	
	void Render( Vec2 cameraPos, Vec2 cameraDir, Vec2 cameraPlane, DepthBuffer* zBuffer );

	// distance betwee the given point and the sprite
	float Distance(Vec2 point) const;

	/* SETTERS */
	
	inline void SetPos(float x, float y) { pos_.set(x, y); }
	inline void SetPos(Vec2 v) { pos_.set(v); }
	// Amount to scale the sprite horizontaly and vertically, must be +ve
	void SetScale(float x, float y);
	// TODO: make this accept a more sensible type, eg float 0.0 on ground 1.0 on ceiling
	void SetOffset(int offset);

	/* GETTERS */

	Texture* TexturePtr() const { return texture_; }
	Vec2 Transform() const { return transform_; }	
	int Offset() const { return vOffset_; }
	Vec2 Scale() const { return scale_; }

private:
	//TODO: pass in a whole camera class
	void SetTransform(Vec2 cameraPos, Vec2 cameraDir, Vec2 camerPlane);

	// position in the world
	Vec2 pos_;

	// moving the sprite to camera space
	Vec2 transform_;

	Vec2 scale_;
	int vOffset_;
	Texture* texture_;
};

#endif