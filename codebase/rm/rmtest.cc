
#include <fstream>
#include <iostream>
#include <cassert>

#include "rm.h"

using namespace std;

void rmTest()
{
   RM *rm = RM::Instance();

  // write your own testing cases here
   vector<Attribute> v;
   rm->createTable("Hello.txt", v);
}

int main()
{
  cout << "test..." << endl;

  rmTest();
  // other tests go here

  cout << "OK" << endl;
}
