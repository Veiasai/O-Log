#pragma once
#include "Recover.h"

using namespace std;

class DiskRecover : public Recover
{
    public:
      virtual uint64_t getOffset();
      DiskRecover(const string &_filename);

    private:
      string filename;
};