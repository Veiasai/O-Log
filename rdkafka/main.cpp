#include "myConsumer.h"

bool run = true;

int main(){
    MyConsumer myConsumer("consumer.xml");
    MyProducer myProducer("producer.xml");
    MyConsumer.setProducer(&myProducer);
    myConsumer.subscribe();
    while(run){
        myConsumer.consume();
        myProducer.poll(0);
    }
}

