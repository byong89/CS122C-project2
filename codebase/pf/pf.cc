#include "pf.h"

PF_Manager* PF_Manager::_pf_manager = 0;


PF_Manager* PF_Manager::Instance(int cacheNumPages)
{
    if(!_pf_manager)
        _pf_manager = new PF_Manager(cacheNumPages);

    return _pf_manager;
}


PF_Manager::PF_Manager(int cacheNumPages)
{
}


PF_Manager::~PF_Manager()
{
}


RC PF_Manager::CreateFile(const char *fileName)
{
    return -1;
}


RC PF_Manager::DestroyFile(const char *fileName)
{
    return -1;
}


RC PF_Manager::OpenFile(const char *fileName, PF_FileHandle &fileHandle)
{
    return -1;
}


RC PF_Manager::CloseFile(PF_FileHandle &fileHandle)
{
    return -1;
}


PF_FileHandle::PF_FileHandle()
{
}


PF_FileHandle::~PF_FileHandle()
{
}


RC PF_FileHandle::ReadPage(PageNum pageNum, void *data)
{
    return -1;
}


RC PF_FileHandle::WritePage(PageNum pageNum, const void *data)
{
    return -1;
}


RC PF_FileHandle::AppendPage(const void *data)
{
    return -1;
}


unsigned PF_FileHandle::GetNumberOfPages()
{
    return -1;
}


