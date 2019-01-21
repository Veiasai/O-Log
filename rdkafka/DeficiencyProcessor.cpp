#include "DeficiencyProcessor.h"
#include <map>

using namespace std;
#define INTERVAL 500000000;

void DeficiencyProcessor::exec(const string &target)
{
    // target time and expected time
    int64_t t, et;
    if (reader.parse(target, value))
    {
        res.code = Status::OK;
        try
        {
            string msg_type = value["MessageType"].asString();
            switch (msg_type[6])
            {
            // price and statistic
            case 'P':
                break;
            case 'S':
            {
                StatisticsFeed *sf = new StatisticsFeed();
                sf->build(value["detail"].asString());

                // avoid the case: sf has been deleted
                string feedcode = sf->FEEDCODE;

                if (sf->TURNOVER_VALUE <= 0 || sf->TURNOVER_VOLUME <= 0)
                {
                    res.code = Status::WARN;
                    char r[200];
                    sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"TURNOVER_VOLUME\":%d, \"TURNOVER_VALUE\":%lf, \"LOG\":\"%s\"}", feedcode.c_str(), sf->TIMESTAMP, sf->TURNOVER_VOLUME, sf->TURNOVER_VALUE, "Invalid turnover value or turnover volume");
                    res.json.push_back(string(r));
                }

                if (msg_q[feedcode] == nullptr)
                {
                    cout << "new pq:" << feedcode << " test:" << sf->OPEN_INTEREST << " time:" << sf->EXCHANGE_TIMESTAMP << endl;
                    msg_q[feedcode] = new pq;
                    msg_t[feedcode] = sf->EXCHANGE_TIMESTAMP;
                    char r[200];
                    bool isValid = true;
                    msg_turnover_value[feedcode] = sf->TURNOVER_VALUE > 0 ? sf->TURNOVER_VALUE : 0;
                    msg_turnover_volume[feedcode] = sf->TURNOVER_VOLUME > 0 ? sf->TURNOVER_VOLUME : 0;
                    cout << "1" << endl;
                }

                // skip head
                et = msg_t[feedcode];
                if (et == sf->EXCHANGE_TIMESTAMP)
                {
                    msg_t[feedcode] += INTERVAL;
                    delete sf;
                    break;
                }
                else if (et > sf->EXCHANGE_TIMESTAMP)
                {
                    char r[200];
                    sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\": %lld, \"LOG\":\"delay %fs\"}", feedcode.c_str(), sf->EXCHANGE_TIMESTAMP, (et - sf->EXCHANGE_TIMESTAMP) / 1000000000.0);
                    res.json.push_back(string(r));
                    break;
                }
                cout << "2" << endl;

                // insert
                pq *sf_pq = msg_q[feedcode];
                sf_pq->push(sf);
                while (sf_pq->size() > 0)
                {
                    cout << "3" << endl;
                    t = sf_pq->top()->EXCHANGE_TIMESTAMP;
                    if (t == et)
                    {
                        // detect continuity                      
                        if (sf->TURNOVER_VALUE < msg_turnover_value[feedcode] || sf->TURNOVER_VOLUME < msg_turnover_volume[feedcode])
                        {
                            char r[200];
                            sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"TURNOVER_VOLUME\":%d, \"TURNOVER_VALUE\":%lf, \"LOG\":\"%s\"}", feedcode.c_str(), sf->TIMESTAMP, sf->TURNOVER_VOLUME, sf->TURNOVER_VALUE, "Turnover value or turnover volume should be monotonically increase");
                            res.json.push_back(string(r));
                        }
                        else
                        {
                            msg_turnover_value[feedcode] = sf->TURNOVER_VALUE;
                            msg_turnover_volume[feedcode] = sf->TURNOVER_VOLUME;
                        }
                        delete sf_pq->top();
                        sf_pq->pop();
                        et += INTERVAL;
                    }
                    else if (t < et)
                    {
                        delete sf_pq->top();
                        sf_pq->pop();
                    }
                    else
                    {
                        break;
                    }
                }
                cout << "4" << endl;

                msg_t[feedcode] = et;
                if (sf_pq->size() > 120)
                {
                    // miss
                    res.code = Status::WARN;
                    char r[200];
                    sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"LOG\":\"%s\"}", feedcode.c_str(), msg_t[feedcode], "miss");
                    res.json.push_back(string(r));

                    // ignore and continue
                    msg_t[feedcode] += INTERVAL;
                }
                else
                {
                    // wating
                }
            }
            break;
            default:
                break;
            }
        }
        catch (int &a)
        {
            //cout << "exec error" << endl;
        }
    }
    else
    {
        //cout << "json parse error" << endl;
    }
}

Pro_res DeficiencyProcessor::getResult()
{
    return this->res;
}

void DeficiencyProcessor::tag(string t)
{
    this->tags = t;
}

string DeficiencyProcessor::tag()
{
    return this->tags;
}

bool DeficiencyProcessor::checkTime(int64_t t, int64_t expect_t)
{
    if (t == expect_t)
    {
        return true;
    }
    else
    {
        return false;
    }
}
