#include "sdlUtility.h"
#include <SDL_image.h>

// TODO: this load only BMP images
SDL_Surface *loadImage( const char* filename, SDL_Surface* screen ) {
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP( filename );
	if( loadedSurface == NULL ) { printf( "Unable to load image %s! SDL Error: %s\n", filename, SDL_GetError() ); }
    else {
		optimizedSurface = SDL_ConvertSurface( loadedSurface, screen->format, 0 );
		if( optimizedSurface == NULL ) { printf( "Unable to optimize image %s! SDL Error: %s\n", filename, SDL_GetError() ); }
        SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
}
    
void applySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination ) {
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, NULL, destination, &offset );
}

void clr(SDL_Renderer* destination, const SDL_Color& col) {
    SDL_SetRenderDrawColor(destination, col.r, col.g, col.b, col.a);
    SDL_RenderClear(destination);
}

void renderRect( int x, int y, int w, int h, const SDL_Color& col, SDL_Renderer* destination, bool filled) {
    SDL_SetRenderDrawColor(destination, col.r, col.g, col.b, col.a);
    SDL_Rect rect; rect.x = x; rect.y = y; rect.w = w; rect.h = h;
    if(filled) {    SDL_RenderFillRect(destination, &rect); }
    else {          SDL_RenderDrawRect(destination, &rect); }
}

void renderRect( SDL_Rect& rect, const SDL_Color& col, SDL_Renderer* destination, bool filled) {
    SDL_SetRenderDrawColor(destination, col.r, col.g, col.b, col.a);
    if(filled) {    SDL_RenderFillRect(destination, &rect); }
    else {          SDL_RenderDrawRect(destination, &rect); }
}