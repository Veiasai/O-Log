#pragma once
#include <vector>
#include "EventLoop.h"
#include "confLoader.h"

using namespace std;
class ContextMap;

class Backuper
{
    private:
      vector<ContextMap *> contextMaps;
      MyProducer *myProducer = NULL;
      void backup();

    public:
      void run();
      void addContextMap(ContextMap *contextMap);
      void produce(string message);
      void setProducer(HandlerConf handlerConf);
};