#include "utils.h"

WorkerConfGroup::WorkerConfGroup()
{
    workerConfs.clear();
    quota = 0;
    hostId = "";
}

WorkerConfGroup::~WorkerConfGroup()
{
    for(auto& workerConf : workerConfs)
    {
        if(workerConf != NULL)
        {
            delete workerConf;
            workerConf = NULL;
        }
    }
}

WorkerConf::WorkerConf()
{
    fileConf = NULL;
    producerConf = NULL;
    filter = NULL;
    backuper = NULL;
}

WorkerConf::~WorkerConf()
{
    if(fileConf != NULL)
    {
        delete fileConf;
        fileConf = NULL;
    }
    if(producerConf != NULL)
    {
        delete producerConf;
        producerConf = NULL;
    }
    if(recover != NULL)
    {
        delete recover;
        recover = NULL;
    }
    filter = NULL;
    backuper = NULL;
}