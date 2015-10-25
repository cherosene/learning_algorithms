#ifndef SDLUTILITY_H
#define SDLUTILITY_H

#include <SDL.h>

//Screen attributes
const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;


SDL_Surface *loadImage( const char* filename );

void applySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination );

void clr(SDL_Renderer* destination, const SDL_Color& col);

void renderRect( int x, int y, int w, int h, const SDL_Color& col, SDL_Renderer* destination, bool filled = true );

void renderRect( SDL_Rect& rect, const SDL_Color& col, SDL_Renderer* destination, bool filled = true );



#endif // SDLUTILITY_H