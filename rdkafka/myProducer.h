#pragma once
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <vector>

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#include <atltime.h>
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#include <unistd.h>
#endif

#include "lib/tinyxml.h"
#include "confLoader.h"

class MyProducer
{
    private:
        RdKafka::Producer *producer = NULL;
        RdKafka::Topic *topic = NULL;

      public:
        MyProducer(HandlerConf handlerConf);
        ~MyProducer();
        void produce(std::string message);
        void poll(int timeout_ms);
};