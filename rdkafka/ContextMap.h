#pragma once
#include "Context.h"
#include <map>
#include <shared_mutex>
#include "EventLoop.h"
#include "Backuper.h"

class EventLoop;
class Backuper;

class ContextMap
{
    private:
      map<string, Context *> myMap;
      mutable std::shared_mutex mutex_;
      vector<EventLoop *> myEventLoops;

    public:
      ContextMap();
      ~ContextMap();
      bool create(string feedcode);
      bool has(string feedcode);
      Context *getContext(string feedcode);
      void addEventLoop(EventLoop *eventLoop);
      void backupMetaData(Backuper *backuper);
};