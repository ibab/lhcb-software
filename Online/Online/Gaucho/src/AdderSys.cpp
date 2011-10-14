/*
 * AdderSys.cpp
 *
 *  Created on: Feb 28, 2011
 *      Author: beat
 */
#include "Gaucho/AdderSys.h"
#include "Gaucho/InfoHandlers.h"
  AdderSys::AdderSys()
{
  this->gg_TaskHandler = new HAdderTaskInfoHandler();
  //    this->gg_ServHandler = new HAdderServInfoHandler();
  this->gg_ServHandler = 0;
  gg_DNSInfo = 0;
}
AdderSys::~AdderSys()
{

}
void AdderSys::Add(MonAdder *a)
{
  Adderlist_t::iterator i;
  bool fnd = false;
  for (i = gg_AdderList.begin(); i != gg_AdderList.end(); i++)
  {
    if (*i == a)
    {
      fnd = true;
      break;
    }
  }
  if (!fnd)
  {
    gg_AdderList.push_back(a);
  }
}
void AdderSys::Remove(MonAdder *a)
{
  Adderlist_t::iterator i;
  bool fnd = true;
  while (fnd)
  {
    fnd = false;
    for (i = gg_AdderList.begin(); i != gg_AdderList.end(); i++)
    {
      if (*i == a)
      {
        fnd = true;
        break;
      }
    }
    if (fnd)
    {
      gg_AdderList.erase(i);
    }
  }
}
AdderSys &AdderSys::Instance()
{
  static AdderSys s;
  return s;
}
void AdderSys::start()
{
  if (gg_DNSInfo == 0)
    gg_DNSInfo = new DimInfo((char*) "DIS_DNS/SERVER_LIST", (char*) "DEAD",
        gg_TaskHandler);
}
void AdderSys::stop()
{
  if (gg_DNSInfo != 0)
  {
    delete gg_DNSInfo;
    gg_DNSInfo = 0;
  }
}
