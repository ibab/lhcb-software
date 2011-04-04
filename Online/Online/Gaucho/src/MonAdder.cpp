#include "Gaucho/MonAdder.h"
//#include "hist_types.h"
//#include "HistService.h"
//#include "HistServer.h"
//#include "HistRPC.h"
#include "Gaucho/MonTypes.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/AdderSys.h"
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
  tname = "";
  tgen = "";
  bool status;
  status = boost::regex_search(server,m_taskexp);
  if (status)
  {
//    printf("Comparing %s with Pattern %s...",server.c_str(),this->m_taskPattern.c_str());
    tname = server;
//    MonMap::iterator it;
//    it = m_TaskMap.find(server);
//    if (it == m_TaskMap.end())
//    {
//      m_TaskMap.insert(std::pair<std::string,void*>(server,new INServiceMap()));
//    }
    tgen = "";
//    printf ("SUCCESS\n");
  }
  else
  {
//    printf ("Failed\n");

  }
  return;
}
//void MonAdder::ServiceName(std::string server, std::string &svc)
//{
//  int atpos = server.find_first_of("@");
//  if (atpos == -1)
//  {
//    atpos = server.length();
//  }
//  svc = server.substr(0,atpos)+"/histos/"+m_serviceName;
//}
INServiceDescr *MonAdder::findINService(std::string servc)
{
  INServIter i;
  i = m_inputServicemap.find(servc);
  if (i != m_inputServicemap.end())
  {
    INServiceDescr *sd = i->second;
    return sd;
  }
  else
  {
    return 0;
  }
}

OUTServiceDescr *MonAdder::findOUTService(std::string servc)
{
  OUTServIter i;
  i = m_outputServicemap.find(servc);
  if (i != m_outputServicemap.end())
  {
    OUTServiceDescr *sd = i->second;
    return sd;
  }
  else
  {
    return 0;
  }
}
void MonAdder::TaskDied(std::string & task)
{
  TskServiceMap::iterator i;
  i = m_TaskMap.find(task);
  if (i != m_TaskMap.end())
  {
    m_TaskMap.erase(i);
    std::string service;
    service = i->second;
    INServIter k = m_inputServicemap.find(service);
    if (k != m_inputServicemap.end())
    {
      m_inputServicemap.erase(k);
      delete k->second->m_Info;
      delete k->second;
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
    if (m_inputServicemap.size() == 1)
    {
//      printf ("First client for adding... Creating our output service...%s\n",m_outsvcname.c_str());
      m_outservice = new ObjService(m_ser, m_outsvcname.c_str(),
          (char*) "C", (void*) &mpty, 4, &m_buffer, &m_usedSize);
      DimServer::start();
    }
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
    delete i->second->m_Info;
    m_inputServicemap.erase(i);
    TskServiceMap::iterator k =m_TaskMap.find(i->second->task);
    if (k != m_TaskMap.end())
    {
      m_TaskMap.erase(k);
    }
    else
    {
//      printf ("Task %s not found in map for removed service %s",TaskName.c_str(),ServiceName.c_str());
    }
    delete i->second;
  }
  if (m_inputServicemap.size() == 0)
  {
//    printf ("Last Client removed... Deleting output service %s\n",m_outsvcname.c_str());
    delete m_outservice;
    m_outservice = 0;
  }
}
