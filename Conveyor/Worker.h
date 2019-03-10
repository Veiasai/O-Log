#pragma once
#include "Filter.h"
#include "Backuper.h"
#include "utils.h"

class Worker
{
    public:
      Worker(FileConf fileConf, ProducerConf producerConf, Filter *filter, Backuper *backuper);
      void run();
};