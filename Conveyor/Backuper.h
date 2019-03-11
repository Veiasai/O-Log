#pragma once

class Backuper
{
    public:
        int init(const string & store);
        int set(const string &);
        const string get();
};