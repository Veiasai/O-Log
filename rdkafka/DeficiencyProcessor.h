#include <map>
#include <string>
#include <queue>
#include <vector>

#include "json/json.h"
#include "processor.h"

using namespace std;

class feedcmp
{
public:
  bool operator() (const StatisticsFeed*  lhs, const StatisticsFeed* rhs) const
  {
    return (lhs->EXCHANGE_TIMESTAMP > rhs->EXCHANGE_TIMESTAMP);
  }
};

using pq = priority_queue<StatisticsFeed*, std::vector<StatisticsFeed*>, feedcmp>;

class DeficiencyProcessor : public Processor{
    public:
        static int64_t localTime;
    private:
        Json::Reader reader;
        Json::Value value;
        string tags;
        map<string, pq*> msg_q;
        map<string, int64_t> msg_t;
        map<string, double> msg_turnover_value;
        map<string, int32_t> msg_turnover_volume;
        Pro_res res;
        bool checkTime(int64_t t, int64_t expect_t);
    public:
        virtual void exec(const string & target);
        virtual Pro_res getResult();
        virtual void tag(string t);
        virtual string tag();
};