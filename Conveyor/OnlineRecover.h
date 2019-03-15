#pragma once
#include "Recover.h"

class OnlineRecover : public Recover
{
    public:
      virtual uint64_t getOffset();
      OnlineRecover(const string &_brokers, const string &_topic);
      ~OnlineRecover();

    private:
      RdKafka::KafkaConsumer *consumer = NULL;
      string brokers = "";
      string topic = "";
};
