#pragma once
#include "processor.h"

using namespace std;

class ValidityProcessor : public Processor{
    private:
        string tags;
        Pro_res res;
    public:
        virtual void exec(MyMessage* message);
        virtual Pro_res getResult();
        virtual void tag(string t);
        virtual string tag();
};