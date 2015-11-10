#include "texture.h"

Texture::Texture(SDL_Renderer* renderTarget, std::string filePath, SDL_BlendMode blendMode) :renderTarget_(renderTarget)
{
	// load the file into a surface
	SDL_Surface *bmp = SDL_LoadBMP(filePath.c_str());
	if (bmp == nullptr) {
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
	}
	else { std::cerr << "Found: " << filePath << std::endl; }

	// Set magenta (super pink) as the transparent colour if a blend mode was given
	if (blendMode != SDL_BLENDMODE_NONE) {
		int error = SDL_SetColorKey(bmp, SDL_TRUE, SDL_MapRGB(bmp->format, 255, 0, 255));
		if (error < 0) {
			std::cerr << "SDL_SetColorKey Error: " << SDL_GetError() << std::endl;
		}
	}

	// convert the surface to a texture
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderTarget_, bmp);
	if (tex == nullptr){
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
	}

	// Make the created texture actually use transparency if given
	if (blendMode != SDL_BLENDMODE_NONE) {
		int error = SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		if (error < 0) {
			std::cerr << "SDL_SetTextureBlendMode Error: " << SDL_GetError() << std::endl;
		}
	}

	// store the created texture
	texture_ = tex;

	// release the temporary surface
	SDL_FreeSurface(bmp);

	// Get the width and height of the texture
	if (texture_) {
		SDL_QueryTexture(texture_, nullptr, nullptr, &width_, &height_);
	}
}

Texture::~Texture()
{
	if (texture_)
	{
		SDL_DestroyTexture(texture_);
	}
}

void Texture::RenderStrip(int destX, int startY, int endY, float texCoordX)
{
	// convert the texCoord from [0,1] to [0,width_]
	SDL_Rect src{ int(texCoordX * float(width_)), 0, 1, height_ };

	// a 1 pixel wide strip where our texture will end up
	SDL_Rect dest{destX, startY, 1, endY - startY + 1};

	SDL_RenderCopy(renderTarget_, texture_, &src, &dest);
}