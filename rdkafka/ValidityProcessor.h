#include "processor.h"

using namespace std;

class ValidityProcessor : public Processor{
    private:
        Json::Reader reader;
        Json::Value value;
        string tags;
        Pro_res res;
    public:
        virtual void exec(const string & target);
        virtual Pro_res getResult();
        virtual void tag(string t);
        virtual string tag();
}