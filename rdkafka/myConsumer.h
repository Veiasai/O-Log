#pragma once
#include "myProducer.h"
#include "processor.h"
#include <sys/time.h>
#include <chrono>
#include "confLoader.h"

class MyConsumer
{
    private:
        RdKafka::KafkaConsumer *consumer;
        MyProducer *producer;
        Processor * processor;
        std::vector<std::string> topics;
        RdKafka::Message* msg_consume(RdKafka::Message* message, void* opaque);
        
    public:
	    MyConsumer();
        MyConsumer(HandlerConf handlerConf);
        ~MyConsumer();
        void subscribe();
        RdKafka::Message* consume();
        void setProducer(MyProducer *myProducer);
        void setProcessor(Processor * processor);
        void commit(RdKafka::Message*);
};
