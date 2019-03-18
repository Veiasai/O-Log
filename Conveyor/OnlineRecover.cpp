#include "OnlineRecover.h"
#include <unistd.h>

OnlineRecover::OnlineRecover(const string& _brokers, const string& _topic)
{
    cout << "init online recover with " << _brokers << " and " << _topic << endl;
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
        cout << "create consumer" << endl;
        RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
        string errstr = "";
        conf->set("metadata.broker.list", brokers, errstr);
        conf->set("group.id", "recover_group", errstr);
        conf->set("default_topic_conf", tconf, errstr);
        consumer = RdKafka::KafkaConsumer::create(conf, errstr);

        delete tconf;
        delete conf;
        
        if (!consumer) {
            std::cerr << "Failed to create consumer: " << errstr << std::endl;
            return 0;
        }
    }
    vector<string> topics;
    topics.push_back(topic);
    consumer->subscribe(topics);
    vector<RdKafka::TopicPartition* > partitions;
    // while(partitions.size()<=1)
    // {
    //     auto msg = consumer->consume(1000);
    //     delete msg;
    //     consumer->assignment(partitions);
    // }
    // cout << "partitions " << partitions.size() << endl;
    int64_t low = -1;
    int64_t high = -1;
    while(consumer->query_watermark_offsets(topic, 0, &low, &high, 10000) != RdKafka::ERR_NO_ERROR && high >=0)
    {
        cout << "low:" << low << " high:" << high << endl;
    }
    cout << "low:" << low << " high:" << high << endl;
    
    while(partitions.size()<0||partitions[0]->offset()<high)
    {
        auto msg = consumer->consume(1000);
        delete msg;
        consumer->assignment(partitions);
    }
    // consumer->assignment(partitions);
    int expectMsgNum = 0;

    for(auto& partition : partitions)
    {
        cout << "partition offset: " << partition->offset() << endl
             << "partition index: " << partition->partition() << endl
             << "partition topic: " << partition->topic() << endl;
        if(partition->offset()>0)
        {
            partition->set_offset(partition->offset() - 1);
            expectMsgNum++;
        }
    }
    cout << "expectMsgNum " << expectMsgNum << endl;
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
            uint64_t offset = getOffsetFromMessage(msg);
            if(maxOffset < offset)
            {
                maxOffset = offset;
            }
            delete msg;
        }
        return maxOffset;
    }
}

uint64_t OnlineRecover::getOffsetFromMessage(RdKafka::Message *message)
{
    // Json::Reader reader;
    // Json::Value value;
    // if (reader.parse(string(static_cast<const char *>(message->payload())), value))
    // {
    //     try
    //     {
    //         value[""]
    //     }
    //     catch(const std::exception& e)
    //     {
    //         std::cerr << "Error when parse message while getting recover offset." << '\n';
    //         return 0;
    //     }
        
    // }
    cout << "key: " << *(message->key()) << endl;
    cout << "msg: " << message->payload() << endl;
    return std::atol(message->key()->c_str());
}