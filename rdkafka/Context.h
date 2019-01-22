#pragma once
#include "process.h"
#include "utils.h"
#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>

using namespace std;

class feedcmp
{
public:
  bool operator() (const StatisticsFeed*  lhs, const StatisticsFeed* rhs) const
  {
    return (lhs->EXCHANGE_TIMESTAMP > rhs->EXCHANGE_TIMESTAMP);
  }
};

using pq = priority_queue<StatisticsFeed*, std::vector<StatisticsFeed*>, feedcmp>;

class Context
{
    private:
      pq *queue = NULL;
      int64_t expectTimestamp;
      double preValue;
      int32_t preVolume;
      mutable mutex mutex_;
    public:
      Context();
      ~Context();
      void lock();
      void unlock();
      pq *getQueue();
      int64_t getExpectTimestamp();
      void setExpectTimestamp(int64_t timestamp);
      double getPreValue();
      void setPreValue(double value);
      int32_t getPreVolume();
      void setPreVolume(int32_t volume);
};