#include "myMessage.h"

MyMessage::MyMessage(RdKafka::Message* message){
    this->_message = message;
    this->_live = 0;
    if (reader.parse(this->message, value))
    {
        try
        {
            string msg_type = value["MessageType"].asString();
            switch (msg_type[6])
            {
            case 'P':
            {
                PriceFeed *pf = new PriceFeed();
                pf->build(value["detail"].asString());
                this->feed = pf;
                this->type = Type::feedtype::Price;
            }
            case 'S':
                StatisticsFeed * sf = new StatisticsFeed();
                sf->build(value["detail"].asString());
                this->feed = sf;
                this->type = Type::feedtype::Statistics;
            default:
                break;
            }
        }
        catch (...)
        {
            // cout << "in catch\n";
        }
    }
    else
    {
        // cout << "in else\n";
    }
};

MyMessage::~MyMessage(){
    // here delete void *, be careful that feed has no destructor
    delete this->_feed;
    delete this->_message;
};

RdKafka::Message* MyMessage::message(){
    return this->_message;
};

const void * const MyMessage::feed(){
    return this->_feed;
}

void MyMessage::live(){
    this->_live++;
}

void MyMessage::kill(){
    assert(this->_live > 0);
    this->_live--;
}

bool MyMessage::free(){
    return this->_live == 0;
}
