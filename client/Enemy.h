#ifndef ENEMY_H
#define ENEMY_H

#include <memory>

#include "../shared/vec2.h"
#include "../shared/MovePacket.h"
#include "texture.h"
#include "screen.h"

class Player;

class Enemy {
public:
	Enemy( Vec2 pos );
	Enemy( const Enemy& ) = delete; // Don't allow the copy constructor
	virtual ~Enemy() { if( texture_ ) delete texture_; }

	void SetTexture(SDL_Renderer* ren, std::string filePath, SDL_BlendMode blendmode = SDL_BLENDMODE_BLEND);
	
	void Update( float dt );

	void Render( const Player& player, DepthBuffer* zBuffer );

	// distance betwee the given point and the sprite
	float Distance(Vec2 point) const;

	void StoreMovePacket( std::unique_ptr<MovePacket> packet );

	/* SETTERS */
	
	void SetPosition( Vec2 p ) { pos_ = p; }		// set the position
	void SetVelocity( Vec2 v ) { vel_ = v; }		// Set the velocity
	void SetScale( Vec2 s );				// amount to scale the sprite horizontally and vertically, must be +ve
	void SetOffset( int offset );			// TODO: make this accept a more sensible type, eg float 0.0 on ground 1.0 on ceiling

	/* GETTERS */

	int Offset() const { return vOffset_; }
	Vec2 Scale() const { return scale_; }

private:
	//TODO: pass in a whole camera class
	void SetTransform( const Player& player );

	Vec2 pos_;			// position
	Vec2 vel_;			// velocity

	std::unique_ptr<MovePacket> newest_move_;
	std::unique_ptr<MovePacket> previous_move_;

	Vec2 transform_;	// move the sprite to camera space
	Vec2 scale_;		// x and y scaling of the sprite
	int vOffset_;		// verticle offset for the sprite
	Texture* texture_;	// what texture the sprite should be, 0xff00ff is transparent by default
};

#endif