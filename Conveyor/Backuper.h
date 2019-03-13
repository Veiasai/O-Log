#pragma once
#include <string>
#include <fstream>

using namespace std;

class Backuper
{
    private:
      ofstream ofs;
      string filename;

    public:
      int init(const string &_filename);
      int set(uint64_t offset);
      uint64_t get();
};