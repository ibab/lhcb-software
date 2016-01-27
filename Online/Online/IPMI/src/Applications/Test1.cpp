#include "Applib/IPMINode.h"
#include <string>
#include <thread>
#include "stdio.h"
using namespace std;
extern "C"
{
  int verbose;
  int csv_output;
}
int main(int argc, char **argv)
{
  int status;
  string uname("root");
  string pword("lhcbipmi");
  string nnod1("hlta0101-ipmi");
  string nnod2("hlta0906-ipmi");
//  char aaa[255];
//  sprintf(aaa,"%d",500);
//  _dl_lookup_symbol("getaddrinfo");
  IPMINode n1(nnod1,uname,pword);
  status = n1.setup();
  IPMINode n2(nnod2,uname,pword);
  status = n2.setup();
  status = n1.getPowerState();
  status = n2.getPowerState();
  status = n1.getPowerState();
  n1.shutdown();
  n2.shutdown();
  return 0;
}
