#include "myConsumer.h"

bool run = true;

int main(){
    MyConsumer myConsumer("consumer.xml");
    myConsumer.subscribe();
    while(run){
        myConsumer.consume();
    }
}

