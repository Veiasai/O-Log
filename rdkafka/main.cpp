#include "myConsumer.h"

bool run = true;

int main(){
    std::cout<<"0"<<std::endl;
    MyConsumer myConsumer("consumer.xml");
    std::cout<<"1"<<std::endl;
    myConsumer.subscribe();
    std::cout<<"2"<<std::endl;
    while(run){
        myConsumer.consume();
    }
}

