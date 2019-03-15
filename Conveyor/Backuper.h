#pragma once
#include <string>
#include <fstream>

using namespace std;

class Backuper
{
    public:
      virtual int set(uint64_t offset) = 0;
      virtual ~Backuper() = 0;
};