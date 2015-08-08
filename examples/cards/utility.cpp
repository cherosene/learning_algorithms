#include "utility.h"


std::pair<SDL_Window*, SDL_Renderer*> startSDL(const char* winName, int winX, int winY, int winW, int winH) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
    	logSDLError(std::cout, "SDL_Init");
    }

    SDL_Window *window = SDL_CreateWindow(winName, winX, winY, winW, winH, SDL_WINDOW_SHOWN);
    if (window == nullptr){
    	logSDLError(std::cout, "CreateWindow");
    	SDL_Quit();
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
    	SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr){
    	logSDLError(std::cout, "CreateRenderer");
        if (window) {SDL_DestroyWindow(window);}
    	SDL_Quit();
    }
    
    return std::pair<SDL_Window*,SDL_Renderer*>(window,renderer);
}

void stopSDL(SDL_Window *win, SDL_Renderer *ren) {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}


void logSDLError(std::ostream &os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
	//Initialize to nullptr to avoid dangling pointer issues
	SDL_Texture *texture = nullptr;
	//Load the image
	SDL_Surface *loadedImage = IMG_Load(file.c_str());
	//If the loading went ok, convert to texture and return the texture
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Make sure converting went ok too
		if (texture == nullptr){
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP");
	}
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int width, int height){
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
    dst.w = width;
    dst.h = height;
	//Query the texture to get its width and height to use
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
	int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}