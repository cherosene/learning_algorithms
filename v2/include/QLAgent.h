#ifndef QLAGENT_H
#define QLAGENT_H

#include <cstdlib>
#include <vector>
#include <map>

template <class T, class U>
class QLAgent {
public:
    // FIXME: this can be done with a multi-map
    typedef std::pair<T,U> qltEntry;
    typedef std::map<qltEntry,float> qlt;
    
    
    // subclass to throw exceptions
    class exception : public std::exception {
    public:
        constexpr static const char* exceptionName = "QLAgent::exception";
    	enum errcode {
            VAF_IS_NULLPTR,
            NO_VALID_ACTIONS,
            QLTENTRY_NOT_FOUND,
            ACTION_NOT_CHOOSEN_SINCE_LAST_UPDATE
        };
    private:
    	enum errcode type;
        static const char *Reason[4];
    public:
    	exception( enum errcode x ) : type(x) {};
        const char *what() const throw() { return (std::string(exceptionName) + ": " + Reason[type]).c_str(); }
    };
    
    
    // QLearning algorithm parameters
    float casualActionProbability;
    float learningRate;
    float discountFactor;
    float generationValueRange;
    // the table where the values are stored
    qlt& qlTable;
    // the function which choose the valid actions associated to a given state
    std::function<std::vector<T>(U)> validActionsFun;
    
    T lastChoosenAction;
    U currentState;
    
private:
    bool hasActionBeenChoosenSinceLastUpdate;
    
    
    
public:
    QLAgent(std::function<std::vector<T>(U)> nvalidActFun, qlt& nqlTable);
    // set the state (for initialization)
    void setCurrentState(U nstate);
    // set the qlearning method parameters
    void setQLParameters(float cap, float lr, float df, float gvr);
    // choose the action based on the values in the qlTable
    T chooseAction();
    // return the value associated to a state
    float stateValue(U state);
    // update the qlTable
    void update(U nstate, float reward = 0.);

protected:
    // controls
    bool isStateInTable(U state);
    // value related methods
    std::vector<T> validActions(U state);
    float expectedValue(T action, U state);
    // table update methods
    void addState(U nstate);
    void updateTableEntry(T action, U state, float nvalue);
    // utility methods
    float generateRandomValue();
    float generateRandomValue(float range);

};



#include "QLAgent.tpp"

#endif // QLAGENT_H