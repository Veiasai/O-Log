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
}