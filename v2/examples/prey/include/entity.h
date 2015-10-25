#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>

class Entity {

public:
    enum Type   { HUNTER, PREY };
    enum Action { IDLE, UP, DOWN, LEFT, RIGHT };

private:
    const float         speed;
    const unsigned int  minX;
    const unsigned int  maxX;
    const unsigned int  minY;
    const unsigned int  maxY;
    const unsigned int  startX;
    const unsigned int  startY;
    
public:
    const unsigned int  width;
    const unsigned int  height;
    int x;
    int y;
    const SDL_Color& color;
    


public:
    // FIXME: why SDL_Rect ?!?!?
    Entity(unsigned int width, unsigned int height, float speed, int startX, int startY, SDL_Rect& boundRect, const SDL_Color& color);
    
    void move(enum Action act);
    void move(enum Action act, float rate);
    void reset();
    
    friend bool checkContact(const Entity& e1, const Entity& e2);
    friend int dist(const Entity& e1, const Entity& e2);

};

#endif // ENTITY_H