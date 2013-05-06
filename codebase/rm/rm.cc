#include "rm.h"
#include "../pf/pf.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>

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
	if(pageManager->OpenFile("tables", tablesFileHandler) == -1){
		cout<<"Tables File not found\n";
		cout<<"Creating new Tables file\n";
		pageManager->CreateFile("tables");
		pageManager->OpenFile("tables", tablesFileHandler);
		vector<Attribute> v;
		createTable("tables", v);
	}

	if(pageManager->OpenFile("columns", columnsFileHandler) == -1){
		cout<<"Columns File not found\n";
		cout<<"Creating new Columns file\n";
		pageManager->CreateFile("columns");
		pageManager->OpenFile("columns", columnsFileHandler);
	}
}

RM::~RM()
{
	pageManager->CloseFile(tablesFileHandler);
	pageManager->CloseFile(columnsFileHandler);
	pageManager->CloseFile(fileHandler);
}

RC RM::createTable(const string tableName, const vector<Attribute> &attrs){
	//Page format: n/id/offset/length. At the end of page, write record.
	pageManager->CreateFile(tableName.data());

	int numRecord = 0;
	int id = 0;
	int length = 0;
	int offset = 0;
	int recordOffset = 4+(8*numRecord); //4bits for the number of records + 8 for each offset & length.

	//Read the table table
	void *buffer = malloc(PF_PAGE_SIZE);
	tablesFileHandler.ReadPage(0, buffer);

	//Read the number of records in the file
	memcpy(&numRecord, buffer+4, 4);
	if(numRecord <= 0){
		numRecord = 1;
		id = 1;
		length = 4+4+4;
		offset = PF_PAGE_SIZE - length;

		memcpy(buffer, &numRecord, sizeof(numRecord));
		memcpy(buffer+sizeof(numRecord), &id, sizeof(id));
		memcpy(buffer+sizeof(numRecord)+sizeof(id), &offset, sizeof(offset));
		memcpy(buffer+sizeof(numRecord)+sizeof(id)+sizeof(offset), &length, sizeof(length));

		//Insert the record at the end of the file
		string fileName = tableName;
		memcpy(buffer+PF_PAGE_SIZE-length, &id, 4); //Go to the end of file and locate the corresponding place
		memcpy(buffer+PF_PAGE_SIZE-length+4, tableName.data(), sizeof(tableName.data()));
		memcpy(buffer+PF_PAGE_SIZE-length+8, tableName.data(), sizeof(tableName.data()));
		tablesFileHandler.WritePage(0, buffer);

	}
	else {
		//Read N;
		id = numRecord;
		//Read the last inserted offset
		recordOffset = (numRecord*(sizeof(id)+sizeof(offset)+sizeof(length)))+4;
		memcpy(&offset, buffer+recordOffset-sizeof(offset)-sizeof(length), 4);
		memcpy(&length, buffer+recordOffset-sizeof(length), 4);

		memcpy(buffer+recordOffset, &id, sizeof(id));
		memcpy(buffer+recordOffset+4,&offset, sizeof(offset));
		memcpy(buffer+recordOffset+8, &length, sizeof(length));

		numRecord++;
		memcpy(buffer, &numRecord, sizeof(numRecord));
		tablesFileHandler.WritePage(0, buffer);

	}

	//Insert attributes to the columns table
	columnsFileHandler.ReadPage(0, buffer);
	int columnsNumRecords = 0;
	int columnOffset = 0;
	memcpy(buffer+4, &columnsNumRecords, 4);
	columnOffset = (columnsNumRecords*12)+4;
	/*for(int i = 0; i<attrs.size(); i++){
		RID rid;
		rid.pageNum = 0;
		rid.slotNum = id;
		void *data;
		int size = 0;

		switch(attrs[i].type) {
		case 0:
		case 1:
			size = 4;
			break;
		case 2:
			size = 4;
		}
		memcpy(data, &size, 4);
		memcpy(data+4, &attrs[i].name)
		inserTuple(tableName, attrs[i].)
	}*/

    //Add new columns file to column File
	return 0;
}

RC RM::insertTuple(const string tableName, const void *data, RID &rid){
	PF_FileHandle fh;
	int numRecord = 0;
	int id = rid.slotNum;
	int scanId = 0;
	int offset = 0;
	int length = sizeof(data);
	int previousOffset = 0;
	int previousLength = 0;
	int scan = sizeof(id)+sizeof(offset)+sizeof(length);
	void *buffer = malloc(PF_PAGE_SIZE);
	tablesFileHandler.ReadPage(0, buffer);

	//Scan for the table filename
	memcpy(&numRecord, buffer+4, 4);
	int i = 0;
	while(scanId != id && i <= numRecord){
		memcpy(&scanId, buffer+(scan*i)+4, sizeof(id));
		if(scanId == id){
			memcpy(&offset, buffer+(scan*i)+4+sizeof(id), sizeof(offset));
		}
	}
	string tName;
	memcpy(&tName, buffer+PF_PAGE_SIZE-offset+sizeof(id)+4, 4);

	pageManager->OpenFile(tName.data(), fh);

	fh.ReadPage(rid.pageNum, buffer);
	memcpy(&numRecord, buffer+4, 4);
	memcpy(&previousOffset, buffer+((numRecord*scan)+4)-sizeof(offset)-sizeof(length), 4);
	memcpy(&previousLength, buffer+((numRecord*scan)+4)-sizeof(length), 4);
	offset = previousOffset-length;
	memcpy(buffer+((numRecord*scan)+4), &id, 4 );
	memcpy(buffer+((numRecord*scan)+4+sizeof(id)), &offset, sizeof(offset));
	memcpy(buffer+((numRecord*scan)+4+sizeof(id)+sizeof(offset)), &length ,sizeof(length));

	memcpy(buffer+offset-length, &id, sizeof(id)); //Go to the end of file and locate the corresponding place
	memcpy(buffer+offset-length+4, tableName.data(), sizeof(tableName.data()));
	memcpy(buffer+offset-length+8, tableName.data(), sizeof(tableName.data()));
	tablesFileHandler.WritePage(rid.pageNum, data);
	return 0;
}

RC RM::deleteTable(const string tableName){
	pageManager->DestroyFile(tableName.data());
	pageManager->CreateFile(tableName.data());


	return 0;
}

RC RM::getAttributes(const string tableName, vector<Attribute> &attrs){
	PF_FileHandle fh;
	int numRecord = 0;
	int id = 0;
	int scanId = 0;
	int offset = 0;
	int length = 0;
	int previousOffset = 0;
	int previousLength = 0;
	int scan = sizeof(id)+sizeof(offset)+sizeof(length);
	void *buffer = malloc(PF_PAGE_SIZE);
	tablesFileHandler.ReadPage(0, buffer);

	//Scan for the table filename
	memcpy(&numRecord, buffer+4, 4);
	int i = 0;
	while(scanId != id && i <= numRecord){
		memcpy(&scanId, buffer+(scan*i)+4, sizeof(id));
		if(scanId == id){
			memcpy(&offset, buffer+(scan*i)+4+sizeof(id), sizeof(offset));
		}
		i++;
	}
	string tName;
	memcpy(&tName, buffer+PF_PAGE_SIZE-offset+sizeof(id)+4, 4);
	return 0;
}

RC RM:: deleteTuple(const string tableName, const RID &rid){
	PF_FileHandle fh;
	int numRecord = 0;
	int id = rid.slotNum;
	int scanId = 0;
	int offset = 0;
	int length = 0;
	int previousOffset = 0;
	int previousLength = 0;
	int scan = sizeof(id)+sizeof(offset)+sizeof(length);
	void *buffer = malloc(PF_PAGE_SIZE);
	tablesFileHandler.ReadPage(0, buffer);

	//Scan for the table filename
	memcpy(&numRecord, buffer+4, 4);
	int i = 0;
	while(scanId != id && i <= numRecord){
		memcpy(&scanId, buffer+(scan*i)+4, sizeof(id));
		if(scanId == id){
			memcpy(&offset, buffer+(scan*i)+4+sizeof(id), sizeof(offset));
		}
		i++;
	}
	string tName;
	memcpy(&tName, buffer+PF_PAGE_SIZE-offset+sizeof(id)+4, 4);
	int delInt = 0;
	void* del;
	pageManager->OpenFile(tableName.data(), fh);
	fh.ReadPage(rid.pageNum, buffer);
	memcpy(&numRecord, buffer+4, 4);
	memcpy(&previousOffset, buffer+((numRecord*scan)+4)-sizeof(offset)-sizeof(length), 4);
	memcpy(&previousLength, buffer+((numRecord*scan)+4)-sizeof(length), 4);
	offset = previousOffset-length;
	memcpy(buffer+((numRecord*scan)+4), &del, 4 );
	memcpy(buffer+((numRecord*scan)+4+sizeof(id)), &delInt, sizeof(offset));
	memcpy(buffer+((numRecord*scan)+4+sizeof(id)+sizeof(offset)), &delInt ,sizeof(length));
	memcpy (buffer+offset, &del, sizeof(del));
	return 0;
}

RC RM::deleteTuples(const string tableName){
	pageManager->DestroyFile(tableName.data());
	pageManager->CreateFile(tableName.data());
	return 0;
}

RC RM::updateTuple(const string tableName, const void *data, const RID &rid){
	PF_FileHandle fh;
	int id = rid.slotNum;
	int pageNum = rid.pageNum;
	int offset = 0;
	int length = sizeof(data);
	int numRecord = 0;
	int scanId = 0;
	int scan = sizeof(id)+sizeof(offset)+sizeof(length);
	void *buffer = malloc(PF_PAGE_SIZE);
	tablesFileHandler.ReadPage(0,buffer);

	memcpy(&numRecord, buffer+sizeof(numRecord), sizeof(numRecord));
	int i = 0;
	while(scanId != id && i <= numRecord){
		memcpy(&scanId, buffer+(scan*i)+4, sizeof(id));
		if(scanId == id){
			memcpy(&offset, buffer+(scan*i)+4+sizeof(id), sizeof(offset));
		}
		i++;
	}
	string tName;
	memcpy(&tName, buffer+PF_PAGE_SIZE-offset+sizeof(id)+4, 4);
	int delInt = 0;
	pageManager->OpenFile(tableName.data(), fh);
	fh.ReadPage(rid.pageNum, buffer);

	scanId = 0;
	i=0;
	memcpy(&numRecord, buffer+sizeof(numRecord), sizeof(numRecord));
	while(scanId != id && i <= numRecord){
		memcpy(&scanId, buffer+(scan*i)+4, sizeof(id));
		if(scanId == id){
			memcpy(&offset, buffer+(scan*i)+4+sizeof(id), sizeof(offset));
			memcpy(buffer+offset, data, length);
			return 0;
		}
		i++;
	}
	return -1;

}
