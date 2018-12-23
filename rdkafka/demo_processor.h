#include <map>
#include <string>
#include <queue>
#include <vector>

#include "json/json.h"
#include "processor.h"
#include "utils.h"

using namespace std;

class feedcmp
{
public:
  bool operator() (const StatisticsFeed* & lhs, const StatisticsFeed* &rhs) const
  {
    return (lhs->TIMESTAMP < rhs->TIMESTAMP);
  }
};

using pq = priority_queue<StatisticsFeed*, std::vector<StatisticsFeed*>, feedcmp>;

class Demo_processor : public Processor{
    static Json::Reader reader;
    static Json::Value value;
    static int64_t localTime;
    private:
        string tags;
        map<string, pq*> msg_q;
        map<string, int64_t> msg_t;
        Pro_res res;
        bool checkTime(int64_t t, int64_t expect_t);
    public:
        virtual void exec(const string & target);
        virtual Pro_res getResult();
        virtual void tag(string t);
        virtual string tag();
};