#include "ContextMap.h"

ContextMap::ContextMap()
{

}
ContextMap::~ContextMap()
{
    for(auto context:myMap)
    {
        delete context.second;
    }
}
bool ContextMap::create(string feedcode)
{
    mutex_.lock();
    // if(myMap.count(feedcode) != 0)
    // {
    //     mutex_.unlock();
    //     return false;
    // }
    myMap[feedcode] = new Context();
    mutex_.unlock();
    return true;
}
bool ContextMap::has(string feedcode)
{
    mutex_.lock_shared();
    if(myMap.count(feedcode) != 0)
    {
        mutex_.unlock_shared();
        return true;
    }
    else
    {
        mutex_.unlock_shared();
        return false;
    }
}
Context *ContextMap::getContext(string feedcode)
{
    mutex_.lock_shared();
    Context *tempContext = myMap[feedcode];
    mutex_.unlock_shared();
    return tempContext;
}