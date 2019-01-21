#include "ValidityProcessor.h"

using namespace std;

void ValidityProcessor::exec(const string &target)
{
    cout << "in ValidityProcessor\n";
    if (reader.parse(target, value))
    {
        cout << "1\n";
        res.code = Status::OK;
        try
        {
            string msg_type = value["MessageType"].asString();
            cout << "2\n";
            switch (msg_type[6])
            {
            case 'P':
            {
                cout << "3\n";
                PriceFeed *pf = new PriceFeed();
                pf->build(value["detail"].asString());
                cout << "4\n";
                int64_t exchange_timestamp = pf->EXCHANGE_TIMESTAMP;
                string feedcode = pf->FEEDCODE;
                cout << "5\n";
                string log = "";
                if ((pf->BID_PRICE >= pf->ASK_PRICE) || pf->BID_PRICE <= 0 || pf->BID_PRICE >= 1e15 || pf->ASK_PRICE <= 0 || pf->ASK_PRICE >= 1e15)
                {
                    log += "Invalid price ";
                }
                cout << "6\n";
                if (pf->BID_VOLUME < 0 || pf->BID_VOLUME > 1e15 || pf->ASK_VOLUME < 0 || pf->ASK_VOLUME > 1e15)
                {
                    log += "Invalid volume";
                }
                cout << "7\n";
                res.code = Status::WARN;
                char r[200];
                sprintf(r, "{\"FEEDCODE\":\"%s\", \"TIMESTAMP\":%lld, \"BID_PRICE\":%lf, \"ASK_PRICE\":%lf,\"BID_VOLUME\":%lf, \"ASK_VOLUME\":%lf, \"LOG\":\"%s\"}", 
                feedcode.c_str(), exchange_timestamp, pf->BID_PRICE, 
                             pf->ASK_PRICE, pf->BID_VOLUME, pf->ASK_VOLUME, log.c_str());
                cout << "8\n";
                res.json.push_back(string(r));
                cout << "9\n";
                break;
            }
            case 'S':
                break;
            default:
                break;
            }
        }
        catch (int &a)
        {
            cout << "in catch\n";
        }
    }
    else
    {
        cout << "in else\n";
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