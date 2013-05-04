
#include "rm.h"
#include <iostream>

RM* RM::_rm = 0;

RM* RM::Instance()
{
    if(!_rm)
        _rm = new RM();

    return _rm;
}

RM::RM()
{
	pageManager = PF_Manager::Instance(); //Create an instance of the page manager

	//Try opening the catalog. If the catalog does not exists, create one and add tables.
	if(pageManager->OpenFile("catalog.txt", fileHandle) == -1){
		cout<<"Catalog File not found\n";
		cout<<"Creating new Catalog file\n";
		pageManager->CreateFile("catalog.txt");
		pageManager->OpenFile("catalog.txt", fileHandle);
		createTablesTable();
	}

}

RM::~RM()
{
	pageManager->CloseFile(fileHandle);
}

RC RM::createTable(const string tableName, const vector<Attribute> &attrs){
	fileHandle.AppendPage(tableName.data());
}

void RM::createTablesTable(){
	RID tablesRecordID;
	tablesRecordID.pageNum = 0;

	vector<Attribute> v;
	AttrType type;
	Attribute attr;
	attr.name = "tables";
	attr.length = 48;
	attr.type = type;

	v.push_back(attr);
	fileHandle.WritePage(tablesRecordID.pageNum, "tables");
}

void RM::createColumnsTable(){

}
