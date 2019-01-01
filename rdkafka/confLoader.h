#pragma once
#include "lib/tinyxml.h"
#include "myProducer.h"
#include <vector>

typedef pair<RdKafka::Conf*, std::string> HandlerConf;
typedef pair<std::vector<HandlerConf>, std::vector<HandlerConf>> HandlerConfs;

class ConfLoader
{
    public:
      static HandlerConfs load(std::string filename);

    private:
      static HandlerConf loadHandlerConf(TiXmlElement *handler);
      static RdKafka::Conf *loadGlobalConf(TiXmlElement *confList);
      static RdKafka::Conf *loadTopicConf(TiXmlElement *confList);
};