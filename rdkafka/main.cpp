#include "myConsumer.h"
#include "demo_processor.h"

using namespace std;

bool run = true;

int xmlTest()
{
    TiXmlDocument doc("../cppStream.xml");
    if(!doc.LoadFile())
    {
        cerr<<"Failed to Load file." << endl;
        return 1;
    }
    TiXmlElement* root = doc.FirstChildElement();
    if(root == NULL)
    {
        cerr << "No root element." << endl;
        return 1;
    }
    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
    {
        for(TiXmlElement* conf = elem->FirstChildElement(); conf != NULL; conf = conf->NextSiblingElement())
        {
            if (conf->FirstChildElement() == NULL)
            {
                cout << conf->GetText() << endl;
            }
            else
            {
                for (TiXmlElement *entry = conf->FirstChildElement(); entry != NULL; entry = entry->NextSiblingElement())
                {
                    cout << entry->GetText() << endl;
                }
            }
        }
    }
    return 0;
}
int main(){
    xmlTest();
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

