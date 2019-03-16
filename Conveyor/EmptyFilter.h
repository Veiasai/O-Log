#pragma once
#include "Filter.h"

class EmptyFilter : public Filter
{
    public:
      virtual bool match(const string & line) const;
};