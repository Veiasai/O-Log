#include "../lib/tinyxml.h"
#include <iostream>

using namespace std;

int main(){
    TiXmlDocument doc("../conf/cppStream.xml");
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