#pragma once
#include "myProducer.h"
#include "myConsumer.h"
#include "myMessage.h"
#include "processor.h"
#include "confLoader.h"
#include "DeficiencyProcessor.h"
#include "ValidityProcessor.h"

class EventLoop
{
    private:
      MyConsumer *myConsumer = NULL;
      MyProducer *myProducer = NULL;
      vector<Processor *> processors;
      // int32_t: partition , it's not neccessary
      // i think that grouping messages by parition could decrease live=0 in queue.
      map<int32_t, queue<MyMessage*>> offset;  
    public:
      EventLoop();
      ~EventLoop();
      void setMyConsumer(HandlerConf handlerConf);
      void setMyProducer(HandlerConf handlerConf);
      void addProcessor(ProcessorType processorType, ContextMap* contextMap = NULL);
      void run();
};