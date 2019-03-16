#pragma once
#include <string.h>
#include <iostream>
#include "rdkafkacpp.h"

using namespace std;

class Recover
{
    public:
      virtual uint64_t getOffset() = 0;
      virtual ~Recover(){};
};