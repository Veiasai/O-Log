#include "Worker.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

static const int batch_size = 20;

Worker::Worker(){
    producer = NULL;
    topic = NULL;
    filter = NULL;
    backuper == NULL;
    partition = 0;
    state = 0;
}


int Worker::init(const FileConf & fileConf, const ProducerConf & producerConf, const Filter * _filter, Backuper * _backuper){
    // init file
    inFile.open(fileConf.filename, ifstream::in);
    assert(inFile.is_open());

    inFile.seekg(fileConf.offset);
    assert(inFile.good());

    // init producer
    string errstr;
    RdKafka::Conf * conf = RdKafka::Conf::create(RdKafka::Conf::ConfType::CONF_GLOBAL); 
    conf->set("bootstrap.servers", producerConf.bootstrap_server, errstr);
    producer = RdKafka::Producer::create(conf, errstr);
    assert(producer);

    RdKafka::Conf * tconf = RdKafka::Conf::create(RdKafka::Conf::ConfType::CONF_TOPIC); 
    topic = RdKafka::Topic::create(producer, producerConf.topic, tconf, errstr);
    assert(topic);

    partition = RdKafka::Topic::PARTITION_UA;

    // filter and backuper
    filter = _filter;
    backuper = _backuper;

    state = 1;
}

 /**
   * @brief After Worker::init(), call run() in a new thread.
   *       If init() goes well, the Worker will begin to read file and send message to Kafka.
   *        Otherwise, return instantly.
   */
void Worker::run(){
    if (state != 1) return;

    while(1){
        int sleep_times = 0;
        u_int64_t offset = inFile.tellg();
        string offset_s = to_string(offset);
        int i = 0;
        for (;i<batch_size;){
            string line;
            if (getline(inFile, line)){
                if (filter->match(line)){
                    producer->produce(
                        topic, 
                        partition, 
                        RdKafka::Producer::RK_MSG_COPY, 
                        &line,
                        line.size(),
                        &offset_s, 
                        NULL); 
                    i++;
                }
                offset = inFile.tellg();
                offset_s = to_string(offset);
            }else if(sleep_times < 5){
                sleep_times++;
                sleep(1);
            }else{
                // avoid sleeping forever when get tail of file
                break;
            }
        }

        // check: at least one send
        if (i > 0){
            producer->flush(0);
            backuper->set(offset);
        }
    }
}