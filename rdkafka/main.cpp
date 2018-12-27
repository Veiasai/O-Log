#include "myConsumer.h"
#include "demo_processor.h"

bool run = true;

int main(){
    MyConsumer myConsumer("consumer.xml");
    MyProducer myProducer("producer.xml");

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

