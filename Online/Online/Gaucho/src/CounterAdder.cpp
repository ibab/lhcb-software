#include "Gaucho/CounterAdder.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonServer.h"
#include "Gaucho/ObjRPC.h"
#include "Gaucho/RootHists.h"
#include "Gaucho/RateService.h"
#include "dis.hxx"
#include "TDirectory.h"
#include <vector>
#include "string.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/AdderSys.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "AIDA/IHistogram1D.h"

typedef std::pair<std::string, void*> MonPair;

extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, MonInfo *h);
}

CounterAdder::CounterAdder(char *myName, char *serviceName)
{
  std::string svc_prefix("/Counter/");
  m_buffersize = 0;
  m_buffer = 0;
  m_usedSize = 0;
  m_reference =-1;
  m_serviceName = svc_prefix +std::string(serviceName);
  m_expandRate = false;
  m_srcnode = "";
  m_outdns = "localhost";
  m_MyName = myName;
  m_lockid = 0;
  m_maplock = 0;
  m_IsEOR = false;
  m_added = 0;
  m_noRPC = false;
  m_ser = 0;
  m_RPCser = 0;
  AdderSys::Instance().gg_AdderList.push_back(this);
  m_type = ADD_COUNTER;
}

CounterAdder::~CounterAdder()
{
  for (TaskServIter i = m_TaskServiceMap.begin();i!= m_TaskServiceMap.end();i++)
  {
    delete (*i).second->m_diminfo;
    delete (*i).second;
  }
  m_TaskServiceMap.clear();
  for (INServIter i = m_inputServicemap.begin();i!= m_inputServicemap.end();i++)
  {
    delete (*i).second->m_Info;
    delete (*i).second;
  }
  m_inputServicemap.clear();
  deletePtr(m_outservice);
  deletePtr(m_ser);
  deletePtr(m_RPCser);
  deletePtr(m_rpc);
}

void CounterAdder::Configure()
{
  std::string nodename = RTL::nodeNameShort();
  if (nodename != "")
  {
    toLowerCase(nodename);
    m_name= "MON_" + m_MyName;
  }
  if (m_srcnode == "")
  {
    if (nodename.find("hlt"))
    {
      std::string rackrow = nodename.substr(3,1); //size of "hlt"
      std::string rack_node = nodename.substr(4);
      if (rack_node.size() == 4)
      {
        m_srcnode = nodename;
      }
      else if (rack_node.size() == 2 )
      {
        m_srcnode = nodename;
      }
      else
      {
        m_srcnode = nodename.substr(0,3);
      }
    }
  }
  m_serviceexp = boost::regex(m_servicePattern.c_str(),boost::regex_constants::icase);
  m_taskexp = boost::regex(m_taskPattern.c_str(),boost::regex_constants::icase);
  m_outsvcname = m_name+m_serviceName;
  lib_rtl_create_lock(0,&m_maplock);
  m_ser = new AddSerializer((ObjMap*)&m_hmap);
  m_rpc = 0;
  if (!m_noRPC)
  {
    std::string nam = m_name+"/Counter/HistCommand";
    m_RPCser = new AddSerializer((ObjMap*)&m_hmap);
    m_rpc = new ObjRPC(m_RPCser,(char*)nam.c_str(), (char*)"I:1;C",(char*)"C", this->m_maplock, 0/*this->m_lockid*/);
  }
}

void CounterAdder::add(void *buff, int siz, MonInfo *h)
{
  void *p;
  unsigned long long tim = gettime();
  if (siz == 4)
  {
    printf("No Link from %s. Update counts....\n",h->m_TargetService.c_str());
    m_received++;
    return;
  }
  if (((SerialHeader*)buff)->m_magic != SERIAL_MAGIC)
  {
    printf("========> [ERROR] Serial Magic Word Missing  from connection %s\n",h->m_TargetService.c_str());
    m_received++;
    return;
  }
  m_expected = m_inputServicemap.size();
  SerialHeader* header = ((SerialHeader*)buff);
  long long cltime     = header->ser_tim;
  long long runno      = header->run_number;
  long long current;
//  printf("Received size %d Header size %d from %s\n",siz,header->buffersize,h->getName());
  if (m_IsEOR)
  {
    current = runno;
  }
  else
  {
    current = cltime;
  }
//  printf("%s Last update %lld Client Time %lld run number %lld %d %d\n",h->m_TargetService.c_str(),m_reference,cltime,runno,m_received,m_expected);
//  m_RateBuff = 0;
  if (m_reference < current)
  {
    m_time0 = tim;
    m_added++;
    m_received = 1;
    if (m_isSaver)
    {
      if (m_received == 1)
      {
        Lock();
      }
    }
    //printf ("New cycle %s... %d\n",h->m_TargetService.c_str(),m_received);
    p = Allocate(siz);
    m_reference = current;
    memset(m_buffer,0,m_buffersize);
    m_hmap.clear();
    memcpy(p,buff,siz);
    m_usedSize = siz;
    void *bend = AddPtr(p,siz);
    void *hstart = AddPtr(p,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
//      printf("Histogram Name: %s\n",nam);
      std::string nams =nam;
      LockMap();
      m_hmap.insert(MonPair(nams,pp));
      UnLockMap();
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
  }
  else if (m_reference == current)
  {
//    printf("Adding %s\n",h->m_TargetService.c_str());
    if (m_histo != 0)
    {
      this->m_monsvc->Lock();
      unsigned long long dtim = tim-m_time0;
      double ftim = dtim/1000000000;
      m_histo->fill(ftim);
      this->m_monsvc->UnLock();
    }
    MonMap hmap;
    hmap.clear();
    void *bend = AddPtr(buff,siz);
    void *hstart = AddPtr(buff,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      hmap.insert(MonPair(std::string(nam),pp));
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
    for (MonIter i=hmap.begin();i!=hmap.end();i++)
    {
      MonIter j = m_hmap.find(i->first);
      if (j!=m_hmap.end())
      {
        LockMap();
        double *ps, *ph;
        DimHistbuff1 *sumh = (DimHistbuff1*)(j->second);
        DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
        ps = (double*)AddPtr(sumh,sumh->dataoff);
        ph = (double*)AddPtr(srch,srch->dataoff);
        switch(srch->type)
        {
        case C_INT:
          {
            *(long long *)ps += *(long long*)ph;
            break;
          }
        case C_LONGLONG:
          {
            *(long long*)ps += *(long long*)ph;
            break;
          }
        case C_FLOAT:
          {
            *(float*)ps += *(float*)ph;
            break;
          }
        case C_DOUBLE:
          {
            *(double*)ps += *(double*)ph;
            break;
          }
        default:
          {
            break;
          }
        }
        UnLockMap();
      }
      else
      {
//        printf("New Histogram... re-allocating...\n");
        DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
        int csiz = m_usedSize;
        int hsiz = srch->reclen;
        void *p;
        LockMap();
        p = ReAllocate(hsiz);
        if (p!=0)
        {
          p = AddPtr(p,csiz);
          memcpy(p,srch,hsiz);
          m_usedSize += hsiz;
          char *nam = (char*)AddPtr(srch,srch->nameoff);
          m_hmap.insert(MonPair(std::string(nam),p));
        }
        UnLockMap();
      }
    }
    m_received++;
    m_added++;
    //printf ("New Source %s... %d\n",h->m_TargetService.c_str(),m_received);
  }
  else
  {
    //    printf("late update from %s\n expected %lli received %lli\n",h->m_TargetService.c_str(),m_reference,current);
    m_received++;
  }
  if (m_received >= m_expected)
  {
    //    printf("Finished one cycle. Updating our service... %d %d\n", m_received,m_expected);
    if (m_isSaver)
    {
      UnLock();
    }
    m_received = 0;
    //printf(" %d %d\n", m_received,m_expected);
    if (m_outservice != 0)
    {
      m_outservice->Serialize();
      m_outservice->Update();
    }
    fflush(stdout);
    if (CycleFn!= 0)
    {
      (*CycleFn)(CycleCBarg, m_buffer, m_buffersize, &m_hmap, this);
    }
  }
}
