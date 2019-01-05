#include "myConsumer.h"
#include "demo_processor.h"
#include "confLoader.h"

bool run = true;

int main(){
    HandlerConfs handlerConfs= ConfLoader::load("rdkafka/conf/cppStream.xml");
    // if (handlerConfs.first.size()==0 || handlerConfs.second.size()==0)
    // {
    //     std::cerr << "Load Conf Error." << std::endl;
    //     return 1;
    // }

    vector<MyConsumer> myConsumers;
    for (auto& handlerConf : handlerConfs.first)
    {
        myConsumers.emplace_back(MyConsumer(handlerConf));
    }

    vector<MyProducer> myProducers;
    for (auto& handlerConf : handlerConfs.second)
    {
        myProducers.emplace_back(MyProducer(handlerConf));
    }

    Processor *dp = new Demo_processor();

    for (int i = 0; i < myConsumers.size(); i++)
    {
        if (!myProducers.empty())
        {
            myConsumers[i].setProducer(&myProducers[i%myProducers.size()]);
        }
        myConsumers[i].setProcessor(dp);
        myConsumers[i].subscribe();
    }
    std::cout << "init success" << std::endl;
    // dp->exec("{\"@timestamp\":1545893491.708491, \"log_time\":\"1545893491503\", \"MessageType\":\"LIMon_StatisticsFeed\", \"detail\":\"rb1000,2306359659839382000,6414.451840,6414.451840,6468.784659,6468.784659,CLOSING_PRICE_PREVIOUS_TRADING_DAY,6414.451840,1,6414.451840,1,0,6468.784659,523,1545893491502980951,7115.663125,5821.906193,54465099783379212,1545893491500000000\"}");
    // myConsumer.setProcessor(dp);
    // myConsumer.setProducer(&myProducer);
    // myProducer.produce("init");
    // myConsumer.subscribe();
    while(run){
        for (auto& myConsumer : myConsumers)
        {
            myConsumer.consume();
        }
        for (auto& myProducer : myProducers)
        {
            myProducer.poll(0);
        }
    }
}

