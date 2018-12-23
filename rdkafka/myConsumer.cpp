#include "myConsumer.h"

extern bool run;
static bool exit_eof = false;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;
static long msg_cnt = 0;
static int64_t msg_bytes = 0;
static void sigterm (int sig) {
	  run = false;
}

class ExampleRebalanceCb : public RdKafka::RebalanceCb {
    private:
        static void part_list_print (const std::vector<RdKafka::TopicPartition*>&partitions){
            for (unsigned int i = 0 ; i < partitions.size() ; i++)
            std::cerr << partitions[i]->topic() <<
            "[" << partitions[i]->partition() << "], ";
            std::cerr << "\n";
        }

    public:
        void rebalance_cb (RdKafka::KafkaConsumer *consumer,
                    RdKafka::ErrorCode err,
                            std::vector<RdKafka::TopicPartition*> &partitions) {
            std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

            part_list_print(partitions);

            if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
            partition_cnt = (int)partitions.size();
            } else {
            consumer->unassign();
            partition_cnt = 0;
            }
            eof_cnt = 0;
        }
};

class ExampleEventCb : public RdKafka::EventCb {
    public:
        void event_cb (RdKafka::Event &event) {

            switch (event.type())
            {
            case RdKafka::Event::EVENT_ERROR:
                std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str() << std::endl;
                if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                run = false;
                break;

            case RdKafka::Event::EVENT_STATS:
                std::cerr << "\"STATS\": " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_LOG:
                fprintf(stderr, "LOG-%i-%s: %s\n",
                        event.severity(), event.fac().c_str(), event.str().c_str());
                break;

            case RdKafka::Event::EVENT_THROTTLE:
            std::cerr << "THROTTLED: " << event.throttle_time() << "ms by " <<
            event.broker_name() << " id " << (int)event.broker_id() << std::endl;
            break;

            default:
                std::cerr << "EVENT " << event.type() <<
                    " (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str() << std::endl;
                break;
            }
        }
};

MyConsumer::MyConsumer(){
}

MyConsumer::MyConsumer(std::string ConfPath){
    
    std::string errstr;
    
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    ExampleRebalanceCb ex_rebalance_cb;
    conf->set("rebalance_cb", &ex_rebalance_cb, errstr);
    conf->set("group.id", "test_group_id", errstr);
    conf->set("metadata.broker.list", "ist-slave5:9092", errstr);

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    conf->set("default_topic_conf", tconf, errstr);
    delete tconf;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    delete conf;

    std::cout << "% Created consumer " << consumer->name() << std::endl;
}

MyConsumer::~MyConsumer()
{
    delete consumer;
}

void MyConsumer::subscribe()
{
	std::vector<std::string> topics;
    topics.push_back("fluent-newData");
    RdKafka::ErrorCode err = consumer->subscribe(topics);
    if (err) {
        std::cerr << "Failed to subscribe to " << topics.size() << " topics: "
                << RdKafka::err2str(err) << std::endl;
        exit(1);
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
            msg_cnt++;
            msg_bytes += message->len();
            if (verbosity >= 3)
                std::cerr << "Read msg at offset " << message->offset() << std::endl;
            RdKafka::MessageTimestamp ts;
            ts = message->timestamp();
            if (verbosity >= 2 &&
            ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
            std::string tsname = "?";
            if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
            tsname = "create time";
                else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                tsname = "log append time";
                std::cout << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
            }
            if (verbosity >= 2 && message->key()) {
                std::cout << "Key: " << *message->key() << std::endl;
            }
            if (verbosity >= 1) {
                printf("%.*s\n",
                    static_cast<int>(message->len()),
                    static_cast<const char *>(message->payload()));
            }
            break;

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
