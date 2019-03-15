#include "DiskRecover.h"

uint64_t DiskRecover::getOffset()
{
    ifstream ifs(filename);
    if(ifs.is_open())
    {
        uint64_t offset = 0;
        while(!ifs.eof())
        {
            ifs >> offset;
        }
        ifs.close();
        return offset;
    }
    else
    {
        return 0;
    }
}

DiskRecover::DiskRecover(const string &_filename)
{
    filename = _filename;
}