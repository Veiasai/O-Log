#pragma once
#include "Filter.h"
#include "Backuper.h"
#include "utils.h"
#include "rdkafkacpp.h"
#include <fstream>
#include <cassert>

class Worker
{
    public:
      Worker();
      int init(const FileConf & fileConf, const ProducerConf & producerConf, const Filter *filter, Backuper *backuper);
      void run();
    private:
      int state;
      ifstream inFile;
      RdKafka::Producer * producer;
      RdKafka::Topic * topic;
      int32_t partition;
      const Filter *filter;
      Backuper *backuper;
};