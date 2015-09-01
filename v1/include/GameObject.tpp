template <class T>
GameObject<T>::GameObject(std::map<T,std::function<void()>> atm, int nx, int ny, float nspeed) : x(nx), y(ny), speed(nspeed), actionToMove(atm) {}

//// SPATIAL POSITION
template <class T>
void GameObject<T>::warp(int nx, int ny) { x = nx; y = ny; }

template <class T>
void GameObject<T>::move(direction dir, int distance)
{
    switch(dir)
    {
        case UP:
            y -= distance;
            break;
        case DOWN:
            y += distance;
            break;
        case LEFT:
            x -= distance;
            break;
        case RIGHT:
            x += distance;
            break;
    }
}

template <class T>
void GameObject<T>::move(direction dir) { move(dir, speed); }

//// ACTIONS
template <class T>
void GameObject<T>::doAction(T act)
{
    typename std::map<T,std::function<void()>>::iterator it = actionToMove.find(act);
    if( it == actionToMove.end() )
    {
        // FIXME: raise an exception when the move is not valid
    }
    
    else
    {
        if( it->second != NULL )
        {
            (it->second)();
        }
    }
}

template <class T>
void GameObject<T>::bindAction(T act, std::function<void()> fun)
{
    actionToMove[act] = fun;
}

template <class T>
void GameObject<T>::dropAction(T act)
{
    typename std::map<T,std::function<void()>>::iterator itToErase;
    itToErase = actionToMove.find(act);
    if(itToErase != actionToMove.end()) {actionToMove.erase(itToErase);}
}

template <class T>
std::vector<T> GameObject<T>::validActions()
{
    std::vector<T> result;
    for(typename std::map<T,std::function<void()>>::iterator it = actionToMove.begin(); it != actionToMove.end(); ++it) { result.push_back(it->first); }
    return result;
}