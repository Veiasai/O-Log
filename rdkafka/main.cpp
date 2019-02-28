#include "EventLoop.h"
#include <thread>
#include "Backuper.h"

int main(){
    HandlerConfs handlerConfs= ConfLoader::load("conf/cppStream.xml");
    if (handlerConfs.first.size()==0 || handlerConfs.second.size()==0)
    {
        std::cerr << "Load Conf Error." << std::endl;
        return 1;
    }

    ContextMap *contextMap = new ContextMap();
    EventLoop eventLoop;
    eventLoop.setMyConsumer(handlerConfs.first[0]);
    eventLoop.setMyProducer(handlerConfs.second[0]);
    eventLoop.addProcessor(Deficiency, contextMap);
    eventLoop.addProcessor(Validity);
    Backuper backuper;
    backuper.addContextMap(contextMap);
    backuper.setProducer(handlerConfs.second[1]);

    std::cout << "Begin eventLoop" << std::endl;
    thread myThread(&EventLoop::run, &eventLoop);
    thread backupThread(&Backuper::run, &backuper);
    myThread.join();
    backupThread.join();
    std::cout << "End eventLoop" << std::endl;

    delete contextMap;
    return 0;
}

