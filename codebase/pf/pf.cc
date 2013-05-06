#include "pf.h"
#include "stdio.h"

PF_Manager* PF_Manager::_pf_manager = 0;


PF_Manager* PF_Manager::Instance()
{
    if(!_pf_manager)
        _pf_manager = new PF_Manager();
    
    return _pf_manager;    
}

//Constructor
PF_Manager::PF_Manager()
{
}

//Destructor
PF_Manager::~PF_Manager()
{
}

    
RC PF_Manager::CreateFile(const char *fileName)
{

	if(FILE* myFile = fopen(fileName, "w")) {
		fclose(myFile);
		return 0;
	}
    return -1;
}


RC PF_Manager::DestroyFile(const char *fileName)
{
	remove(fileName);
	return 0;
}


RC PF_Manager::OpenFile(const char *fileName, PF_FileHandle &fileHandle)
{
	FILE* tmp = fopen(fileName, "r+");
	if(fileHandle.setFile(tmp) == 0 && tmp != NULL){
		//fileHandle.closeFile();
		return 0;
	}
    return -1;
}


RC PF_Manager::CloseFile(PF_FileHandle &fileHandle)
{
    return fileHandle.closeFile();
}


PF_FileHandle::PF_FileHandle()
{
	this->myFile = NULL;
	this->pageCount = 0;
}
 

PF_FileHandle::~PF_FileHandle()
{
}


RC PF_FileHandle::ReadPage(PageNum pageNum, void *data)
{
	fseek(this->myFile, PF_PAGE_SIZE * pageNum, SEEK_SET);
	for(unsigned i = 0; i < PF_PAGE_SIZE; i++){
        *((char *)data+i) = fgetc(this->myFile);
    }
	fseek(this->myFile, 0, SEEK_END);
    return 0;
}


RC PF_FileHandle::WritePage(PageNum pageNum, const void *data)
{
	if (pageNum > pageCount) {
		AppendPage(data);
	}
	fseek(this->myFile, PF_PAGE_SIZE * pageNum, SEEK_SET);
	fwrite(data, 1, PF_PAGE_SIZE, this->myFile);
	fseek(this->myFile, 0, SEEK_END);

    return 0;
}


RC PF_FileHandle::AppendPage(const void *data)
{
	WritePage(this->pageCount, data);
	this->pageCount++;

    return 0;
}


unsigned PF_FileHandle::GetNumberOfPages()
{
    return this->pageCount;
}

RC PF_FileHandle::setFile(FILE* &myFile){
	if(this->myFile != NULL){
		return -1;
	}
	this->myFile = myFile;
	return 0;
}

RC PF_FileHandle::closeFile(){
	if(fclose(this->myFile) == 0) {
		this->myFile = NULL;
		return 0;
	}
	return -1;
}

