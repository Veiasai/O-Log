#include "myProducer.h"

class MyConsumer
{
    private:
        RdKafka::KafkaConsumer *consumer;
        MyProducer *producer;
        void msg_consume(RdKafka::Message* message, void* opaque);
        
    public:
	MyConsumer();
        MyConsumer(std::string ConfPath);
        ~MyConsumer();
        void subscribe();
        void consume();
        void setProducer(MyProducer *myProducer);
};
