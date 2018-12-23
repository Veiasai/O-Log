#include "demo_processor.h"
#include <map>
#include "json/json.h"

using namespace std;

void  Demo_processor::exec(const string & target){
    if (reader.parse(target, value)){
        res.code = Status::OK;

        string msg_type = value["MessageType"].asString();
        switch (msg_type[6]){
            // price and statistic
            case 'P': break;
            case 'S': 
                StatisticsFeed* sf = new StatisticsFeed();
                sf->build(value["detail"].asString());
                if (msg_q[sf->FEEDCODE] == nullptr) {
                    msg_q[sf->FEEDCODE] = new pq;
                    msg_t[sf->FEEDCODE] = sf->TIMESTAMP;
                }
                pq * sf_pq = msg_q[sf->FEEDCODE];
                sf_pq->push(sf);
                int64_t ct = msg_t[sf->FEEDCODE];
                while (checkTime(sf_pq->top()->TIMESTAMP, ct)){
                    sf_pq->pop();
                    ct += 500;
                }
                msg_t[sf->FEEDCODE] = ct;
                if (sf_pq->size() > 120){
                    // miss
                    res.code = Status::WARN;
                    char r[200];
                    sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"LOG\":\"%s\"}", sf->FEEDCODE.c_str(), msg_t[sf->FEEDCODE], "miss");
                    res.json = string(r);

                    // ignore and continue
                    msg_t[sf->FEEDCODE] += 500;
                }else{
                    // wating
                }
                break;
            default:
                
        }
    }
}

Pro_res Demo_processor::getResult(){
    return this->res;
}

void Demo_processor::tag(string t){
    this->tags = t;
}

string Demo_processor::tag(){
    return this->tags;
}

bool Demo_processor::checkTime(int64_t t, int64_t expect_t){
    if (t - expect_t < 500){
        return true;
    }
}
