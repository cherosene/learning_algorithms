#ifndef QLGAMEOBJECT_H
#define QLGAMEOBJECT_H

#include "GameObject.h"

/*
    This class is a GameObject which implements the Qlearning algorithm (https://en.wikipedia.org/wiki/Q-learning).
*/

// T is the type of actions; U is the type of states
template <class T, class U>
class QLGameObject : GameObject<T>
{
public:
    typedef std::map<std::pair<T,U>,float> qlTableType;
    
    // the table for the Qlearning algorithm
    qlTableType qlTable;
private:
    bool isQlTableUpToDate = true;
    // parameters for the Qlearning algorithm
    float learningRate = 0.1;
    float discountFactor = 0;
    // last action performed/state
    T lastAction;
    U lastState;
    
public:
    // to instantiate a QLGameObject it requires a map [action->function] (see GameObject.h) and a set of states to define the qlTable
    QLGameObject(std::map<T,std::function<void()>> atm, std::vector<U> nstates, int nx = 0, int ny = 0, float nspeed = 0.);
    
    // call the function associated with action act
    void doAction(T act);
    void doAction();
    
    // initialize the qlTable with random values
    void rinitQlTable(float range = 1.);
    // set the parameters for the qlearning algorithm (https://en.wikipedia.org/wiki/Q-learning)
    void setQlParameters(float lr, float df);
    
    // choose the next action
    T chooseAction();
    // choose the next action between the permitted ones
    T chooseAction(std::vector<T> permittedActions);
    // update qlTable and set new state
    void qlUpdate(U ns, float reward = 0.);
    
    // save the current QLTable; the set of states and actions must be the same when loading the data
    void saveQLTable(const char* filename);
    // load a previously saved QLTable;
    void loadQLTable(const char* filename);
};


#include "QLGameObject.tpp"

#endif // QLGAMEOBJECT_H
