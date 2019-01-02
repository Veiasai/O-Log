#include "../lib/tinyxml.h"
#include <iostream>

using namespace std;

void loadGlobalConf(TiXmlElement *confList);
void loadTopicConf(TiXmlElement *confList);
void loadHandlerConf(TiXmlElement *handler);

void load(std::string filename)
{
    TiXmlDocument doc(filename.c_str());
    if(!doc.LoadFile())
    {
        return;
    }
    TiXmlElement* root = doc.FirstChildElement();

    TiXmlElement *consumers = root ? root->FirstChildElement() : NULL;
    TiXmlElement *producers = consumers ? consumers->NextSiblingElement() : NULL;

    if (consumers != NULL)
    {
        for (TiXmlElement *consumer = consumers->FirstChildElement(); consumer != NULL; consumer = consumer->NextSiblingElement())
        {
            loadHandlerConf(consumer);
        }
    }

    if (producers != NULL)
    {
        for (TiXmlElement *producer = producers->FirstChildElement(); producer != NULL; producer = producer->NextSiblingElement())
        {
            loadHandlerConf(producer);
        }
    }
    return;
}

void loadHandlerConf(TiXmlElement *handler)
{
    TiXmlElement *topic = handler->FirstChildElement();
    cout << topic->GetText() << endl;
    TiXmlElement *globalConfList = topic ? topic->NextSiblingElement() : NULL;
    TiXmlElement *topicConfList = globalConfList ? globalConfList->NextSiblingElement() : NULL;
    loadGlobalConf(globalConfList);
    loadTopicConf(topicConfList);
    return;
}

void loadGlobalConf(TiXmlElement *confList)
{
    for (TiXmlElement *entry = confList->FirstChildElement(); entry != NULL; entry = entry->NextSiblingElement())
    {
        TiXmlElement *key = entry->FirstChildElement();
        TiXmlElement *value = key->NextSiblingElement();
        cout << key->GetText() << endl
             << value->GetText() << endl;
    }
    return;
}

void loadTopicConf(TiXmlElement *confList)
{
    for (TiXmlElement *entry = confList->FirstChildElement(); entry != NULL; entry = entry->NextSiblingElement())
    {
        TiXmlElement *key = entry->FirstChildElement();
        TiXmlElement *value = key->NextSiblingElement();
        cout << key->GetText() << endl
             << value->GetText() << endl;
    }
    return;
}

int main()
{
    load("../conf/cppStream.xml");
}
