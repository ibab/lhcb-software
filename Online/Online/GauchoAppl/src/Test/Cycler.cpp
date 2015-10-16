/*
 * Cycler.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: beat
 */
#include "dic.hxx"
#include <string>
class myInfo : public DimInfo
{
public:
  std::string sName;
  myInfo(const char * nam, const std::string &snam, const char *nlink = "DEAD") : DimInfo(nam,(char*)nlink)
  {
    sName = snam;
  }
virtual void infoHandler()
{
  char *state;
  state  = getString();
  getSize();
  std::string stat = std::string(state);
  sleep(5);
  if (stat == "READY")
  {
    DimClient::sendCommand(sName.c_str(),"start");
  }
  else if (stat == "RUNNING")
  {
    DimClient::sendCommand(sName.c_str(),"stop");
  }
}
};
int main(int , char *argv[]) // Taskname, DNS Node
{
  char *DNS = argv[2];
  char *tsk = argv[1];
  DimClient::setDnsNode(DNS);
  std::string rserv = tsk;
  rserv+="/status";
  myInfo info((char*)rserv.c_str(),tsk);
  sleep(100000000);
  return 0;
}
