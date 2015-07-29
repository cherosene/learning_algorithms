#include <cstdlib>

#include "QLGameObject.h"

template <class T, class U>
QLGameObject<T,U>::QLGameObject(std::map<T,std::function<void()>> atm, std::vector<U> nstates, int nx, int ny, float nspeed) : GameObject<T>(atm, nx, ny, nspeed)
{
    for(typename std::map<T,std::function<void()>>::iterator amit = this->actionToMove.beign(); amit != this->actionToMove.end(); amit++ )
    {
        for(typename std::vector<U>::iterator sit = nstates.begin(); sit != nstates.end(); sit++ )
        {
            qlTable.insert( std::pair<std::pair<T,U>,float>( std::pair<T,U>(amit->first,*sit), 0. ) );
        }
    }
}

template <class T, class U>
void QLGameObject<T,U>::rinitQlTable(float range)
{
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