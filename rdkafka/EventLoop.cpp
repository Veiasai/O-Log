#include "EventLoop.h"

EventLoop::EventLoop()
{

}

EventLoop::~EventLoop()
{
    if(myConsumer != NULL)
    {
        delete myConsumer;
        myConsumer = NULL;
    }
    if(myProducer != NULL)
    {
        delete myProducer;
        myProducer = NULL;
    }
    if(!processors.empty())
    {
        for(auto& processor : processors)
        {
            delete processor;
        }
        processors.clear();
    }
}

void EventLoop::setMyConsumer(HandlerConf handlerConf)
{
    myConsumer = new MyConsumer(handlerConf);
    myConsumer->subscribe();
}
void EventLoop::setMyProducer(HandlerConf handlerConf)
{
    myProducer = new MyProducer(handlerConf);
}
void EventLoop::addProcessor(ProcessorType processorType)
{
    switch (processorType)
    {
        case ValidityProcessor:
            
            break;
        case DeficiencyProcessor:
            processors.push_back(new Demo_processor());
            break;
    
        default:
            break;
    }
}

void EventLoop::run()
{
    while(true)
    {
        std::cout << "in loop" << std::endl;
        RdKafka::Message *message = myConsumer->consume();
        std::cout << 1 << std::endl;
        std::string messageStr(static_cast<const char *>(message->payload()));
        std::cout << 2 << std::endl;
        for(auto& processor : processors)
        {
            // for now exec use string as input, you should change it to RdKafka::Message*, and don't forget to delete it.
            processor->exec(messageStr);
            std::cout << 3 << std::endl;
            Pro_res res = processor->getResult();
            std::cout << 4 << std::endl;
            if (res.code != Status::OK){
                myProducer->produce(res.json);
            }
            std::cout << 5 << std::endl;
        }
        myProducer->poll(0);
    }
}
