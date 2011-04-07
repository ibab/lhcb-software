#include "Gaucho/MonAdder.h"
#include "Gaucho/MonTypes.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/AdderSys.h"
#include "Gaucho/AddSerializer.h"
#include "Gaucho/ObjRPC.h"

static int mpty;
typedef std::pair<std::string, MonObj*> MonPair;
extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, MonInfo *h)
  {
    MonAdder *adder = (MonAdder*)tis;
    adder->add(buff, siz,h);
  }
}
MonAdder::MonAdder()
{
  m_received    = 0;
  CycleFn       = 0;
  m_buffersize  = 0;
  m_buffer      = 0;
  m_usedSize    = 0;
  m_reference   = -1;
  m_expandRate  = false;
  m_lockid      = 0;
  m_maplock     = 0;
  m_IsEOR       = false;
  m_added       = 0;
  m_noRPC       = false;
  m_outservice  = 0;
  m_RPCser      = 0;
  m_rpc         = 0;
  m_ser         = 0;
  m_RPCser      = 0;
}

MonAdder::~MonAdder()
{
  for (TaskServIter i = m_TaskServiceMap.begin();i!= m_TaskServiceMap.end();i++)
  {
    delete i->second->m_diminfo;
    delete i->second;
  }
  m_TaskServiceMap.clear();
  for (INServIter i = m_inputServicemap.begin();i!= m_inputServicemap.end();i++)
  {
    delete i->second->m_Info;
    delete i->second;
  }
  m_inputServicemap.clear();
  deletePtr(m_outservice);
  deletePtr(m_ser);
  deletePtr(m_RPCser);
  deletePtr(m_rpc);
}

void MonAdder::Configure()
{
  std::string nodename = RTL::nodeNameShort();
  if (nodename != "")
  {
    toLowerCase(nodename);
    m_name= "MON_" + m_MyName;
  }
  m_serviceexp = boost::regex(m_servicePattern.c_str(),boost::regex_constants::icase);
  m_taskexp = boost::regex(m_taskPattern.c_str(),boost::regex_constants::icase);
  m_outsvcname = m_name+m_serviceName;
  lib_rtl_create_lock(0,&m_maplock);
  m_ser = new AddSerializer((ObjMap*)&m_hmap);
  m_rpc = 0;
  if (!m_noRPC)
  {
    std::string nam;
    if ( m_type == ADD_HISTO )
    {
      nam = m_name + "/Histos/HistCommand";
    }
    else if ( m_type == ADD_COUNTER )
    {
      nam = m_name+"/Counter/HistCommand";
    }
    m_RPCser = new AddSerializer((ObjMap*)&m_hmap);
    m_rpc = new ObjRPC(m_RPCser,(char*)nam.c_str(), (char*)"I:1;C",(char*)"C", this->m_maplock, 0/*this->m_lockid*/);
  }
}

void *MonAdder::Allocate(int siz)
{
  if (m_buffersize<siz)
  {
    if (m_buffer != 0)
    {
      free (m_buffer);
    }
    m_buffer = malloc(siz);
    if (m_buffer != 0)
    {
      memset(m_buffer,0,siz);
      m_buffersize = siz;
    }
  }
  return m_buffer;
}
void *MonAdder::ReAllocate(int incsiz)
{
  void *newbuffer;
  int newsiz = m_usedSize+incsiz;
  if (m_buffersize<newsiz)
  {
    newbuffer = realloc(m_buffer,newsiz);
    if (newbuffer != 0)
    {
      if (newbuffer != m_buffer)
      {
        m_hmap.clear();
        void *bend = AddPtr(newbuffer,m_usedSize);
        void *hstart = AddPtr(newbuffer,sizeof(SerialHeader));
        DimBuffBase *pp = (DimBuffBase*)hstart;
        while (pp<bend)
        {
          char *nam = (char*)AddPtr(pp,pp->nameoff);
          m_hmap.insert(MonPair(std::string(nam),(MonObj*)pp));
          pp=(DimBuffBase*)AddPtr(pp,pp->reclen);
        }
        m_buffer = newbuffer;
      }
      m_buffersize = newsiz;
    }
    else
    {
      free(m_buffer);
      m_buffersize = 0;
    }
  }
  return m_buffer;
}

void MonAdder::TaskName(std::string &server, std::string &tname, std::string &tgen)
{
  bool status = boost::regex_search(server,m_taskexp);
  if (status)
  {
    tname = server;
    tgen = "";
    return;
  }
  tname = "";
  tgen = "";
}

INServiceDescr *MonAdder::findINService(std::string servc)
{
  INServIter i = m_inputServicemap.find(servc);
  if (i != m_inputServicemap.end())
  {
    INServiceDescr *sd = i->second;
    return sd;
  }
  return 0;
}

OUTServiceDescr *MonAdder::findOUTService(std::string servc)
{
  OUTServIter i = m_outputServicemap.find(servc);
  if (i != m_outputServicemap.end())
  {
    OUTServiceDescr *sd = i->second;
    return sd;
  }
  return 0;
}
void MonAdder::TaskDied(std::string & task)
{
  TskServiceMap::iterator i = m_TaskMap.find(task);
  if (i != m_TaskMap.end())
  {
    const std::string& service = i->second;
    INServIter k = m_inputServicemap.find(service);
    m_TaskMap.erase(i);
    if (k != m_inputServicemap.end())
    {
      INServiceDescr *sd = k->second;
      delete sd->m_Info;
      delete sd;
      m_inputServicemap.erase(k);
    }
    if (m_inputServicemap.size() == 0)
    {
//      printf ("Last Input Service... Deleting output service %s\n",m_outsvcname.c_str());
      delete m_outservice;
      m_outservice = 0;
    }
  }
}
void MonAdder::NewService(DimInfo *, std::string &TaskName, std::string &ServiceName)
{
  int status;
  status = boost::regex_search(ServiceName, m_serviceexp);
  if (status)
  {
//    printf("Comparing %s with Pattern %s...",ServiceName.c_str(),this->m_servicePattern.c_str());
//    printf("SUCCESS\n");
    INServiceMap::iterator i;
    i = m_inputServicemap.find(ServiceName);
    if (i == m_inputServicemap.end())
    {
      INServiceDescr *sd = new INServiceDescr((char*) (ServiceName.c_str()),
          new MonInfo((char*) (ServiceName.c_str()), &BufferAdder, this),TaskName);
      //            printf("Service: %s\n",service->at(0).c_str());
      m_inputServicemap.insert(std::make_pair(ServiceName, sd));
      TskServiceMap::iterator k =m_TaskMap.find(TaskName);
      if (k == m_TaskMap.end())
      {
        m_TaskMap.insert(std::make_pair(TaskName,ServiceName));
      }
      else
      {
//        printf ("Task %s already exists in map to be added for service %s",TaskName.c_str(),ServiceName.c_str());
      }
    }
    //if (m_inputServicemap.size() == 1)
    if ( 0 == m_outservice )
    {
//      printf ("First client for adding... Creating our output service...%s\n",m_outsvcname.c_str());
      m_outservice = new ObjService(m_ser,m_outsvcname.c_str(),(char*) "C", (void*) &mpty, 4, &m_buffer, &m_usedSize);
    }
    DimServer::start();
  }
}
void MonAdder::RemovedService(DimInfo *, std::string &, std::string &ServiceName)
{
  int status;
//  printf("Removed Service %s My pattern %s\n",ServiceName.c_str(),this->m_servicePattern.c_str());
  status = boost::regex_search(ServiceName,m_serviceexp);
  INServiceMap::iterator i;
  i = m_inputServicemap.find(ServiceName);
  if (i != m_inputServicemap.end())
  {
    INServiceDescr *sd = i->second;
    delete sd->m_Info;
    TskServiceMap::iterator k =m_TaskMap.find(sd->task);
    if (k != m_TaskMap.end())
    {
      m_TaskMap.erase(k);
    }
    delete sd;
    m_inputServicemap.erase(i);
  }
  if (m_inputServicemap.size() == 0)
  {
    deletePtr(m_outservice);
  }
}

void MonAdder::dumpServices()
{
  INServiceMap::iterator i;
  for (i=m_inputServicemap.begin();i!=m_inputServicemap.end();i++)
  {
    printf("Service Name:%s\n",i->first.c_str());
  }
}

unsigned long long MonAdder::gettime()
{
#define onesec_nano (unsigned long long)(1000000000)
  unsigned long long timstamp;
  struct timeval tv;
  struct timezone *tz;
  tz = 0;
  gettimeofday(&tv, tz);
  timstamp  = tv.tv_sec;
  timstamp *= onesec_nano;
  timstamp += tv.tv_usec*1000;
  return timstamp;
}
