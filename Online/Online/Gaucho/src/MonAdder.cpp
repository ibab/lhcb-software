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
    this->m_TaskMap.insert(std::pair<std::string,void*>(server,&this->m_inputServicemap));
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
  MonMap::iterator i;
  i = m_TaskMap.find(task);
  if (i != m_TaskMap.end())
  {
    std::string service;
    INServIter k, l;
//    INServiceMap *m = (INServiceMap*) i->second;
    INServiceMap::iterator siter;
    std::vector<INServiceMap::iterator> svec;
    siter = m_inputServicemap.begin();
    while (siter != m_inputServicemap.end())
    {
      for(siter= m_inputServicemap.begin();siter!=m_inputServicemap.end();siter++)
      {
        if (siter->first.find(task.c_str(),0) != std::string::npos)
        {
          m_inputServicemap.erase(siter);
          INServiceDescr *sd = siter->second;
//          printf("Deleting inservice %s from Task %s\n",sd->m_Info->getName(), task.c_str());
          delete sd->m_Info;
          delete sd;
          break;
        }
      }
    }
    if (m_inputServicemap.size()==0)
    {
      delete this->m_outservice;
      m_outservice = 0;
    }
    m_TaskMap.erase(i);
  }
}
//      siter = m->begin();
//      m->erase(siter);
//      delete siter->second->m_Info;
//      delete siter->second;
//      siter=m->begin();
//    }
//    service = i->second->m_svcname;
//    //        printf("Task %s died. Removing service @%s@\n",tskname.c_str(),service.c_str());
//    k = m_inputServicemap.find(service);
//    if (k != m_inputServicemap.end())
//    {
//      delete k->second->m_Info;
//      delete k->second;
//      m_inputServicemap.erase(k);
//      if (m_inputServicemap.empty())
//      {
//        //            printf ("No more client... Deleteing our output service...\n");
//        delete m_outservice;
//        m_outservice = 0;
//      }
//    }
//    else
//    {
//      //          printf("Very Funny... service @%s@ (%d)is not in service map...\n",service.c_str(),(int)service.size());
//      //          printf("map contents...\n");
//      //          int indx=0;
//      //          for (k=m_inputServicemap.begin(); k!= m_inputServicemap.end();k++)
//      //          {
//      //            printf("Index %d key: @%s@ (%d) @%s@\n",indx,k->first.c_str(),(int)k->first.size(),k->second->m_serviceName.c_str());
//      //            indx++;
//      //          }
//    }
//    TaskSvcDescr *ts = i->second;
//    delete ts->m_diminfo;
//    delete ts;
//    m_TaskServiceMap.erase(i);
//    //        printf("%s: Removing task %s\n",m_name.c_str(),tskname.c_str());
//    //        if (m_inputServicemap.empty())
//    //        {
//    //          printf("no more clients... Exiting volontarily....\n");
//    //          exit(0);
//    //        }
//  }
//  else
//  {
//    //        printf("Task %s not found in map\n",tskname.c_str());
//  }
//  INServIter k;
//  std::string srvc = task + m_serviceName;
//  k = m_inputServicemap.find(srvc);
//  if (k != m_inputServicemap.end())
//  {
//    INServiceDescr *is = k->second;
//    m_inputServicemap.erase(k);
//    delete is->m_Info;
//    delete is;
//    if (m_inputServicemap.empty())
//    {
//      //          printf ("No more client... Deleteing our output service...\n");
//      delete m_outservice;
//      m_outservice = 0;
//    }
//  }
//}
void MonAdder::NewService(DimInfo *, std::string &TaskName, std::string &ServiceName)
{
  int status;
  status = boost::regex_search(ServiceName, m_serviceexp);
  if (status)
  {
//    printf("Comparing %s with Pattern %s...",ServiceName.c_str(),this->m_servicePattern.c_str());
//    printf("SUCCESS\n");
    MonMap::iterator i;
//    printf("New Service %s from task %s\n",ServiceName.c_str(),TaskName.c_str());
    i = this->m_TaskMap.find(TaskName);
    if (i != m_TaskMap.end())
    {
      INServiceMap *m = (INServiceMap*) i->second;
      INServiceMap::iterator k;
      k = m->find(ServiceName);
      if (k == m->end())
      {
        INServiceDescr *sd = new INServiceDescr((char*) (ServiceName.c_str()),
            new MonInfo((char*) (ServiceName.c_str()), &BufferAdder, this));
        //            printf("Service: %s\n",service->at(0).c_str());
        m->insert(INServicePair(ServiceName, sd));
        //            printf("%s: Adding service @%s@\n",m_name.c_str(),service->at(0).c_str());
        if (m->size() == 1)
        {
//          printf ("First client for adding... Creating our output service...%s\n",m_outsvcname.c_str());
          m_outservice = new ObjService(m_ser, m_outsvcname.c_str(),
              (char*) "C", (void*) &mpty, 4, &m_buffer, &m_usedSize);
          DimServer::start();
        }
      }
    }
    else
    {
      printf("FUNNY... New Service from unconnected tas... %s\n", TaskName.c_str());
    }
  }
  else
  {
//    printf("Failed\n");
  }
}
void MonAdder::RemovedService(DimInfo *, std::string &TaskName, std::string &ServiceName)
{
  int status;
  status = boost::regex_search(ServiceName,m_serviceexp);
  if (status)
  {
    MonMap::iterator i;
    i = this->m_TaskMap.find(TaskName);
    if (i != m_TaskMap.end())
    {
      INServiceMap *m=(INServiceMap*)i->second;
      INServiceMap::iterator k;
      k = m->find(ServiceName);
      if (k != m->end())
      {
        INServiceDescr *sd = k->second;
//            printf("Service: %s\n",service->at(0).c_str());
        m->erase(k);
        delete sd->m_Info;
        delete sd;
        if (m->size() == 0)
        {
//              printf ("First client for adding... Creating our output service...\n");
          delete m_outservice;
          m_outservice = 0;
        }
      }
    }
  }
}
