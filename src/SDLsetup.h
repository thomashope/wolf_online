//
//  SDLsetup.h
//
//  Created by Thomas Joseph Hope on 09/09/2014.
//  with a LOT of help from the tutorials by Will Usher
//  http://www.willusher.io/pages/sdl2/

#ifndef SDLSETUP_H
#define SDLSETUP_H

#include <SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// log an sdl error to the output stream of our choice
void logSDLError(std::ostream &os, const std::string &msg);
// loads a BMP image into a texture on the rendering device
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);

// draw an SDL_texture to and SDL_renderer at position x,y
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
// draw the texture scaled to given width and height
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);

void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, SDL_Rect dst, SDL_Rect* clip = nullptr) {
    // draw the clipped texture to the destination rectangle
    SDL_RenderCopy(ren, tex, clip, &dst);
}
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect* clip = nullptr) {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    renderTexture(tex, ren, dst, clip);
}

std::string getProjectPath(const std::string &subDir = ""){
    //We need to choose the path separator properly based on which
    //platform we're running on, since Windows uses a different
    //separator than most systems
#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif
    //This will hold the base resource path: Lessons/res/
    //We give it static lifetime so that we'll only need to call
    //SDL_GetBasePath once to get the executable path
    static std::string baseRes;
    if (baseRes.empty()){
        //SDL_GetBasePath will return NULL if something went wrong in retrieving the path
        char *basePath = SDL_GetBasePath();
        if (basePath){
            baseRes = basePath;
            SDL_free(basePath);
        }
        else {
            std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
            return "";
        }
    }
    //If we want a specific subdirectory path in the resource directory
    //append it to the base path. This would be something like Lessons/res/Lesson0
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

#endif
