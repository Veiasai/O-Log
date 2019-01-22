#include "EventLoop.h"

bool runTag = true;

EventLoop::EventLoop()
{
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
void EventLoop::addProcessor(ProcessorType processorType)
{
    switch (processorType)
    {
    case Validity:
        processors.push_back(new ValidityProcessor());
        break;
    case Deficiency:
        processors.push_back(new DeficiencyProcessor());
        break;
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
            cout << "message at offset:" << message->offset() << " in partition:" << message->partition() << endl;
            std::string messageStr(static_cast<const char *>(message->payload()));
            for (auto &processor : processors)
            {
                // for now exec use string as input, you should change it to RdKafka::Message*, and don't forget to delete it.
                processor->exec(messageStr);
                Pro_res res = processor->getResult();
                if (res.code != Status::OK)
                {
                    for (string s : res.json){
                        myProducer->produce(s);
                    }
                }
            }
            myConsumer->setOffset(message->offset(), message->partition());
        }
        myProducer->poll(0);
    }
}
