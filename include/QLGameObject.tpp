#include <cstdlib>
#include <fstream>

template <class T, class U>
QLGameObject<T,U>::QLGameObject(std::map<T,std::function<void()>> atm, std::vector<U> nstates, int nx, int ny, float nspeed) : GameObject<T>(atm, nx, ny, nspeed) {
    validationFun = nullptr;
    for(typename std::map<T,std::function<void()>>::iterator amit = this->actionToMove.begin(); amit != this->actionToMove.end(); amit++ )
    {
        for(typename std::vector<U>::iterator sit = nstates.begin(); sit != nstates.end(); sit++ )
        {
            qlTable.insert( std::pair<std::pair<T,U>,float>( std::pair<T,U>(amit->first,*sit), 0. ) );
        }
    }
}

template <class T, class U>
QLGameObject<T,U>::QLGameObject(std::map<T,std::function<void()>> atm, std::vector<U> nstates, std::function<std::vector<T>(U)> validActFun, int nx, int ny, float nspeed) : GameObject<T>(atm, nx, ny, nspeed) {
    validationFun = validActFun;
    for(typename std::vector<U>::iterator sit = nstates.begin(); sit != nstates.end(); sit++ ) {
        std::vector<T> validActVec = validActFun(*sit);
        for(typename std::vector<T>::iterator ait = validActVec.begin(); ait != validActVec.end(); ait++) {
            qlTable.insert( std::pair<std::pair<T,U>,float>( std::pair<T,U>(*ait,*sit), 0. ) );
        }
    }
}


template<class T, class U>
void QLGameObject<T,U>::doAction(T act) {
    if(isQlTableUpToDate)
    {
        GameObject<T>::doAction(act);
        lastAction = act;
        isQlTableUpToDate = false;
    }
    else
    {
        // FIXME: throw exception
    }
}

template<class T, class U>
void QLGameObject<T,U>::doAction() {
    doAction(chooseAction());
}



template <class T, class U>
void QLGameObject<T,U>::rinitQlTable(float range) {
    float r;
    
    if(range<=0)
    {
        // FIXME: throw an exception
    }
    
    for( typename std::map<std::pair<T,U>,float>::iterator it = qlTable.begin(); it != qlTable.end(); it++ )
    {
        r = static_cast<float>(rand())/(static_cast<float>(RAND_MAX/range));
        it->second = r;
    }
}

template <class T, class U>
void QLGameObject<T,U>::setQlParameters(float lr, float df) { learningRate = lr; discountFactor = df; }


template <class T, class U>
T QLGameObject<T,U>::chooseAction() {
    std::vector<T> va;
    if(validationFun == nullptr) {
        va = this->validActions();
    }
    else {
        va = (*validationFun)(lastState);
    }
    
    return chooseAction(va);
}

template <class T, class U>
T QLGameObject<T,U>::chooseAction(std::vector<T> permittedActions) {
    T result = permittedActions[0];
    float ofv = qlTable[std::pair<T,U>(permittedActions[0],this->lastState)];
    float tmpValue;
    
    for(typename std::vector<T>::iterator it = permittedActions.begin(); it != permittedActions.end(); it++) {
        tmpValue = qlTable[std::pair<T,U>(*it,this->lastState)];
        if(tmpValue > ofv) {
            ofv = tmpValue;
            result = *it;
        }
    }
    return result;
}

template <class T, class U>
U QLGameObject<T,U>::stateValue(U state) {
    std::vector<T> va = this->validActions();
    
    float* m = nullptr;
    
    for(typename std::vector<T>::iterator it = va.begin(); it != va.end(); it++) {
        std::pair<T,U> curPair = std::pair<T,U>(*it,state);
        typename qlTableType::iterator curIt = qlTable.find(curPair);
        if(curIt != qlTable.end()) {
            if(m) {
                if(curIt->second > m) { m = curIt->second; }
            }
            else {
                m = new float;
                m = curIt->second;
            }
        }
    }
    
    if(!m) {
        // FIXME: throw exception
    }
    return m;
}

template <class T, class U>
void QLGameObject<T,U>::qlUpdate(U ns, float reward) {
    std::pair<T,U> lastAS = std::pair<T,U>(lastAction,lastState);
    
    std::vector<T> va = this->validActions();
    float ofv = qlTable[std::pair<T,U>(va[0],ns)];
    for(typename std::vector<T>::iterator it = va.begin(); it != va.end(); it++) { ofv = std::max(ofv,qlTable[std::pair<T,U>(*it,ns)]); }
    
    qlTable[lastAS] += learningRate * ( reward + discountFactor *  ofv - qlTable[lastAS] );
    lastState = ns;
    isQlTableUpToDate = true;
}


template <class T, class U>
void QLGameObject<T,U>::saveQLTable(const char* filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    
    for(typename qlTableType::iterator it = qlTable.begin(); it != qlTable.end(); it++) {
        outputFile << it->second << std::endl;
    }
    
    outputFile.close();
}

template <class T, class U>
void QLGameObject<T,U>::loadQLTable(std::ifstream inputFile) {
    for(typename qlTableType::iterator it = qlTable.begin(); it != qlTable.end(); it++) {
        inputFile >> it->second;
    }
}

template <class T, class U>
void QLGameObject<T,U>::loadQLTable(const char* filename) {
    std::ifstream inputFile;
    inputFile.open(filename);
    
    loadQLTable(inputFile);
    
    inputFile.close();
}