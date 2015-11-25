#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>
#include <string>
#include <iostream>

class Texture {
public:
	Texture(SDL_Renderer* renderTarget, std::string filePath, SDL_BlendMode blendMode = SDL_BLENDMODE_NONE);
	~Texture();

	void RenderRect(SDL_Rect* sample, SDL_Rect* dest);
	void RenderStrip(int destX, int startY, int endY, float texCoordX);

	SDL_Texture* GetTexture() { return texture_; }
	int Width() { return width_; }
	int Height() { return height_; }

private:
	SDL_Texture* texture_;
	SDL_Renderer* renderTarget_;
	int width_;
	int height_;
};

#endif