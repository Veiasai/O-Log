#pragma once
#include "./librdkafka-PrintPoint/src-cpp/rdkafkacpp.h"
#include "json/json.h"
#include "utils.h"

class MyMessage {
    public:
        MyMessage(RdKafka::Message* message);
        ~MyMessage();
        RdKafka::Message* message();
        const void * const feed();
        Type::feedtype type();
        void live();
        void kill();
        bool free();
    private:
        RdKafka::Message* _message;
        int _live;
        void * _feed;
        Type::feedtype _type;
};