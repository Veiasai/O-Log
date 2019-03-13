#include "Backuper.h"

int Backuper::init(const string & _filename)
{
    filename = _filename;
    ofs.open(_filename, ios::app);
    if(ofs.is_open())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int Backuper::set(uint64_t offset)
{
    ofs << offset;
    ofs << endl;
    ofs.flush();
    return 0;
}

uint64_t Backuper::get()
{
    ifstream ifs(filename);
    if(ifs.is_open())
    {
        uint64_t offset = 0;
        while(!ifs.eof())
        {
            ifs >> offset;
        }
        return offset;
    }
    else
    {
        return 0;
    }
}