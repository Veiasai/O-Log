#include "utils.h"
#include <iostream>
#include <sstream>

using namespace std;

int StatisticsFeed::build(string body){
    try{
        stringstream ss;
        for (auto & c : body)
            if (c == ',') c = ' ';
        ss.str(body);
        // ss >> TIMESTAMP;
        // ss.ignore(256, ',');    // ignore until ,  ([DATA_RECORDER] [Data] LIMon_StatisticsFeed)
        ss  >> FEEDCODE >> INSTRUMENT_ID >> HIGH_PRICE >> LOW_PRICE >> OPENING_PRICE >> CLOSING_PRICE >> CLOSING_PRICE_TYPE
            >> LAST_PRICE >> LAST_VOLUME >> TURNOVER_VALUE >> TURNOVER_VOLUME >> TURNOVER_TRADE_COUNT >> SETTLEMENT_PRICE
            >> STREAM_ID >> EVENT_TIME >> UPPER_PRICE_LIMIT >> LOWER_PRICE_LIMIT >> OPEN_INTEREST >> EXCHANGE_TIMESTAMP;
    }catch(exception e){
        cout << "statistic build error" << endl;
        return 1;
    }
    return 0;
}
    