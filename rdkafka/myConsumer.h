#pragma once
#include "myProducer.h"
#include "processor.h"
#include <sys/time.h>

class MyConsumer
{
    private:
        RdKafka::KafkaConsumer *consumer;
        MyProducer *producer;
        Processor * processor;
        void msg_consume(RdKafka::Message* message, void* opaque);
        
    public:
	MyConsumer();
        MyConsumer(std::string ConfPath);
        ~MyConsumer();
        void subscribe();
        void consume();
        void setProducer(MyProducer *myProducer);
        void setProcessor(Processor * processor);
};
