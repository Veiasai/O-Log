#include "confLoader.h"

HandlerConfs ConfLoader::load(std::string filename)
{
    HandlerConfs handlerConfs;
    TiXmlDocument doc(filename.c_str());
    if(!doc.LoadFile())
    {
        return handlerConfs;
    }
    TiXmlElement* root = doc.FirstChildElement();

    TiXmlElement *consumers = root ? root->FirstChildElement() : NULL;
    TiXmlElement *producers = consumers ? consumers->NextSiblingElement() : NULL;

    if (consumers != NULL)
    {
        for (TiXmlElement *consumer = consumers->FirstChildElement(); consumer != NULL; consumer = consumer->NextSiblingElement())
        {
            handlerConfs.first.push_back(loadHandlerConf(consumer));
        }
    }

    if (producers != NULL)
    {
        for (TiXmlElement *producer = producers->FirstChildElement(); producer != NULL; producer = producer->NextSiblingElement())
        {
            handlerConfs.second.push_back(loadHandlerConf(producer));
        }
    }
    return handlerConfs;
}

HandlerConf ConfLoader::loadHandlerConf(TiXmlElement *handler)
{
    HandlerConf handlerConf;
    TiXmlElement *topic = handler->FirstChildElement();
    handlerConf.second = topic->GetText();
    TiXmlElement *globalConfList = topic ? topic->NextSiblingElement() : NULL;
    TiXmlElement *topicConfList = globalConfList ? globalConfList->NextSiblingElement() : NULL;
    RdKafka::Conf *globalConf = loadGlobalConf(globalConfList);
    RdKafka::Conf *topicConf = loadTopicConf(topicConfList);
    std::string errstr;
    globalConf->set("default_topic_conf", topicConf, errstr);
    delete topicConf;
    handlerConf.first = globalConf;
    return handlerConf;
}

RdKafka::Conf* ConfLoader::loadGlobalConf(TiXmlElement *confList)
{
    RdKafka::Conf *globalConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;
    for (TiXmlElement *entry = confList->FirstChildElement(); entry != NULL; entry = entry->NextSiblingElement())
    {
        TiXmlElement *key = entry->FirstChildElement();
        TiXmlElement *value = key->NextSiblingElement();
        globalConf->set(key->GetText(), value->GetText(), errstr);
    }
    return globalConf;
}

RdKafka::Conf* ConfLoader::loadTopicConf(TiXmlElement *confList)
{
    RdKafka::Conf *topicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    std::string errstr;
    for (TiXmlElement *entry = confList->FirstChildElement(); entry != NULL; entry = entry->NextSiblingElement())
    {
        TiXmlElement *key = entry->FirstChildElement();
        TiXmlElement *value = key->NextSiblingElement();
        topicConf->set(key->GetText(), value->GetText(), errstr);
    }
    return topicConf;
}