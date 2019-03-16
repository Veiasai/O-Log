#include "RegexFilter.h"

bool RegexFilter::match(const string & line) const
{
    return regex_match(line, rule);
}

void RegexFilter::setRule(string &ruleStr)
{
    rule = regex(ruleStr.c_str());
}