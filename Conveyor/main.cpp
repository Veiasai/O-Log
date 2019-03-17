#include "ConfLoader.h"
#include "QuotaManager.h"
#include "Worker.h"
#include <thread>

int main()
{
    vector<WorkerConfGroup *> workerConfGroups = ConfLoader::load("conf/conveyor.xml");
    QuotaManager quotaMananger;
    vector<thread> workerthreads;
    for (int i = 0; i < workerConfGroups.size();i++)
    {
        // string userId = workerConfGroups[i]->hostId + "_Level_" + to_string(i + 1);
        // quotaMananger.Create(userId, NULL, workerConfGroups[i]->quota);
        string userId = "";
        for(auto& workerConf : workerConfGroups[i]->workerConfs)
        {
            workerConf->producerConf->clientId = workerConf->fileConf->filename;
            workerConf->producerConf->userId = userId;

            // Worker *worker = new Worker();
            // worker->init(*workerConf->fileConf, *workerConf->producerConf, workerConf->filter, workerConf->backuper);
            // delete worker;
            
            uint64_t offset = workerConf->recover->getOffset();
            cout << "recover " << workerConf->producerConf->topic << " from " << offset << endl;
            workerConf->fileConf->offset = offset;
            Worker *worker = new Worker();
            //Tips: fileConf and producerConf will be delete after worker inited, so don't keep those two instance
            //Tips: filter and backuper's life-cycle should be managed by worker, so don't forget to delete them before worker died
            if(worker->init(*workerConf->fileConf, *workerConf->producerConf, workerConf->filter, workerConf->backuper)==0)
            {
                workerthreads.emplace_back(thread(&Worker::run, worker));
            }
            else
            {
                delete worker;
                worker = NULL;
            }
        }
    }

    for(auto& workerConfGroup : workerConfGroups)
    {
        if(workerConfGroup != NULL)
        {
            delete workerConfGroup;
            workerConfGroup = NULL;
        }
    }

    for(auto& thread:workerthreads)
    {
        thread.join();
    }
}