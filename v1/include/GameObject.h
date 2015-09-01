#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <map>


// T is the type of actions
template <class T>
class GameObject
{
public:
    int x, y;
    float speed;
    enum direction { UP, DOWN, LEFT, RIGHT, DIRECTION_NUM };
    std::map<T,std::function<void()>> actionToMove;
    
public:
    // to instantiate a GameObject it requires a map [action -> function] to perform a move when the program calls an action
    GameObject(std::map<T,std::function<void()>> atm, int nx = 0, int ny = 0, float nspeed = 0.);
    
    //// SPATIAL POSITION
    // warp to a specific position
    void warp(int nx, int ny);
    // movements
    void move(direction dir, int distance);
    void move(direction dir);
    
    //// ACTIONS
    // call the function associated with action act
    void doAction(T act);
    // bind a function to an action
    void bindAction(T act, std::function<void()> fun);
    // drop a function binded to an action
    void dropAction(T act);
    // get a vector with the actions currently available to perform a move
    std::vector<T> validActions();
};


#include "GameObject.tpp"

#endif // GAMEOBJECT_H
