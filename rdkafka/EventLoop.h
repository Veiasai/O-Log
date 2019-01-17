#pragma once
#include "myProducer.h"
#include "myConsumer.h"
#include "processor.h"
#include "confLoader.h"
#include "DeficiencyProcessor.h"

class EventLoop
{
    private:
      MyConsumer *myConsumer = NULL;
      MyProducer *myProducer = NULL;
      vector<Processor *> processors;
    public:
      EventLoop();
      ~EventLoop();
      void setMyConsumer(HandlerConf handlerConf);
      void setMyProducer(HandlerConf handlerConf);
      void addProcessor(ProcessorType processorType);
      void run();
};