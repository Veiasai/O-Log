#include "demo_processor.h"
#include <map>
#include "json/json.h"

using namespace std;
#define INTERVAL 500000000;

void  Demo_processor::exec(const string & target){
    int64_t t, et;
    int64_t * et_p;
    if (reader.parse(target, value)){
        res.code = Status::OK;
        try{
        string msg_type = value["MessageType"].asString();
            switch (msg_type[6]){
                // price and statistic
                case 'P': break;
                case 'S': {
                    StatisticsFeed* sf = new StatisticsFeed();
                    sf->build(value["detail"].asString());
                    if (msg_q[sf->FEEDCODE] == nullptr) {
                        cout << "new pq:" <<  sf->FEEDCODE << " test:" << sf->OPEN_INTEREST << " time:" << sf->EXCHANGE_TIMESTAMP << endl;
                        msg_q[sf->FEEDCODE] = new pq;
                        msg_t[sf->FEEDCODE] = sf->EXCHANGE_TIMESTAMP;
                    }

                    // skip head
                    et = msg_t[sf->FEEDCODE];
                    et_p = & msg_t[sf->FEEDCODE];
                    if (et == sf->EXCHANGE_TIMESTAMP){
                        msg_t[sf->FEEDCODE] += INTERVAL;
                        delete sf;
                        break;
                    }

                    // insert
                    pq * sf_pq = msg_q[sf->FEEDCODE];
                    sf_pq->push(sf);
                    while (sf_pq->size() > 0){
                        t = sf_pq->top()->EXCHANGE_TIMESTAMP;
                        if (t == et){
                            delete sf_pq->top();
                            sf_pq->pop();
                            et += INTERVAL;
                        }else if (t < et){
                            delete sf_pq->top();
                            sf_pq->pop();
                        }else{
                            break;
                        }
                    }

                    // avoid the case: sf has been deleted
                    *et_p = et;
                    if (sf_pq->size() > 60){
                        // miss
                        res.code = Status::WARN;
                        char r[200];
                        sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"LOG\":\"%s\"}", sf->FEEDCODE.c_str(), msg_t[sf->FEEDCODE], "miss");
                        res.json = string(r);

                        // ignore and continue
                        msg_t[sf->FEEDCODE] += INTERVAL;
                    }else{
                        // wating
                    }
                }
                    break;
                default:
                    break;
            }
        }catch(int &a){
            //cout << "exec error" << endl;
        }
    }else{
        //cout << "json parse error" << endl;
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
    if (t == expect_t){
        return true;
    }else{
        return false;
    }
}
