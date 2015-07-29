#ifndef QLGAMEOBJECT_H
#define QLGAMEOBJECT_H

#include "GameObject.h"

/*
    This class is a GameObject which implements the QLearning algorithm (https://en.wikipedia.org/wiki/Q-learning).
*/

// T is the type of actions; U is the type of states
template <class T, class U>
class QLGameObject : GameObject<T>
{
public:
    // the table for the QLearning algorithm
    std::map< std::pair<T,U>, float > qlTable;
    
public:
    // to instantiate a QLGameObject it requires a map [action->function] (see GameObject.h) and a set of states to define the qlTable
    QLGameObject(std::map<T,std::function<void()>> atm, std::vector<U> nstates, int nx = 0, int ny = 0, float nspeed = 0.);
    
    // initialize the qlTable with random values
    void rinitQlTable(float range = 1.);
};

#endif // QLGAMEOBJECT_H
