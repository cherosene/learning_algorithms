#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

std::pair<SDL_Window*, SDL_Renderer*> startSDL(const char* winName, int winX, int winY, int winW, int winH);
void stopSDL(SDL_Window *win, SDL_Renderer *ren);

void logSDLError(std::ostream &os, const std::string &msg);
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int width, int height);