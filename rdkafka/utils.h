#pragma once
#include <string>

using namespace std;

class Status{
    public:
        enum code {OK, ERROR, WARN};
};

class StatisticsFeed{
    public:
        int build(string body);

    public:
        int32_t TIMESTAMP;
        string FEEDCODE;
        int64_t INSTRUMENT_ID;
        double HIGH_PRICE;
        double LOW_PRICE;
        double OPENING_PRICE;
        double CLOSING_PRICE;
        string CLOSING_PRICE_TYPE;
        double LAST_PRICE;
        int32_t LAST_VOLUME;
        double TURNOVER_VALUE;
        int32_t TURNOVER_VOLUME;
        uint32_t TURNOVER_TRADE_COUNT;
        double SETTLEMENT_PRICE;
        uint32_t STREAM_ID;
        int64_t EVENT_TIME;
        double UPPER_PRICE_LIMIT;
        double LOWER_PRICE_LIMIT;
        int64_t OPEN_INTEREST;
        int64_t EXCHANGE_TIMESTAMP;
};
    