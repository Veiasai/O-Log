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

void ContextMap::addEventLoop(EventLoop *eventLoop)
{
    mutex_.lock();
    myEventLoops.push_back(eventLoop);
    mutex_.unlock();
}

void ContextMap::backupMetaData(Backuper *backuper)
{
    mutex_.lock();
    string metaData = "";
    for(auto entry : myMap)
    {
        string feedcode = "feedcode:" + entry.first;
        string expectTimestamp = "expectTimestamp:" + to_string(entry.second->getExpectTimestamp());
        string preValue = "preValue:" + to_string(entry.second->getPreValue());
        string preVolume = "preVolume:" + to_string(entry.second->getPreVolume());
        metaData = metaData + feedcode + "," + expectTimestamp + "," + preValue + "," + preVolume + ";";
    }
    backuper->produce(metaData);
    mutex_.unlock();
}