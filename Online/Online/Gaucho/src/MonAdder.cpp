#include "Gaucho/MonAdder.h"
//#include "hist_types.h"
//#include "HistService.h"
//#include "HistServer.h"
//#include "HistRPC.h"
#include "Gaucho/MonTypes.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/Utilities.h"
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
    tname = server.substr(0,server.find("@"));
    tgen = "";
  }
  return;
}
void MonAdder::ServiceName(std::string server, std::string &svc)
{
  int atpos = server.find_first_of("@");
  if (atpos == -1)
  {
    atpos = server.length();
  }
  svc = server.substr(0,atpos)+"/histos/"+m_serviceName;
}
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
void MonAdder::TaskHandler(char *input, int )
{
//  char *input;
//  int siz;
//  input = (char*)m_DNSInfo->getData();
//  siz = m_DNSInfo->getSize();
  if (strcmp(input,"DEAD") == 0)
  {
//    printf("DNS Down!!!!\n");
    return;
  }
//  printf("New Message from DNS %s\n",input);
  bool Newe;
  Newe = true;
  char *sinput=input;
  if ((input[0] == '+') || (input[0] == '-') || (input[0] == '!'))
  {
    Newe = (input[0] == '+');
    sinput = &input[1];
  }
  dyn_string *server_list = Strsplit(sinput,(char*)"|");

  dyn_string::iterator i;
  for (i=server_list->begin();i!=server_list->end();i++)
  {
    std::string server = *i;
    std::string tskname,tgen;
    std::string servc;
    TaskName(server,tskname,tgen);
    if (tskname != "")
    {
      if (Newe)
      {
        TaskServIter i;
        i = m_TaskServiceMap.find(tskname);
        if (i == m_TaskServiceMap.end())
        {
          servc = tskname+std::string("/SERVICE_LIST");
          DimInfo *slist = new DimInfo(servc.c_str(),(char *)"DEAD",&gg_ServHandler);
          TaskSvcDescr *ts = new TaskSvcDescr(tskname,slist);
          m_TaskServiceMap.insert(TaskServicePair(tskname,ts));
//          printf("Listening to Task %s\n",tskname.c_str());
        }
        else
        {
//          printf("Duplicate Task %s. DNS restart? \n",tskname.c_str());
        }
      }
      else
      {
        TaskServIter i;
        i = m_TaskServiceMap.find(tskname);
        if (i != m_TaskServiceMap.end())
        {
          std::string service;
          INServIter k,l;
          service = i->second->m_svcname;
  //        printf("Task %s died. Removing service @%s@\n",tskname.c_str(),service.c_str());
          k = m_inputServicemap.find(service);
          if (k != m_inputServicemap.end())
          {
            delete k->second->m_Info;
            delete k->second;
            m_inputServicemap.erase(k);
            if (m_inputServicemap.empty())
            {
  //            printf ("No more client... Deleteing our output service...\n");
              delete m_outservice;
              m_outservice = 0;
            }
          }
          else
          {
  //          printf("Very Funny... service @%s@ (%d)is not in service map...\n",service.c_str(),(int)service.size());
  //          printf("map contents...\n");
  //          int indx=0;
  //          for (k=m_inputServicemap.begin(); k!= m_inputServicemap.end();k++)
  //          {
  //            printf("Index %d key: @%s@ (%d) @%s@\n",indx,k->first.c_str(),(int)k->first.size(),k->second->m_serviceName.c_str());
  //            indx++;
  //          }
          }
          TaskSvcDescr *ts = i->second;
          delete ts->m_diminfo;
          delete ts;
          m_TaskServiceMap.erase(i);
  //        printf("%s: Removing task %s\n",m_name.c_str(),tskname.c_str());
  //        if (m_inputServicemap.empty())
  //        {
  //          printf("no more clients... Exiting volontarily....\n");
  //          exit(0);
  //        }
        }
        else
        {
  //        printf("Task %s not found in map\n",tskname.c_str());
        }
        INServIter k;
        std::string srvc=tskname+m_serviceName;
        k = m_inputServicemap.find(srvc);
        if (k != m_inputServicemap.end())
        {
          INServiceDescr *is = k->second;
          m_inputServicemap.erase(k);
          delete is->m_Info;
          delete is;
          if (m_inputServicemap.empty())
          {
  //          printf ("No more client... Deleteing our output service...\n");
            delete m_outservice;
            m_outservice = 0;
          }
        }
      }
    }
  }
}

void MonAdder::ServiceHandler(DimInfo *myInfo, char *input,int )
{
  if (strcmp(input,"DEAD") == 0)
  {
    return;
  }
//  printf("New Message from Service List Service %s data:\n%s\n",myInfo->getName(),input);
  bool Newe;
  Newe = false;
  char *sinput=input;
  if ((input[0] == '+') || (input[0] == '-') || (input[0] == '!'))
  {
    Newe = (input[0] == '+');
    sinput = &input[1];
  }
  else
  {
    Newe = true;
  }
  dyn_string *service_list = Strsplit(sinput,(char*)"\n");
  for (unsigned int j=0;j<service_list->size();j++)
  {
    std::string service_line(service_list->at(j));
    dyn_string *service = Strsplit((char*)service_line.c_str(),(char*)"|");
    bool status;
//    printf("comparing %s to pattern %s\n",service->at(0).c_str(),this->m_servicePattern.c_str());
    status = boost::regex_search(service->at(0),m_serviceexp);
    if (status)
    {
      INServIter i;
      i = m_inputServicemap.find(service->at(0));
      if (Newe)
      {
        if (i == m_inputServicemap.end())
        {
//          servc = svc->at(0);
          TaskServIter k;
          std::string taskn=std::string(myInfo->getName());
          taskn = taskn.substr(0,taskn.find("/SERVICE_LIST"));
          k = m_TaskServiceMap.find(taskn);
          if (k != m_TaskServiceMap.end())
          {
            k->second->m_svcname = service->at(0);
          }
          INServiceDescr *sd = new INServiceDescr((char*)(service->at(0).c_str()),
          new MonInfo((char*)(service->at(0).c_str()),&BufferAdder,this));
//            printf("Service: %s\n",service->at(0).c_str());
          m_inputServicemap.insert(INServicePair(service->at(0).c_str(),sd));
//            printf("%s: Adding service @%s@\n",m_name.c_str(),service->at(0).c_str());
          if (m_inputServicemap.size() == 1)
          {
//              printf ("First client for adding... Creating our output service...\n");
            m_outservice = new ObjService(m_ser,m_outsvcname.c_str(),(char*)"C",(void*)&mpty, 4, &m_buffer, &m_usedSize);
            DimServer::start();
          }
        }
        else
        {
//            printf("Duplicate service %s. DNS restart? \n",service->at(0).c_str());
        }
      }
      else
      {
        if (i != m_inputServicemap.end())
        {
          TaskServIter k;
          std::string taskn=std::string(myInfo->getName());
          taskn = taskn.substr(0,taskn.find("/SERVICE_LIST"));
          k = m_TaskServiceMap.find(taskn);
          if (k != m_TaskServiceMap.end())
          {
            k->second->m_svcname = "";
          }
          INServiceDescr *sd = i->second;
          delete sd->m_Info;
          delete sd;
          m_inputServicemap.erase(i);
    //      printf("%s: Removing service %s\n",m_name.c_str(),servc.c_str());
          if (m_inputServicemap.empty())
          {
//            printf ("No more client... Deleteing our output service...\n");
            delete m_outservice;
            m_outservice = 0;
          }
        }
      }
    }
  }
}
void HAdderServInfoHandler::infoHandler(void)
{
  char *input;
  int siz;
  input = (char*)this->itsService->getData();
  siz = this->itsService->getSize();
//  printf("Service Info Handler %s. Input :\n%s\n",this->itsService->getName(),input);
  std::vector<MonAdder*>::iterator i;
  for (i=MonAdder::gg_AdderList.begin();i!=MonAdder::gg_AdderList.end();i++)
  {
    MonAdder *p;
    p = *i;
    p->ServiceHandler(itsService,input,siz);
  }
//    m_Hadder->TaskHandler();
//    m_Hadder->ServiceHandler(itsService);
}

void HAdderTaskInfoHandler::infoHandler(void)
{
  char *input;
  int siz;
  input = (char*)this->itsService->getData();
//  printf("Server Info Handler. Input :%s\n",input);
  siz = this->itsService->getSize();
  std::vector<MonAdder*>::iterator i;
  for (i=MonAdder::gg_AdderList.begin();i!=MonAdder::gg_AdderList.end();i++)
  {
    MonAdder *p;
    p = *i;
    p->TaskHandler(input,siz);
  }
//    m_Hadder->TaskHandler();
}

