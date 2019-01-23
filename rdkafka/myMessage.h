#pragma once
#include "./librdkafka-PrintPoint/src-cpp/rdkafkacpp.h"
#include "json/json.h"
#include "utils.h"

class MyMessage {
    public:
        static Json::Reader reader;
        static Json::Value value;

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