#include "DeficiencyProcessor.h"
#include <map>

using namespace std;
#define INTERVAL 500000000

void DeficiencyProcessor::exec(const RdKafka::Message* message){
    std::string messageStr(static_cast<const char *>(message->payload()));
    exec(messageStr);
}

void DeficiencyProcessor::exec(const string &target)
{
    cout << "in DeficiencyProcessor\n";
    // target time and expected time
    int64_t t, et;
    Context *context;
    if (reader.parse(target, value))
    {
        res.code = Status::OK;
        res.json.clear();
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

                if (!contextMap->has(feedcode))
                {
                    cout << "new context:" << feedcode << " time:" << sf->EXCHANGE_TIMESTAMP << endl;
                    contextMap->create(feedcode);
                    context = contextMap->getContext(feedcode);
                    context->lock();
                    context->setExpectTimestamp(sf->EXCHANGE_TIMESTAMP);
                    context->setPreValue(sf->TURNOVER_VALUE > 0 ? sf->TURNOVER_VALUE : 0);
                    context->setPreVolume(sf->TURNOVER_VOLUME > 0 ? sf->TURNOVER_VOLUME : 0);
                    context->unlock();
                }
                
                context = contextMap->getContext(feedcode);
                context->lock();
                // skip head
                et = context->getExpectTimestamp();
                if (et == sf->EXCHANGE_TIMESTAMP)
                {
                    context->setExpectTimestamp(et + INTERVAL);
                    delete sf;
                    context->unlock();
                    break;
                }
                else if (et > sf->EXCHANGE_TIMESTAMP)
                {
                    res.code = Status::WARN;
                    char r[200];
                    sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\": %lld, \"LOG\":\"delay %fs\"}", feedcode.c_str(), sf->EXCHANGE_TIMESTAMP, (et - sf->EXCHANGE_TIMESTAMP) / 1000000000.0);
                    res.json.push_back(string(r));
                    context->unlock();
                    break;
                }

                // insert
                pq *sf_pq = context->getQueue();
                sf_pq->push(sf);
                while (sf_pq->size() > 0)
                {
                    t = sf_pq->top()->EXCHANGE_TIMESTAMP;
                    if (t == et)
                    {
                        // detect continuity                      
                        if (sf->TURNOVER_VALUE < context->getPreValue() || sf->TURNOVER_VOLUME < context->getPreVolume())
                        {
                            char r[200];
                            sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"TURNOVER_VOLUME\":%d, \"TURNOVER_VALUE\":%lf, \"LOG\":\"%s\"}", feedcode.c_str(), sf->TIMESTAMP, sf->TURNOVER_VOLUME, sf->TURNOVER_VALUE, "Turnover value or turnover volume should be monotonically increase");
                            res.json.push_back(string(r));
                        }
                        else
                        {
                            context->setPreValue(sf->TURNOVER_VALUE);
                            context->setPreVolume(sf->TURNOVER_VOLUME);
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
                        context->unlock();
                        break;
                    }
                }

                context->setExpectTimestamp(et);
                if (sf_pq->size() > 120)
                {
                    // miss
                    res.code = Status::WARN;
                    char r[200];
                    sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"LOG\":\"%s\"}", feedcode.c_str(), context->getExpectTimestamp(), "miss");
                    res.json.push_back(string(r));

                    // ignore and continue
                    context->setExpectTimestamp(context->getExpectTimestamp()+INTERVAL);
                }
                else
                {
                    // wating
                }
                context->unlock();
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

void DeficiencyProcessor::setContextMap(ContextMap *cm)
{
    contextMap = cm;
}