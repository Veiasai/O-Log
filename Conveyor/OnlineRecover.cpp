#include "OnlineRecover.h"

OnlineRecover::OnlineRecover(const string& _brokers, const string& _topic)
{
    brokers = _brokers;
    topic = _topic;
}

OnlineRecover::~OnlineRecover()
{
    if(consumer != NULL)
    {
        delete consumer;
        consumer = NULL;
    }
}

uint64_t OnlineRecover::getOffset()
{
    if(consumer == NULL)
    {
        RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
        string errstr = "";
        conf->set("metadata.broker.list", brokers, errstr);
        conf->set("default_topic_conf", tconf, errstr);
        consumer = RdKafka::KafkaConsumer::create(conf, errstr);

        delete tconf;
        delete conf;
        
        if (!consumer) {
            std::cerr << "Failed to create consumer: " << errstr << std::endl;
            return 0;
        }
    }
    consumer->unsubscribe();
    vector<string> topics;
    topics.push_back(topic);
    consumer->subscribe(topics);
    vector<RdKafka::TopicPartition* > partitions;
    consumer->position(partitions);
    int expectMsgNum = 0;
    for(auto& partition : partitions)
    {
        if(partition->offset()>0)
        {
            partition->set_offset(partition->offset() - 1);
            expectMsgNum++;
        }
    }
    vector<RdKafka::Message *> msgs;
    while(msgs.size()<expectMsgNum)
    {
        auto msg = consumer->consume(1000);
        if(msg->err() == RdKafka::ERR_NO_ERROR)
        {
            msgs.push_back(msg);
        }
        else
        {
            delete msg;
        }
    }

    delete consumer;
    consumer = NULL;

    if (msgs.size()==0)
    {
        return 0;
    }
    else
    {
        uint64_t maxOffset = 0;
        for(auto msg : msgs)
        {
            msg->payload();
        }
        return maxOffset;
    }
}