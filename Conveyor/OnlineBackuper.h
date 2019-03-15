#pragma once
#include "Backuper.h"

class OnlineBackuper : public Backuper
{
    public:
      virtual int set(uint64_t offset);
};