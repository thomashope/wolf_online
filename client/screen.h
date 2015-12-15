#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include <string>

struct DepthBuffer {
	int top;
	int bottom;
	double perpDist; //  perpendicular distance to wall
	double depth;
};

class Screen
{
public:
	Screen( int width, int height, std::string title );
	~Screen();

	void Display();

	int Width() const { return width_; }
	int Height() const { return height_; }

	SDL_Renderer* GetRenderer() const { return renderer_; }
	SDL_Window* GetWindow() const { return window_; }
	DepthBuffer* GetDepthBuffer() const { return depthBuffer_; }

private:
	int width_;
	int height_;

	SDL_Renderer* renderer_;
	SDL_Window* window_;
	DepthBuffer* depthBuffer_;
};

#endif