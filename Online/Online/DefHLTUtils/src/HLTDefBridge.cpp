/*
 * HLTDefBridge.cpp
 *
 *  Created on: Jun 27, 2012
 *      Author: Beat Jost
 */

#include "dic.hxx"
#include "dis.hxx"
#include <string>
#include "ROMon/Utilities.h"
#include "stdio.h"
#include "RTL/rtl.h"
#include "RTL/strdef.h"
class DefHltcommand : public DimCommand
{
public:
  DefHltcommand(const char *name, char *format):  DimCommand(name, format)
  {
  }
  virtual void commandHandler()
  {
    char * input = this->getString();
    ::printf("%s: Got Input %s\n",RTL::processName().c_str(),input);
    dyn_string *MEPlist;
    MEPlist = Strsplit(input,"|");

    size_t i;
    for (i=0;i<MEPlist->size();i++)
    {
      if (MEPlist->at(i).size() == 0) break;
      dyn_string *MEP = Strsplit(MEPlist->at(i).c_str()," ");
      int val;
      if (MEP->size()>1)
      {
        ::sscanf(MEP->at(1).c_str(),"%d",&val);
        DimClient::sendCommandNB(MEP->at(0).c_str(),val);
      }
      delete (MEP);
    }
    delete (MEPlist);
  }
};

int main(int , char **)
{
  std::string myHost = RTL::str_lower(RTL::nodeNameShort());
  DimClient::setDnsNode(myHost.c_str());
  DimServer::setDnsNode("ecs03");
  DimServer::autoStartOn();
  DimServer::start((myHost+"_HLTDefBridge").c_str());
  DefHltcommand m_DefHltcommand((myHost+"_HLTDefBridge/EnDisCommand").c_str(),(char*)"C");
  while (1)
  {
    sleep (60);
  }
  return 0;
}
