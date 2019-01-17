#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "json/json.h"

using namespace std;

using Pro_res = struct Process_result_;
struct Process_result_ {
    vector<string> json;
    Status::code code;
};

enum ProcessorType
{
    ValidityProcessor,
    DeficiencyProcessor
};

class Processor {
    public:
        virtual void exec(const string & target) =0;
        virtual Pro_res getResult() =0;
        virtual void tag(string tag) =0;
        virtual string tag() =0;
};

