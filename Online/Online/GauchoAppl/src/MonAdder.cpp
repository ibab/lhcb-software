// workaround for https://its.cern.ch/jira/browse/GAUDI-1168
#include "GaudiKernel/MinimalDataObjectHandle.h"
#include "dim.h"

#include "GauchoAppl/MonAdder.h"
#include "Gaucho/MonTypes.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/Utilities.h"
#include "GauchoAppl/AdderSys.h"
#include "GauchoAppl/AddSerializer.h"
#include "Gaucho/ObjRPC.h"
#include "GauchoAppl/AddTimer.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "AIDA/IHistogram1D.h"
#include "components/AdderSvc.h"
#include "GauchoAppl/SaveTimer.h"
static int mpty;
DimServerDns *MonAdder::m_ServiceDns = 0;
typedef std::pair<std::string, MonObj*> MonPair;
extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, MonInfo *h)
  {
    MonAdder *adder = (MonAdder*)tis;
    adder->basicAdd(buff, siz,h);
  }
}
MonAdder::MonAdder()
{
  m_received    = 0;
  CycleFn       = 0;
  PauseFn       = 0;
  m_buffersize  = 0;
  m_buffer      = 0;
  m_usedSize    = 0;
  m_reference   = -1;
  m_expandRate  = false;
//  m_lockid      = 0;
//  m_maplock     = 0;
  m_IsEOR       = false;
  m_added       = 0;
  m_noRPC       = false;
  m_outservice  = 0;
  m_RPCser      = 0;
  m_rpc         = 0;
  m_ser         = 0;
  m_RPCser      = 0;
  m_updated     = false;
  m_timer = 0;
  m_disableOutput = false;
  m_histo = 0;
//  m_DimDns = 0;
  m_Dimcmd = 0;
  m_parentAdderSvc = 0;
  m_doPause = false;
  m_SaveonUpdate = false;
  m_isSaver = false;
  m_expected = 0;
  m_timeout = 0;
  m_monsvc = 0;
  m_DebugOn = false;
  PauseArg = 0;
  m_type = 0;
  m_RateBuff=0;
  CycleCBarg = 0;
  m_time0 = 0;
  m_locked = false;
  m_lockid = 0;
  m_dohisto = false;
  m_rectmo = 0;
  m_oldProf = 0;
  m_updateStamp = 0;
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
  deletePtr(m_Dimcmd);
  deletePtr(m_ServiceDns);
}

void MonAdder::Configure()
{
  std::string nodename = RTL::nodeNameShort();
  if (nodename != "")
  {
    toLowerCase(nodename);
    m_name= "MON_" + m_MyName;
  }
  m_outsvcname = m_name+m_serviceName;
  m_cmdname = nodename+"_"+RTL::processName()+"/"+m_MyName+/*"/"+*/m_serviceName+"/Timeout";
//  DimClient::getDnsNode();
//  if (m_DimDns == 0)
//  {
//    std::string cdnsname = DimClient::getDnsNode();
//    std::string sdnsname = DimServer::getDnsNode();
//    toLowerCase(cdnsname);
//    toLowerCase(sdnsname);
//    if (sdnsname == std::string("localhost"))
//    {
//      sdnsname = RTL::nodeNameShort();
//      toLowerCase(sdnsname);
//    }
//    dyn_string *cdns = Strsplit(cdnsname.c_str(),".");
//    dyn_string *sdns = Strsplit(sdnsname.c_str(),".");
//    if (cdns->at(0) != sdns->at(0))
//    {
//      m_ServiceDns = new DimServerDns(cdnsname.c_str());
////      printf("Created new DimServerDns with node name %s\n",cdnsname.c_str());
//      m_DimDns->autoStartOn();
//      DimServer::start(m_DimDns, (char*)((RTL::processName()+"/").c_str()));
//    }
//  }
  if (m_ServiceDns != 0)
  {
    m_ServiceDns->autoStartOn();
    DimServer::start(m_ServiceDns,(char*) (("MON_" + RTL::processName()).c_str()));
  }
  m_timer = new AddTimer(this,m_rectmo);
  m_serviceexp = boost::regex(m_servicePattern.c_str(),boost::regex_constants::icase);
  m_taskexp = boost::regex(m_taskPattern.c_str(),boost::regex_constants::icase);
//  BRTL_create_mutex(&m_maplock);
  m_ser = new AddSerializer((ObjMap*)&m_hmap);
  m_rpc = 0;
  if (!m_noRPC)
  {
    std::string nam;
    if ( m_type == ADD_HISTO )
    {
      m_RPCName = m_name + "/Histos/HistCommand";
    }
    else if ( m_type == ADD_COUNTER )
    {
      m_RPCName = m_name+"/Counter/HistCommand";
    }
//    m_RPCser = new AddSerializer((ObjMap*)&m_hmap);
//    m_maplock.m_name = m_RPCName;
//    m_rpc = new ObjRPC(m_ServiceDns,m_RPCser,(char*)m_RPCName.c_str(), (char*)"I:1;C",(char*)"C", &m_maplock, 0/*this->m_lockid*/);
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
//      if (newbuffer != m_buffer)
//      {
        m_hmap.clear();
        void *bend = AddPtr(newbuffer,m_usedSize);
        void *hstart = AddPtr(newbuffer,sizeof(SerialHeader));
        DimBuffBase *pp = (DimBuffBase*)hstart;
        while (pp<bend)
        {
          if (offsetinBounds(pp,pp->reclen,pp->nameoff))
          {
            char *nam = (char*)AddPtr(pp,pp->nameoff);
            m_hmap.insert(MonPair(std::string(nam),(MonObj*)pp));
          }
          else
          {
            printf("Problem in reallocating buffer: Name offset in record bad. %d (%0x)\n",pp->nameoff,pp->nameoff);
          }
          pp=(DimBuffBase*)AddPtr(pp,pp->reclen);
        }
        m_buffer = newbuffer;
//      }
      m_buffersize = newsiz;
      SerialHeader *hd = (SerialHeader*)m_buffer;
      hd->buffersize += incsiz;
    }
    else
    {
      free(m_buffer);
      m_buffersize = 0;
      m_buffer = 0;
    }
  }
  return m_buffer;
}

void MonAdder::TaskName(std::string &server, std::string &tname, std::string &tgen)
{
//  printf("Comparing Server %s with %s...",server.c_str(),this->m_taskPattern.c_str());
  bool status = boost::regex_search(server,m_taskexp);
  if (status)
  {
//    printf("=====>SUCCESS\n");
    tname = server;
    tgen = "";
    return;
  }
//  printf("FAILED\n");
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

//OUTServiceDescr *MonAdder::findOUTService(std::string servc)
//{
//  OUTServIter i = m_outputServicemap.find(servc);
//  if (i != m_outputServicemap.end())
//  {
//    OUTServiceDescr *sd = i->second;
//    return sd;
//  }
//  return 0;
//}
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
//      printf ("TaskDied: Last Input Service... Deleting output service %s\n",m_outsvcname.c_str());
      delete m_outservice;
      m_outservice = 0;
    }
  }
}

void MonAdder::SynchronizeServices(std::string taskname, std::vector<std::string>&service_list)
{
  unsigned int i;
  bool fnd = false;
  std::string le;
  std::string tskname,tgen;
//  printf("In SynchronizeServices: Taskname = %s, expected task name =%s\n",taskname.c_str(),m_taskPattern.c_str());
  bool status = boost::regex_search(taskname,m_taskexp);
  if (!status)
  {
    return;
  }
  for (i=0;i<service_list.size();i++)
  {
    NewService(0,taskname,service_list[i]);
  }
  INServiceMap::iterator j;
  std::vector<std::string> toRemoveList;



  TskServiceMap::iterator ii = m_TaskMap.find(taskname);
  if (ii != m_TaskMap.end())
  {
    const std::string& service = ii->second;
    for (i=0;i<service_list.size();i++)
    {
      if (service_list[i] == service)
      {
        fnd = true;
        break;
      }
    }
    if (!fnd)
    {
      toRemoveList.push_back(service);
    }
    for(i=0;i<toRemoveList.size();i++)
    {
//      printf("removing service %s (%s) from taskname %s apparently matching pattern %s\n",toRemoveList[i].c_str(),this->m_servicePattern.c_str(),taskname.c_str(),m_taskPattern.c_str());
      RemovedService(0,taskname,toRemoveList[i]);
    }
  }
  else
  {
    return;
  }
}

void MonAdder::NewService(DimInfo *, std::string &TaskName, std::string &ServiceName)
{
  int status;
//  printf("Comparing %s with Pattern %s...",ServiceName.c_str(),this->m_servicePattern.c_str());
  status = boost::regex_search(ServiceName, m_serviceexp);
  if (status)
  {
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
      if (!m_disableOutput)
      {
        m_outservice = new ObjService(m_ServiceDns,m_ser,m_outsvcname.c_str(),(char*) "C", (void*) &mpty, 4, &m_buffer, &m_usedSize);
      }
    }
    if (0 == m_rpc)
    {
      if (!this->m_noRPC)
      {
        m_RPCser = new AddSerializer((ObjMap*)&m_hmap);
        m_maplock.m_name = m_RPCName;
        m_rpc = new ObjRPC(m_ServiceDns,m_RPCser,(char*)m_RPCName.c_str(), (char*)"I:1;C",(char*)"C", &m_maplock, 0/*this->m_lockid*/);
      }
    }
    if (0==m_Dimcmd)
    {
      m_Dimcmd = new TimeoutCmd(m_ServiceDns,(char*)m_cmdname.c_str(),this);
    }
    if (m_ServiceDns != 0)
    {
      DimServer::start(m_ServiceDns);
    }
  }
  else
  {
//    printf("Not Interesting\n");
  }
}
void MonAdder::RemovedService(DimInfo *, std::string &, std::string &ServiceName)
{
//  int status;
//  printf("Removed Service %s My pattern %s\n",ServiceName.c_str(),this->m_servicePattern.c_str());
//  status = boost::regex_search(ServiceName,m_serviceexp);
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
    if (m_inputServicemap.size() == 0)
    {
//      printf("RemovedService: Last Service %s removed... Deleting our output service",this->m_servicePattern.c_str());
      if (m_outservice != 0)
      {
//        printf("Name %s\n",m_outservice->getName());
      }
      else
      {
//        printf("\n");
      }
      deletePtr(m_outservice);
    }
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
void MonAdder::start()
{
  if (m_timer != 0)
  {
    m_timer->Start();
  }
}
void MonAdder::stop()
{
  if (m_timer != 0)
  {
    m_timer->Stop();
  }
}
void MonAdder::TimeoutHandler()
{
  INServIter i;
  ::lib_rtl_output(LIB_RTL_INFO,"MonAdder Timeout handler for service %s for expected time %lli\n",
      m_name.c_str(),m_reference);
//  printf("MonAdder Timeout handler for expected time %lli\n",m_reference);
  DimLock l;
  for (i=this->m_inputServicemap.begin();i!=m_inputServicemap.end();i++)
  {
    INServiceDescr *d = i->second;
    if (d->last_update < this->m_reference)
    {
      ::lib_rtl_output(LIB_RTL_INFO,"Timeout from source %s expected %lli last received %lli\n",
          d->m_Info->m_TargetService.c_str(),m_reference,d->last_update);
      d->m_timeouts++;
//      printf("Timeout from source %s (PID = %d) expected %lli last received %lli cons. TMO %d\n ",
//          d->m_Info->m_TargetService.c_str(),d->m_pid, m_reference,d->last_update,d->m_timeouts);
      if (d->m_buffer != 0)
      {
        add(d->m_buffer,d->m_bufsiz,d->m_Info);
      }
      else
      {
        m_received++;
      }
    }
  }
//  printf("Timeout Handler after add... received %d expected %d\n",(int)m_received,(int)m_expected);
  m_timeout = true;
  Update();
  if (m_isSaver && m_SaveonUpdate)
  {
    this->m_parentAdderSvc->m_SaveTimer->timerHandler();
  }
//  printf("called Update...\n");
}
void MonAdder::i_update(bool force)
{
  if (m_DebugOn)
  {
    ::lib_rtl_output(LIB_RTL_INFO,"MonAdder::i_update: Expected Number: %d received: %d force: %s\n",
        m_expected,m_received,force?"true":"false");
  }
  if ((m_received >= m_expected) || force)
  {
    if (!m_timeout)
    {
      this->m_timer->Stop();
    }
    //    //printf("Finished one cycle. Updating our service... %d %d\n", m_received,expected);
    if (m_reference != m_updateStamp)
    {
      Update();
      m_updateStamp = m_reference;
      if (m_isSaver && m_SaveonUpdate)
      {
        this->m_parentAdderSvc->m_SaveTimer->timerHandler();
      }
      if (CycleFn != 0)
      {
        (*CycleFn)(CycleCBarg, m_buffer, m_usedSize, &m_hmap, this);
      }
      if (m_doPause)
      {
        if (PauseFn != 0)
        {
          (*PauseFn)(PauseArg);
        }
      }
    }
    m_added = 0;
    m_received = 0;
    m_reference = 0;
  }
}
void MonAdder::basicAdd(void *buff, int siz, MonInfo *h)
{
  unsigned long long tim = gettime();
//  printf("Basic Add called...expected %d received %d\n",(int)m_expected,(int)m_received);
  SerialHeader* header= ((SerialHeader*)buff);
  if (siz == 4)
  {
    printf("[ERROR] No Link from %s. Update counts....\n",h->m_TargetService.c_str());
    m_received++;
    i_update();
    return;
  }
  if (header->m_magic != SERIAL_MAGIC)
  {
    printf("========> [ERROR] Serial Magic Word Missing  from connection %s\n",h->m_TargetService.c_str());
    m_received++;
    i_update();
    return;
  }
  INServiceDescr *isvcd;
  INServIter it= m_inputServicemap.find(h->m_TargetService);
  if (it == m_inputServicemap.end())
  {
    printf("!!!!!Logic Error Adder add method called with service that's not in the input service map %s\n",h->m_TargetService.c_str());
    return;
  }
  isvcd = it->second;
  if (buff != isvcd->m_buffer)
  {
    buff = isvcd->CpyBuffer(buff,siz);
  }
  isvcd->m_pid = DimClient::getServerPid();
  isvcd->m_timeouts = 0;
//  printf("Received data from %s\n",h->m_TargetService.c_str());
  m_expected = m_inputServicemap.size();
  long long current  = (m_IsEOR) ? header->run_number : header->ser_tim;
  isvcd->last_update = current;

  m_RateBuff = 0;
  if (m_DebugOn)
  {
    ::lib_rtl_output(LIB_RTL_INFO,
        "MonAdder received packet from %s. Expected Number %d received %d,timestamp %ld current Timestamp %ld\n",
        h->m_TargetService.c_str(),m_expected,m_received,m_reference,current);
  }
  if (m_histo != 0)
  {
    //printf("HistAdder Locking MonitorSvc\n");
    this->m_monsvc->Lock();
    unsigned long long dtim = tim-m_time0;
    double ftim = dtim/1000000000;
    if (m_histo != 0)
    {
      m_histo->fill(ftim);
    }
    //printf("HistAdder UNLocking MonitorSvc\n");
    this->m_monsvc->UnLock();
  }
  if (m_reference < current)
  {
    if (m_DebugOn)
    {
      ::lib_rtl_output(LIB_RTL_INFO,"MonAdder: First Fragment from %s...\n",h->m_TargetService.c_str());
    }
    if ((m_received>0) && (m_received < m_expected) && !m_timeout)
    {
      if (m_DebugOn)
      {
        ::lib_rtl_output(LIB_RTL_INFO,"MonAdder: Previous period didn't finish. Expected: %d, received:%d. Forcing update\n",
            m_expected, m_received);
      }
      i_update(true);
    }
//    printf("First fragment received from %s... starting timer...\n",h->m_TargetService.c_str());
    m_timeout = false;
    m_firstSource = isvcd->m_serviceName;
    if(this->m_rectmo >0) this->m_timer->Start();
//    if ((m_reference != -1) && !m_updated)
//    {
//      if (m_outservice != 0)
//      {
//        m_outservice->Serialize();
//        m_outservice->Update();
//        m_updated = false;
//      }
//    }
    m_time0 = tim;
    m_added++;
    m_received = 1;
    if (m_isSaver)
    {
      if (m_received == 1)
      {
        if (!m_locked)
        {
          //printf("HistAdder Locking\n");
          Lock();
        }
        m_locked = true;
      }
    }
    ////printf ("New cycle %s... %d\n",h->m_TargetService.c_str(),m_received);
    void *p = Allocate(siz);
    m_reference = current;
    memset(m_buffer,0,m_buffersize);
    m_hmap.clear();
    Memcpy(p,buff,siz);
    m_usedSize = siz;
//    SerialHeader *sh = (SerialHeader*)m_buffer;
//    if (sh->buffersize != siz)
//    {
//      printf("[Sever Error] Received first packet if size %d where size in header is %d\n",siz,sh->buffersize);
//    }
    void *bend = AddPtr(p,siz);
    void *hstart = AddPtr(p,sizeof(SerialHeader));
    DimBuffBase *pp = (DimBuffBase*)hstart;
    while (pp<bend)
    {
      if (pp->reclen == 0) break;
      if (offsetinBounds(pp,pp->reclen,pp->nameoff))
      {
        char *nam = (char*)AddPtr(pp,pp->nameoff);
        if (offsetinBounds(pp,pp->reclen,pp->dataoff))
        {
    //      //printf("Histogram Name: %s\n",nam);
          std::string nams =nam;
          //printf("HistAdder Locking MAP\n");
          LockMap();
          m_hmap.insert(std::make_pair(nams,pp));
          //printf("HistAdder UNLocking MAP\n");
          UnLockMap();
          if ((MONTYPE)pp->type == H_RATE)
          {
            m_RateBuff = pp;
          }
        }
        else
        {
          printf("Data Offset negative or bigger than Record Length for %s %s\n",nam, h->m_TargetService.c_str());
        }
      }
      else
      {
        printf("Bad Name offset %d (%0x) from %s\n",pp->nameoff,pp->nameoff, h->m_TargetService.c_str());
      }
      pp=(DimBuffBase*)AddPtr(pp,pp->reclen);
    }
  }
  else if (m_reference == current)
  {
    if (m_DebugOn)
    {
      ::lib_rtl_output(LIB_RTL_INFO,"MonAdder %s matching TimeStamps... Adding\n",h->m_TargetService.c_str());
    }
    add(buff, siz, h);
  }
  else
  {
    if (m_DebugOn)
    {
      ::lib_rtl_output(LIB_RTL_INFO,"late update from %s\n m_expected %lli received %lli. Using stored buffer...\n",h->m_TargetService.c_str(),m_reference,current);
    }
//    printf("late update from %s\n m_expected %lli received %lli. Using stored buffer...\n",h->m_TargetService.c_str(),m_reference,current);
    add(buff,siz,h);
//    m_received++;
  }
  i_update();
}
