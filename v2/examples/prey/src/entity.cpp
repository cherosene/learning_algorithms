#include "entity.h"
#include <algorithm>


Entity::Entity(unsigned int width, unsigned int height, float speed, int startX, int startY, SDL_Rect& boundRect, const SDL_Color& color) : speed(speed), minX(boundRect.x), maxX(minX + boundRect.w), minY(boundRect.y), maxY(minY + boundRect.h), startX(startX), startY(startY), width(width), height(height), x(startX), y(startY), color(color) {}


void Entity::move(enum Action act) {
    move(act, 1);
}

void Entity::move(enum Action act, float rate) {
    switch(act) {
        case UP:
            y = std::max( y - speed * rate, (float)minY + height/2 );
        break;
        case DOWN:
            y = std::min( y + speed * rate, (float)maxY - height/2 );
        break;
        case LEFT:
            x = std::max( x - speed * rate, (float)minX + width/2 );
        break;
        case RIGHT:
            x = std::min( x + speed * rate, (float)maxX - width/2 );
        break;
        default:
        break;
    }
}

void Entity::reset() { x = startX; y = startY; }

bool checkContact(const Entity& e1, const Entity& e2) {
    if( 2 * abs(e1.x - e2.x) <= (int)(e1.width + e2.width) && 2 * abs(e1.y - e2.y) <= (int)(e1.height + e2.height) ) { return true; }
    return false;
}

int dist(const Entity& e1, const Entity& e2) {
    if(checkContact(e1,e2)) { return 0; }
    int distanceX = abs(e1.x - e2.x) - ( (int)(e1.width + e2.width) )/2;
    int distanceY = abs(e1.y - e2.y) - ( (int)(e1.height + e2.height) )/2;
    // if(distanceX + distanceY == 0 ) {
//         printf("WRONG!!!!\n");
//     }
    return distanceX + distanceY;
}