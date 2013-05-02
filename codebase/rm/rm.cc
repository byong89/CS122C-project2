
#include "rm.h"

RM* RM::_rm = 0;

RM* RM::Instance()
{
    if(!_rm)
        _rm = new RM();

    return _rm;
}

RM::RM()
{
	pageManager = PF_Manager::Instance();
}

RM::~RM()
{
}

RC RM::createTable(const string tableName, const vector<Attribute> &attrs){

}
