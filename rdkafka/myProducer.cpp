#include "myProducer.h"

MyProducer::MyProducer(HandlerConf handlerConf)
{
    std::string errstr = "";
    
    producer = RdKafka::Producer::create(handlerConf.first.first, errstr);
    delete handlerConf.first.first;
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }
    std::cout << "% Created producer " << producer->name() << std::endl;

    topic = RdKafka::Topic::create(producer, handlerConf.second[0], handlerConf.first.second, errstr);
    delete handlerConf.first.second;
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }
}

MyProducer::~MyProducer()
{
    delete topic;
    delete producer;
}

void MyProducer::produce(std::string message)
{
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    RdKafka::ErrorCode resp =producer->produce(topic, partition,
			  RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
			  const_cast<char *>(message.c_str()), message.size(),
			  NULL, NULL);
    if (resp != RdKafka::ERR_NO_ERROR){
	    // std::cerr << "% Produce failed: " <<RdKafka::err2str(resp) << std::endl;
    }
    else{
        // std::cerr << "% Produced message (" << message.size() << " bytes)" <<std::endl;
    }
    poll(0);
}

void MyProducer::poll(int timeout_ms)
{
    producer->poll(timeout_ms);
}
