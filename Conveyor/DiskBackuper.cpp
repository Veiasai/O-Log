#include "DiskBackuper.h"

int DiskBackuper::set(uint64_t offset)
{
    ofs << offset;
    ofs << endl;
    ofs.flush();
    return 0;
}

DiskBackuper::DiskBackuper(const string &_filename)
{
    ofs.open(_filename, ios::app);
    if(ofs.is_open())
    {
        cout << "open " << _filename << " success" << endl;
    }
    else
    {
        cout << "open " << _filename << " failed" << endl;
    }
    
}