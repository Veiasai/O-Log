#pragma once
#include "Filter.h"
#include <regex>
#include <iostream>

class RegexFilter : public Filter
{
    private:
      regex rule;

    public:
      virtual bool match(const string & line) const;
      void setRule(string &ruleStr);
};