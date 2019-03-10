#pragma once

#include <stdint.h>
#include <string>

using namespace std;

class QuotaManager
{
    public:
      void Create(string userId, string clientId, uint64_t quota);
      void Delete(string userId, string clientId);
};