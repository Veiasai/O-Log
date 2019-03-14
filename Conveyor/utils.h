#pragma once
#include <string>
#include "Filter.h"
#include "Backuper.h"
#include <vector>

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
      string bootstrap_server;
      string topic;
      string userId;
      string clientId;
};

class WorkerConf
{
    public:
      FileConf* fileConf;
      ProducerConf* producerConf;
      Filter *filter;
      Backuper *backuper;
      WorkerConf();
      ~WorkerConf();
};

class WorkerConfGroup
{
  public:
    vector<WorkerConf *> workerConfs;
    uint64_t quota;
    string hostId;
    WorkerConfGroup();
    ~WorkerConfGroup();
};