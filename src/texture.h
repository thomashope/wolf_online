#ifndef TEXTURE_H
#define TEXTURE_h

#include <SDL.h>
#include <string>
#include <iostream>

class Texture {
public:
	Texture(SDL_Renderer* renderTarget, std::string filePath, SDL_BlendMode blendMode = SDL_BLENDMODE_NONE);
	~Texture();

	void RenderStrip(int destX, int startY, int endY, float texCoordX);

	SDL_Texture* GetTexture() { return texture_; }

private:
	SDL_Texture* texture_ = nullptr;
	SDL_Renderer* renderTarget_;
	int width_;
	int height_;
};

#endif