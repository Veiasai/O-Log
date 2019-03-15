#pragma once
#include "Recover.h"
// #include "json/json.h"
#include <stdlib.h>

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
      uint64_t getOffsetFromMessage(RdKafka::Message *message);
};
