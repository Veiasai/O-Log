#include "EventLoop.h"
#include <thread>

bool run = true;

int main(){
    HandlerConfs handlerConfs= ConfLoader::load("rdkafka/conf/cppStream.xml");
    EventLoop eventLoop;
    eventLoop.setMyConsumer(handlerConfs.first[0]);
    eventLoop.setMyProducer(handlerConfs.second[0]);
    eventLoop.addProcessor(Deficiency);
    eventLoop.addProcessor(Validity);
    thread myThread(&EventLoop::run, &eventLoop);
    myThread.join();
    return 0;
}

