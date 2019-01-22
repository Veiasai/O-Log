#pragma once
#include "Context.h"
#include <map>
#include <shared_mutex>

class ContextMap
{
    private:
      map<string, Context *> myMap;
      mutable std::shared_mutex mutex_;
    public:
      ContextMap();
      ~ContextMap();
      bool create(string feedcode);
      bool has(string feedcode);
      Context *getContext(string feedcode);
};