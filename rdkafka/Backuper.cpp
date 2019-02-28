#include "Backuper.h"

void Backuper::run()
{
    while(true)
    {
        backup();
        sleep(1000);
    }
}

void Backuper::addContextMap(ContextMap *contextMap)
{
    contextMaps.push_back(contextMap);
}

void Backuper::backup()
{
    for (auto contextMap : contextMaps)
    {
        contextMap->backupMetaData(this);
    }
}

void Backuper::produce(string message)
{
    myProducer->produce(message);
}

void Backuper::setProducer(HandlerConf handlerConf)
{
    myProducer = new MyProducer(handlerConf);
}