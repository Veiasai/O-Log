#include "ValidityProcessor.h"

using namespace std;

void ValidityProcessor::exec(MyMessage* message){
    // cout << "in ValidityProcessor\n";
    switch (message->type){
        case Type::feedtype::Price:
        {
            // be careful to void *
            PriceFeed *pf = (PriceFeed*) message->feed();
            
            int64_t exchange_timestamp = pf->EXCHANGE_TIMESTAMP;
            string feedcode = pf->FEEDCODE;
            string log = "";
            if ((pf->BID_PRICE >= pf->ASK_PRICE) || pf->BID_PRICE <= 0 || pf->BID_PRICE >= 1e15 || pf->ASK_PRICE <= 0 || pf->ASK_PRICE >= 1e15)
            {
                log += "Invalid price ";
            }
            if (pf->BID_VOLUME < 0 || pf->BID_VOLUME > 1e15 || pf->ASK_VOLUME < 0 || pf->ASK_VOLUME > 1e15)
            {
                log += "Invalid volume";
            }
            if(log.size()!=0)
            {
                res.code = Status::WARN;
            }
            char r[200];
            sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"BID_PRICE\":%f, \"ASK_PRICE\":%f,\"BID_VOLUME\":%d, \"ASK_VOLUME\":%d, \"LOG\":\"%s\"}",
                    feedcode.c_str(), exchange_timestamp, pf->BID_PRICE,
                    pf->ASK_PRICE, pf->BID_VOLUME, pf->ASK_VOLUME, log.c_str());
            res.json.push_back(string(r));
            break;
        }
        case Type::feedtype::Statistics:
            break;
        default:
            break;
        }
    }
}

Pro_res ValidityProcessor::getResult(){
    return this->res;
}

void ValidityProcessor::tag(string t){
    this->tags = t;
}

string ValidityProcessor::tag(){
    return this->tags;
}