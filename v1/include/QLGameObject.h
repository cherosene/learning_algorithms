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
    typedef std::pair<T,U> qlTableEntryType;
    typedef std::map<qlTableEntryType,float> qlTableType;
    
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
    std::function<std::vector<T>(U)>* validationFun;
    
public:
    // to instantiate a QLGameObject it requires a map [action->function] (see GameObject.h) and a set of states to define the qlTable
    QLGameObject(std::map<T,std::function<void()>> atm, int nx = 0, int ny = 0, float nspeed = 0.);
    // eventually a function which specify the valid actions for a certain state can be passed as a parameter
    QLGameObject(std::map<T,std::function<void()>> atm, std::function<std::vector<T>(U)> validActFun, int nx = 0, int ny = 0, float nspeed = 0.);
    
    // call the function associated with action act
    void doAction(T act);
    void doAction();
    
    // initialize the qlTable with random values
    void rinitQlTable(std::vector<U> nstates, float range = 1.);
    // add a state (with random value in the qlTable)
    void addState(U state, float range = 1.);
    // set the parameters for the qlearning algorithm (https://en.wikipedia.org/wiki/Q-learning)
    void setQlParameters(float lr, float df);
    // delete content of qlTable
    void clearQLTable();
    
    // override last state registered (used for the ql algorithm)
    void overrideLastState(U nstate);
    
    // choose the next action
    T chooseAction();
    // choose the next action between the permitted ones
    T chooseAction(std::vector<T> permittedActions);
    // get a vector with the actions currently available to perform a move
    std::vector<T> validActions();
    std::vector<T> validActions(U state);
    // evaluate the current value of a state (used for comparison between states)
    float stateValue(U state);
    // update qlTable and set new state
    void qlUpdate(U ns, float reward = 0.);
    
    // save the current QLTable; the set of states and actions must be the same when loading the data
    //void saveQLTable(const char* filename);
    // load a previously saved QLTable;
    //void loadQLTable(std::ifstream& inputFile);
    //void loadQLTable(const char* filename);
};


#include "QLGameObject.tpp"

#endif // QLGAMEOBJECT_H
