#include "myConsumer.h"

int main(){
    MyConsumer myConsumer("consumer.xml");
    myConsumer.subscribe();
    while(run){
        myConsumer.consume();
    }
}

