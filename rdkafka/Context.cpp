#include "Context.h"

Context::Context()
{

}
Context::~Context()
{
    delete queue;
}
void Context::lock()
{
    mutex_.lock();
}
void Context::unlock()
{
    mutex_.unlock();
}
pq *Context::getQueue()
{
    if (queue == NULL)
    {
        queue = new pq();
    }
    return queue;
}
int64_t Context::getExpectTimestamp()
{
    return expectTimestamp;
}
void Context::setExpectTimestamp(int64_t timestamp)
{
    expectTimestamp = timestamp;
}
double Context::getPreValue()
{
    return preValue;
}
void Context::setPreValue(double value)
{
    preValue = value;
}
int32_t Context::getPreVolume()
{
    return preVolume;
}
void Context::setPreVolume(int32_t volume){
    preVolume = volume;
}