#include "ConfLoader.h"
#include "Filter.h"
#include "RegexFilter.h"


vector<WorkerConfGroup*> ConfLoader::load(string filename)
{
    vector<WorkerConfGroup *> workerConfGroups;
    std::cerr << filename.c_str() << std::endl;
    TiXmlDocument doc(filename.c_str());
    if(!doc.LoadFile())
    {
        return workerConfGroups;
    }
    TiXmlElement *root = doc.FirstChildElement();
    TiXmlElement *host = root ? root->FirstChildElement() : NULL;
    TiXmlElement *quotas = host ? host->NextSiblingElement() : NULL;
    TiXmlElement *groups = quotas ? quotas->NextSiblingElement() : NULL;
    string hostId = "";
    if(host!=NULL)
    {
        hostId = host->GetText();
    }
    if(quotas!=NULL)
    {
        workerConfGroups = loadQuotas(quotas);
    }
    if(groups!=NULL)
    {
        TiXmlElement *group = groups->FirstChildElement();
        for(auto& workerConfGroup : workerConfGroups)
        {
            workerConfGroup->hostId = hostId;
            if (group != NULL)
            {
                for (TiXmlElement *worker = group->FirstChildElement(); worker != NULL;worker=worker->NextSiblingElement())
                {
                    workerConfGroup->workerConfs.push_back(loadWorkerConf(worker));
                }
                group = group->NextSiblingElement();
            }
        }
    }
    return workerConfGroups;
}

vector<WorkerConfGroup *> ConfLoader::loadQuotas(TiXmlElement *quotas)
{
    vector<WorkerConfGroup *> workerConfGroups;
    if(quotas != NULL)
    {
        for (TiXmlElement *quota = quotas->FirstChildElement(); quota != NULL;quota=quota->NextSiblingElement())
        {
            WorkerConfGroup *workerConfGroup = new WorkerConfGroup();
            workerConfGroup->quota = atoll(quota->GetText());
            workerConfGroups.push_back(workerConfGroup);
        }
    }
    return workerConfGroups;
}

WorkerConf* ConfLoader::loadWorkerConf(TiXmlElement *worker)
{
    WorkerConf *workerConf = NULL;
    if(worker != NULL)
    {
        workerConf = new WorkerConf();
        workerConf->fileConf = new FileConf();
        workerConf->producerConf = new ProducerConf();
        TiXmlElement *element = worker->FirstChildElement();
        workerConf->fileConf->filename = element->GetText();
        element = element->NextSiblingElement();
        workerConf->producerConf->bootstrap_server = element->GetText();
        element = element->NextSiblingElement();
        workerConf->producerConf->topic = element->GetText();
        element = element->NextSiblingElement();
        workerConf->filter = loadFilter(element->FirstChildElement());
        string backupFilename = workerConf->fileConf->filename.substr(0, workerConf->fileConf->filename.find_first_of('.'));
        backupFilename += ".backup";
        Backuper *backuper = new Backuper();
        backuper->init(backupFilename);
        workerConf->backuper = backuper;
    }
    return workerConf;
}

Filter* ConfLoader::loadFilter(TiXmlElement *filterElement)
{
    Filter *filter = NULL;
    if(filterElement != NULL)
    {
        if(strncmp(filterElement->FirstChildElement()->GetText(),"regex",sizeof("regex"))==0)
        {
            RegexFilter* regexFilter = new RegexFilter();
            string rule = filterElement->FirstChildElement()->NextSiblingElement()->GetText();
            regexFilter->setRule(rule);
            filter = regexFilter;
        }
    }
    return filter;
}