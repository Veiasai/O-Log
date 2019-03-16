#include "RegexFilter.h"

bool RegexFilter::match(const string & line) const
{
    return regex_match(line, rule);
}

void RegexFilter::setRule(string &ruleStr)
{
    std::cout << "Filter Rule:" << ruleStr << endl;
    rule = regex(ruleStr.c_str());
}