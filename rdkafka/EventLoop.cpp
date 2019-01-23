#include "EventLoop.h"
#include "myMessage.h"

bool runTag = true;

EventLoop::EventLoop()
{
    loopCount = 0;
}

EventLoop::~EventLoop()
{
    if (myConsumer != NULL)
    {
        delete myConsumer;
        myConsumer = NULL;
    }
    if (myProducer != NULL)
    {
        delete myProducer;
        myProducer = NULL;
    }
    if (!processors.empty())
    {
        for (auto &processor : processors)
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
void EventLoop::addProcessor(ProcessorType processorType, ContextMap* contextMap)
{
    switch (processorType)
    {
    case Validity:
        processors.push_back(new ValidityProcessor());
        break;
    case Deficiency:
        {
            auto tempProcessor = new DeficiencyProcessor();
            tempProcessor->setContextMap(contextMap);
            processors.push_back(tempProcessor);
            break;
        }
    default:
        break;
    }
}

void EventLoop::run()
{
    while(runTag)
    {
        RdKafka::Message *message = myConsumer->consume();
        if (message != NULL)
        {
            loopCount++;
            auto myMessage = new MyMessage(message);
            offset[message->partition].push(myMessage);
            // std::string messageStr(static_cast<const char *>(message->payload()));
            for (auto &processor : processors)
            {
                processor->exec(myMessage);
                Pro_res res = processor->getResult();
                if (res.code != Status::OK)
                {
                    for (string s : res.json){
                        myProducer->produce(s);
                    }
                }
            }
            if (loopCount > 1000){
                store_offset();
            }
        }
        myProducer->poll(0);
    }
}

void EventLoop::store_offset(){
    for (auto & q : offset){
        MyMessage* cur = NULL;
        MyMessage* next= NULL;
        while(!q.second.empty()){
            next = q.second.front();
            if (next->free()){
                if (cur != NULL)
                    delete cur;
                cur = next;
            }else{
                break;
            }
        }
        if (cur != NULL){
            myConsumer->commit(cur->message);
            delete cur;
        }
    }
}