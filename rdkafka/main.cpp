#include "myConsumer.h"
#include "demo_processor.h"
#include "confLoader.h"

bool run = true;

int main(){
    HandlerConfs handlerConfs= ConfLoader::load("./conf/cppStream.xml");
    if (handlerConfs.first.size()==0 || handlerConfs.second.size()==0)
    {
        std::cerr << handlerConfs.first.size() << std::endl
                  << handlerConfs.second.size() << std::endl;
        std::cerr << "Load Conf Error." << std::endl;
        return 1;
    }

    MyConsumer myConsumer(handlerConfs.first[0]);
    MyProducer myProducer(handlerConfs.second[0]);

    Processor* dp = new Demo_processor();
    // dp->exec("{\"@timestamp\":1545893491.708491, \"log_time\":\"1545893491503\", \"MessageType\":\"LIMon_StatisticsFeed\", \"detail\":\"rb1000,2306359659839382000,6414.451840,6414.451840,6468.784659,6468.784659,CLOSING_PRICE_PREVIOUS_TRADING_DAY,6414.451840,1,6414.451840,1,0,6468.784659,523,1545893491502980951,7115.663125,5821.906193,54465099783379212,1545893491500000000\"}");
    myConsumer.setProcessor(dp);
    myConsumer.setProducer(&myProducer);
    myProducer.produce("init");
    myConsumer.subscribe();
    while(run){
        myConsumer.consume();
        myProducer.poll(0);
    }
}

