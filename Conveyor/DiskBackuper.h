#pragma once
#include "Backuper.h"
#include <iostream>

class DiskBackuper : public Backuper
{
    private:
      ofstream ofs;

    public:
      virtual int set(uint64_t offset);
      DiskBackuper(const string &filename);
};
