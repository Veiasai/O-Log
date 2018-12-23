#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

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

#include <librdkafka/rdkafkacpp.h>
#include <lib/tinyxml2.h>

class MyConsumer
{
    private:
        RdKafka::KafkaConsumer *consumer;
        void msg_consume(RdKafka::Message* message, void* opaque);
        
    public:
        MyConsumer(string ConfPath);
        ~MyConsumer();
        void subscribe();
        void consume();
};