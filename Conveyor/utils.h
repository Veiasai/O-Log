#pragma once
#include <string>

using namespace std;

class FileConf
{
    public:
      string filename;
      uint32_t offset;
};

class ProducerConf
{
    public:
      string topic;
      string userId;
      string clientId;
};