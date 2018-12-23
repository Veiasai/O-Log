#include "processor.h"
#include "json/json.h"

int main(){
    std::string strValue = "{\"key1\":\"value\"}";

    Json::Reader reader;
    Json::Value value;

    if (reader.parse(strValue, value))
    {
        std::string out = value["key1"].asString();
        std::cout << out << endl;
    }
}
