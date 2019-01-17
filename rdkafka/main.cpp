#include "EventLoop.h"
#include <thread>

bool run = true;

int main(){
    HandlerConfs handlerConfs= ConfLoader::load("conf/cppStream.xml");
    if (handlerConfs.first.size()==0 || handlerConfs.second.size()==0)
    {
        std::cerr << "Load Conf Error." << std::endl;
        return 1;
    }
    EventLoop eventLoop;
    eventLoop.setMyConsumer(handlerConfs.first[0]);
    eventLoop.setMyProducer(handlerConfs.second[0]);
    eventLoop.addProcessor(Deficiency);
    eventLoop.addProcessor(Validity);
    thread myThread(&EventLoop::run, &eventLoop);
    myThread.join();
    return 0;
}

