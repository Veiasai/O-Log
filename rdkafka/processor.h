#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "./librdkafka-PrintPoint/src-cpp/rdkafkacpp.h"
#include "myMessage.h"

using namespace std;

using Pro_res = struct Process_result_;
struct Process_result_ {
    vector<string> json;
    Status::code code;
};

enum ProcessorType
{
    Validity,
    Deficiency
};

class Processor {
    public:
        virtual void exec(MyMessage* message) =0;
        virtual Pro_res getResult() =0;
        virtual void tag(string tag) =0;
        virtual string tag() =0;
};

