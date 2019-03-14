#pragma once
#include <string>

using namespace std;

class Filter
{
    public:
        virtual bool match(const string & line) const;
        Filter();
        virtual ~Filter();
};