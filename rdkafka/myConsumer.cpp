#include "myConsumer.h"

extern bool run;
static bool exit_eof = false;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;
static void sigterm (int sig) {
	  run = false;
}

class ExampleRebalanceCb : public RdKafka::RebalanceCb {
    private:
        static void part_list_print (const std::vector<RdKafka::TopicPartition*>&partitions){
            std::cout << "start part list print\n";
            for (unsigned int i = 0 ; i < partitions.size() ; i++)
            std::cerr << partitions[i]->topic() <<
            "[" << partitions[i]->partition() << "], ";
            std::cerr << "\n";
        }

    public:
        void rebalance_cb (RdKafka::KafkaConsumer *consumer,
                RdKafka::ErrorCode err,
                    std::vector<RdKafka::TopicPartition*> &partitions) {
        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            // application may load offets from arbitrary external
            // storage here and update \p partitions
            consumer->assign(partitions);
        } else if (err == RdKafka::ERR__REVOKE_PARTITIONS) {
            // Application may commit offsets manually here
            // if auto.commit.enable=false
            consumer->unassign();
        } else {
            std::cerr << "Rebalancing error:" <<
                        RdKafka::err2str(err) << std::endl;
        }
    }
};

class ExampleEventCb : public RdKafka::EventCb {
    public:
        void event_cb (RdKafka::Event &event) {
            std::cout << "start event_cb\n";
            switch (event.type())
            {
            case RdKafka::Event::EVENT_ERROR:
            
                std::cout << "event_error\n";
                std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str() << std::endl;
                if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                run = false;
                break;

            case RdKafka::Event::EVENT_STATS:
                std::cout << "event_stats\n";
                std::cerr << "\"STATS\": " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_LOG:
                std::cout << "event_log\n";
                fprintf(stderr, "LOG-%i-%s: %s\n",
                        event.severity(), event.fac().c_str(), event.str().c_str());
                break;

            case RdKafka::Event::EVENT_THROTTLE:
                std::cout << "event_throttle\n";
                std::cerr << "THROTTLED: " << event.throttle_time() << "ms by " <<
                event.broker_name() << " id " << (int)event.broker_id() << std::endl;
                break;

            default:
                std::cout << "event_unknow\n";
                std::cerr << "EVENT " << event.type() <<
                    " (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str() << std::endl;
                break;
            }
        }
};

MyConsumer::MyConsumer(){
}

MyConsumer::MyConsumer(HandlerConf handlerConf){
    
    std::string errstr;
    topics = std::move(handlerConf.second);
    std::cout << topics.size() << std::endl;
    ExampleEventCb ex_event_cb;
    handlerConf.first.first->set("event_cb", &ex_event_cb, errstr);
    handlerConf.first.first->set("default_topic_conf", handlerConf.first.second, errstr);
    delete handlerConf.first.second;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    consumer = RdKafka::KafkaConsumer::create(handlerConf.first.first, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    delete handlerConf.first.first;

    std::cout << "% Created consumer " << consumer->name() << std::endl;
}

MyConsumer::~MyConsumer()
{
    delete consumer;
}

void MyConsumer::subscribe()
{
    std::cout << topics.size() << std::endl;
    std::cout << topics[0] << std::endl;
    RdKafka::ErrorCode err = consumer->subscribe(topics);
    std::cout << 5 << std::endl;
    if (err)
    {
        std::cerr << "Failed to subscribe to " << topics.size() << " topics: "
                << RdKafka::err2str(err) << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "subscribe success" << std::endl;
    }
}

void MyConsumer::consume()
{
    RdKafka::Message *msg = consumer->consume(1000);
    msg_consume(msg, NULL);
    delete msg;
}

void MyConsumer::msg_consume(RdKafka::Message* message, void* opaque) {
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            {
                // printf("%.*s\n", static_cast<int>(message->len()), static_cast<const char *>(message->payload()));
                // char buffer[static_cast<int>(message->len()) + 1];
                // printf(buffer, "%.*s", static_cast<int>(message->len()), static_cast<const char *>(message->payload()));
                // std::cout<<"input: "<<getCurrentTime()<< std::endl;
                std::string messageStr(static_cast<const char *>(message->payload()));
                // auto inputTime = std::chrono::duration_cast<chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                // std::cout<<"input: "<<inputTime.count()<<std::endl;
                processor->exec(messageStr);
                Pro_res res = processor->getResult();
                if (res.code != Status::OK){
                    // auto outputTime = std::chrono::duration_cast<chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                    // std::cout<<"output: "<<outputTime.count()<< std::endl;
                    producer->produce(res.json);
                }
                break;
            }

        case RdKafka::ERR__PARTITION_EOF:
            /* Last message */
            if (exit_eof && ++eof_cnt == partition_cnt) {
                std::cerr << "%% EOF reached for all " << partition_cnt <<
                    " partition(s)" << std::endl;
                run = false;
            }
            break;

        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run = false;
            break;

        default:
            /* Errors */
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run = false;
    }
}

void MyConsumer::setProducer(MyProducer *myProducer)
{
    producer = myProducer;
}

void MyConsumer::setProcessor(Processor * p){
    processor = p;
}