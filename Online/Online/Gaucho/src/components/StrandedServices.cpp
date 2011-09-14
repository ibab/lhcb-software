/*
 * StrandedServices.h
 *
 *  Created on: Sep 9, 2011
 *      Author: beat
 */

#include "StrandedServices.h"
#include "GaudiKernel/SvcFactory.h"
#include "Gaucho/Utilities.h"
#include "CPP/IocSensor.h"

#define CMD_BROWSE 0


DECLARE_SERVICE_FACTORY(StrandedServices)

StrandBrowser::  StrandBrowser(StrandedServices *s) : gService(s)
{

}

void StrandBrowser::handle(const Event& ev)  {
  if(ev.eventtype == IocEvent)  {
      switch(ev.type) {
        case CMD_BROWSE:
        {
          StrandedServices* service = (StrandedServices*)ev.data;
          service->analyze();
        }
        // do something
        break;
        default:
        break;
      }
  }
}

StrandInfo::StrandInfo(std::string name,StrandBrowser *serv):DimInfo(name.c_str(),0,0)
{
  m_name = name;
  m_service = serv;
}

void StrandInfo::infoHandler()
{
  char *rawstate = (char*)getData();
  int datlen = getSize();
  dyn_string *stat=Strsplit(rawstate,"/");
  std::string state = stat->at(0);
//  if (state == "READY")
  if (state == "RUNNING")
  {
    IocSensor::instance().send(m_service, CMD_BROWSE, (void*)m_service->gService);
  }
  delete stat;
}

StrandedServices::StrandedServices(const std::string& name, ISvcLocator* sl):Service(name,sl)
{
  declareProperty("SubFarms",m_subfarms);
  declareProperty("PartitionName",m_partname);
  m_hltstate = 0;
  m_Browser = 0;
}
StrandedServices::~StrandedServices()
{
}
StatusCode StrandedServices::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if ( IIncidentListener::interfaceID().versionMatch(riid) )
  {
    *ppvIF = (IIncidentListener*)this;
  }
  else
  {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode StrandedServices::initialize()
{
  return StatusCode::SUCCESS;
}
StatusCode StrandedServices::stop()
{
  DimClient::setDnsNode("ecs01");
  std::string nam="SMI/LHCB_HLT/LHCB_HLT";
  m_Browser = new StrandBrowser(this);
  m_hltstate = new StrandInfo(nam,m_Browser);
//  analyze();
  Service::stop();
  return StatusCode::SUCCESS;
}
StatusCode StrandedServices::start()
{
  Service::start();
  deletePtr(m_hltstate);
  deletePtr(m_Browser);
  return StatusCode::SUCCESS;
}

int StrandedServices::analyze()
{
  DimClient::setDnsNode("mona08");
  DimBrowser *br;
  std::map<std::string,int> badfarms;
  std::map<std::string,int> badnodes;
  std::map<std::string,int>::iterator it;
  br = new DimBrowser();
  int nserv;
  nserv = br->getServices("MON_LHCb_hlt01_MEPrx_00/*/Data");
  delete br;
  DimClient::setDnsNode("hlt01");
  br = new DimBrowser();
  if (nserv != 0)
  {
    nserv = br->getServices("MON_LHCb_*_MEPrx/*/Data");
    if (nserv != 0)
    {
      int typ;
      typ =1;
      char *serv;
      char *fmt;
      typ = br->getNextService(serv,fmt);
      while (typ != 0)
      {
        dyn_string *a = Strsplit(serv,"_");
        badfarms[a->at(2)] = 1;
        typ =br->getNextService(serv,fmt);
      }
      delete br;
      for (it=badfarms.begin();it!=badfarms.end();it++)
      {
        DimClient::setDnsNode((*it).first.c_str());
        br = new DimBrowser();
        int ns = br->getServices("*MEPRx_01/*/Data*");
        typ = br->getNextService(serv,fmt);
        while (typ != 0)
        {
          dyn_string *a = Strsplit(serv,"_");
          badnodes[a->at(1)] = 1;
          typ =br->getNextService(serv,fmt);
        }
        delete br;
      }
    }
    printf("Nodes still having MEPRx services after stop:\n");
    for (it=badnodes.begin();it!=badnodes.end();it++)
    {
      std::string nod = (*it).first;
      printf("%s\n",nod.c_str());
    }
  }
  return 0;
}
void StrandedServices::handle(const Incident& inc)
{

}
