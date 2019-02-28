#include <map>
#include <unordered_map>
#include "json/json.h"
#include "processor.h"
#include "ContextMap.h"

using namespace std;
class ContextMap;

class DeficiencyProcessor : public Processor
{
  public:
    static int64_t localTime;

  private:
    string tags;
    // map<string, pq*> msg_q;
    // map<string, int64_t> msg_t;
    // map<string, double> msg_turnover_value;
    // map<string, int32_t> msg_turnover_volume;
    unordered_map<void *, void *> sf_to_m;
    ContextMap *contextMap;
    Pro_res res;
    bool checkTime(int64_t t, int64_t expect_t);

  public:
    virtual void exec(MyMessage *message);
    virtual Pro_res getResult();
    virtual void tag(string t);
    virtual string tag();
    void setContextMap(ContextMap *cm);
};