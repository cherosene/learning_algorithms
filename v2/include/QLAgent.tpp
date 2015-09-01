// exception messagges
template <class T, class U>
const char *QLAgent<T,U>::exception::Reason[4] = {
    "validActionFun is not defined.",
    "No valid actions specified for the state given.",
    "Entry not found in the qlTable.",
    "No action choosen since last update. Are you forgetting something?"
};


// public methods
template <class T, class U>
QLAgent<T,U>::QLAgent(std::function<std::vector<T>(U)> nvalidActFun, qlt& nqlTable) : qlTable(nqlTable), validActionsFun(nvalidActFun) {
    hasActionBeenChoosenSinceLastUpdate = false;
};

template <class T, class U>
void QLAgent<T,U>::setCurrentState(U nstate) {
    if(!isStateInTable(nstate)) { addState(nstate); }
    currentState = nstate;
}

template <class T, class U>
void QLAgent<T,U>::setQLParameters(float cap, float lr, float df, float gvr) {
    // FIXME: put here controls for the values
    casualActionProbability = cap;
    learningRate = lr;
    discountFactor = df;
    generationValueRange = gvr;
}

template <class T, class U>
T QLAgent<T,U>::chooseAction() {
    std::vector<T> va = validActions(currentState);
    if(va.size() == 0) { throw exception(exception::NO_VALID_ACTIONS); }
    
    // choose to act casually or in a standard manner
    float p = generateRandomValue(1.);
    if(p < casualActionProbability) {
        int actionIndex = rand() % va.size();
        lastChoosenAction = va[actionIndex];
    }
    else {
        typename std::vector<T>::iterator itToBest = va.begin();
        float bestValue = expectedValue(*itToBest,currentState);
        for(typename std::vector<T>::iterator it = va.begin(); it != va.end(); it++) {
            float curValue = expectedValue(*it,currentState);
            if(curValue > bestValue) {
                bestValue = curValue;
                itToBest = it;
            }
        }
        lastChoosenAction = *itToBest;
    }
    
    hasActionBeenChoosenSinceLastUpdate = true;
    return lastChoosenAction;
}

template <class T, class U>
float QLAgent<T,U>::stateValue(U state) {
    std::vector<T> va = validActions(state);
    if(va.size() == 0) { throw exception(exception::NO_VALID_ACTIONS); }
    
    float bestValue = expectedValue(*(va.begin()),state);
    for(typename std::vector<T>::iterator it = va.begin(); it != va.end(); it++) {
        float curValue = expectedValue(*it,state);
        if(curValue > bestValue) { bestValue = curValue; }
    }
    
    return bestValue;
}

template <class T, class U>
void QLAgent<T,U>::update(U nstate, float reward) {
    if(!hasActionBeenChoosenSinceLastUpdate) { throw exception(exception::ACTION_NOT_CHOOSEN_SINCE_LAST_UPDATE); }
    // if not already in table, add the new state and the corresponding actions
    if(!isStateInTable(nstate)) { addState(nstate); }
    
    float oldValue = expectedValue(lastChoosenAction,currentState);
    float nvalue = oldValue + learningRate * (reward + discountFactor * stateValue(nstate) - oldValue);
    updateTableEntry(lastChoosenAction,currentState,nvalue);
    currentState = nstate;
    hasActionBeenChoosenSinceLastUpdate = false;
}

// protected and private methods
template <class T, class U>
bool QLAgent<T,U>::isStateInTable(U state) {
    std::vector<T> va = validActions(state);
    if(va.size() == 0) { return false; }
    
    return qlTable.find(qltEntry(*va.begin(),state)) != qlTable.end();
}

template <class T, class U>
std::vector<T> QLAgent<T,U>::validActions(U state) {
    if(validActionsFun != nullptr) { return validActionsFun(state); }
    else { throw exception(exception::VAF_IS_NULLPTR); }
}

template <class T, class U>
float QLAgent<T,U>::expectedValue(T action, U state) {
    typename qlt::iterator it = qlTable.find(qltEntry(action,state));
    if(it == qlTable.end()) { throw exception(exception::QLTENTRY_NOT_FOUND); }
    else { return it->second; }
}

template <class T, class U>
void QLAgent<T,U>::addState(U nstate) {
    std::vector<T> va = validActions(nstate);
    if(va.size() == 0) { throw exception(exception::NO_VALID_ACTIONS); }
    for(typename std::vector<T>::iterator it = va.begin(); it != va.end(); it++) {
        qlTable[qltEntry(*it,nstate)] = generateRandomValue();
    }
}

template <class T, class U>
void QLAgent<T,U>::updateTableEntry(T action, U state, float nvalue) {
    typename qlt::iterator it = qlTable.find(qltEntry(action,state));
    if(it == qlTable.end()) { throw exception(exception::QLTENTRY_NOT_FOUND); }
    else { it->second = nvalue; }
}

template <class T, class U>
float QLAgent<T,U>::generateRandomValue() {
    return generateRandomValue(generationValueRange);
}

template <class T, class U>
float QLAgent<T,U>::generateRandomValue(float range) {
    return static_cast<float>(rand())/(static_cast<float>(RAND_MAX/range));
}