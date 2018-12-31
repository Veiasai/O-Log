#include "demo_processor.h"
#include <map>
#include "json/json.h"

using namespace std;
#define INTERVAL 500000000;

void  Demo_processor::exec(const string & target){
    // target time and expected time
    int64_t t, et;
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

                    // avoid the case: sf has been deleted
                    string feedcode = sf->FEEDCODE;

                    if (msg_q[feedcode] == nullptr) {
                        cout << "new pq:" <<  feedcode << " test:" << sf->OPEN_INTEREST << " time:" << sf->EXCHANGE_TIMESTAMP << endl;
                        msg_q[feedcode] = new pq;
                        msg_t[feedcode] = sf->EXCHANGE_TIMESTAMP;
                    }

                    // skip head
                    et = msg_t[feedcode];
                    if (et == sf->EXCHANGE_TIMESTAMP){
                        msg_t[feedcode] += INTERVAL;
                        delete sf;
                        break;
                    }

                    // insert
                    pq * sf_pq = msg_q[feedcode];
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

                    msg_t[feedcode] = et;
                    if (sf_pq->size() > 120){
                        // miss
                        res.code = Status::WARN;
                        char r[200];
                        pq *temp_pq = new pq;
                        while(sf_pq->size() > 0)
                        {
                            cout<<to_string(sf_pq->top()->EXCHANGE_TIMESTAMP)<<endl;
                            temp_pq->push(sf_pq->top());
                            sf_pq->pop();
                        }
                        cout<<endl;
                        while(temp_pq->size() > 0)
                        {
                            sf_pq->push(temp_pq->top());
                            temp_pq->pop();
                        }
                        delete temp_pq;
                        sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"LOG\":\"%s\"}", feedcode.c_str(), msg_t[feedcode], "miss");
                        res.json = string(r);

                        // ignore and continue
                        msg_t[feedcode] += INTERVAL;
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
