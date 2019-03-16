#pragma once
#include "lib/tinyxml.h"
#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "Filter.h"
#include "OnlineBackuper.h"
#include "DiskBackuper.h"
#include "OnlineRecover.h"
#include "DiskRecover.h"
#include "EmptyFilter.h"

class ConfLoader
{
    public:
      static vector<WorkerConfGroup*> load(string filename);
    private:
      static vector<WorkerConfGroup *> loadQuotas(TiXmlElement *quotas);
      static WorkerConf *loadWorkerConf(TiXmlElement *worker);
      static Filter *loadFilter(TiXmlElement *filterElement);
};