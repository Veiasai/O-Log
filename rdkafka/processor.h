#pragma once
#include <iostream>
#include <string>
#include "utils.h"

using namespace std;

using Pro_res = struct Process_result_;
struct Process_result_ {
    string json;
    Status::code code;
};

class Processor {
    public:
        virtual void exec(const string & target) =0;
        virtual Pro_res getResult() =0;
        virtual void tag(string tag) =0;
        virtual string tag() =0;
};

