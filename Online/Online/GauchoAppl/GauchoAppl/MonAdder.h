#ifndef ONLINE_GAUCHO_MonAdder_H
#define ONLINE_GAUCHO_MonAdder_H

#include "Gaucho/MonTypes.h"
#include "Gaucho/ObjService.h"
#include <map>
#include <string>
#include <boost/regex.hpp>
#include "dim/dic.hxx"
#include "Gaucho/MonInfo.h"
#include "RTL/rtl.h"
#include "Gaucho/Utilities.h"
#include "GauchoAppl/AddTimer.h"
#include "Gaucho/BRTL_Lock.h"
#include "dim/dis.hxx"
#define ADD_HISTO 1
#define ADD_COUNTER 2

//#include "CCPCSerializer.h"
//char hist_prefix[]="_HIST_";

#define offsetinBounds(base,recl,offs) !((AddPtr(base,offs)>AddPtr(base,recl))||(AddPtr(base,offs)<base)||(offs<0))

//typedef std::map<std::string, void*> MonMap;
class MonMap : public std::map<std::string, void*>
{
  public:
    void dumpKeys()
    {
      printf("Dump of MonMap...\n");
      for (auto it=begin();it!=end();it++)
      {
        printf("Histogram: '%s at %p'\n",it->first.c_str(),it->second);
      }
    };
    void dumpKeys(FILE *logFile)
    {
      fprintf(logFile,"Dump of MonMap...\n");
      for (auto it=begin();it!=end();it++)
      {
        fprintf(logFile,"Histogram: '%s at %p'\n",it->first.c_str(),it->second);
      }
    };
};

typedef MonMap::iterator MonIter;
class ObjService;
class HistServer;
class ObjRPC;
class MonInfo;
class DimInfo;
//class AddTimer;

//static int empty=-1;

class TaskSvcDescr
{
public:
  DimInfo *m_diminfo;
  std::string TaskName;
  std::string m_svcname;
  TaskSvcDescr(std::string &nam, DimInfo *i)
  {
    TaskName = nam;
    m_diminfo = i;
    m_svcname = "";
  }
};

class INServiceDescr
{
public:
  std::string m_serviceName;
  MonInfo *m_Info;
  std::string task;
  long long last_update;
  void *m_buffer;
  int m_bufsiz;
  int m_pid;
  int m_timeouts;
  INServiceDescr(char *name, MonInfo* info, std::string tsk)
  {
    m_serviceName = name;
    m_Info = info;
    last_update = -1;
    task = tsk;
    m_buffer = 0;
    m_bufsiz = 0;
    m_pid = -1;
    m_timeouts = 0;
  }
  virtual ~INServiceDescr()
  {
    deallocPtr(m_buffer);
    m_bufsiz =0;
  }
  void *CpyBuffer(void *buff, int siz)
  {
    if (m_bufsiz < siz)
    {
      deallocPtr(m_buffer);
      m_buffer = malloc(siz);
      if (m_buffer != 0)
      {
        m_bufsiz = siz;
        memcpy(m_buffer,buff,siz);
      }
    }
    else
    {
      memcpy(m_buffer,buff,siz);
    }
    return m_buffer;
  }
};
//class OUTServiceDescr
//{
//public:
//  std::string m_serviceName;
//  long long last_update;
//  void *m_svc;
//  void *m_buffer;
//  int m_buffersize;
//  OUTServiceDescr(char *name, void *srvc)
//  {
//    m_serviceName = std::string(name);
//    m_svc = srvc;//new ObjService(m_serviceName.c_str(),(char*)"C",&empty,4,&m_buffer,&m_buffersize);
//  }
//  ~OUTServiceDescr()
//  {
////    delete m_svc;
//  }
//};
typedef std::map<std::string, TaskSvcDescr*> TaskServiceMap;
typedef std::pair<std::string, TaskSvcDescr*> TaskServicePair;
typedef TaskServiceMap::iterator TaskServIter;

typedef std::map<std::string, INServiceDescr*> INServiceMap;
typedef std::pair<std::string, INServiceDescr*> INServicePair;
typedef INServiceMap::iterator INServIter;

//typedef std::map<std::string, OUTServiceDescr*> OUTServiceMap;
//typedef std::pair<std::string, OUTServiceDescr*> OUTServicePair;
//typedef OUTServiceMap::iterator OUTServIter;

class TaskDescr
{
public:
  INServiceMap m_servicemap;
  std::string m_TaskName;
  TaskDescr(char *name)
  {
    m_TaskName = name;
//    m_services.erase();
  }
};
typedef std::map<std::string, TaskDescr*> TskMap;
typedef std::pair<std::string, INServiceDescr*> TskPair;
typedef TskMap::iterator TskIter;
typedef std::map<std::string,std::string> TskServiceMap;
namespace AIDA
{
  class IHistogram1D;
}
class IGauchoMonitorSvc;

class TimeoutCmd;
class AdderSvc;
class MonAdder
{
public:
  long long m_rectmo;
  std::string m_firstSource;
  AddTimer *m_timer;
  TskServiceMap m_TaskMap;
  int m_type;
  bool m_updated;
  bool m_disableOutput;
  bool m_doPause;
  size_t m_expected;
  void *CycleCBarg;
  void *PauseArg;
  void (*CycleFn)(void*,void*,int, MonMap *, MonAdder *);
  void (*PauseFn)(void*);
  void *m_buffer;
  int m_buffersize;
  int m_usedSize;
  bool m_DebugOn;
//  FILE *logFile;
  std::string m_MyName;
  std::string m_NamePrefix;
  std::string m_outsvcname;
  INServiceMap m_inputServicemap;
  std::string m_MyServiceName;
  std::string m_RPCName;
//  OUTServiceMap m_outputServicemap;
  TaskServiceMap m_TaskServiceMap;
  void TaskName(std::string &server, std::string &tname, std::string &tgen);

  std::string   m_taskPattern;
  std::string   m_servicePattern;
  boost::regex  m_taskexp;
  boost::regex  m_serviceexp;
  long long     m_reference;
  MonMap        m_hmap;
  size_t        m_received;
  int           m_added;
  std::string   m_name;
  ObjService    *m_outservice;
  ObjRPC        *m_rpc;
  ObjSerializer *m_ser;
  ObjSerializer *m_RPCser;
  std::string    m_serviceName;
  std::string    m_cmdname;
  bool           m_expandRate;
  bool           m_IsEOR;
  bool           m_noRPC;
  bool 					m_SaveonUpdate;
  unsigned long long m_time0;
  DimBuffBase *m_RateBuff;
  bool m_locked;
//  std::string m_MyName;
//  std::string m_NamePrefix;
  DimBuffBase *m_oldProf;
  AdderSvc *m_parentAdderSvc;
public:
  static DimServerDns *m_ServiceDns;
  TimeoutCmd *m_Dimcmd;
  BRTLLock *m_lockid;
  BRTLLock m_maplock;
  bool m_dohisto;
  bool m_timeout;
  AIDA::IHistogram1D *m_histo;
  IGauchoMonitorSvc *m_monsvc;

  virtual void add(void *buffer, int siz, MonInfo *h)=0;
  virtual void basicAdd(void *buffer, int siz, MonInfo *h);
  virtual void addBuffer(void *buffer, int siz, MonInfo *h)=0;
  bool m_isSaver;
  MonAdder();
  virtual ~MonAdder();
  void *Allocate(int siz);
  void *ReAllocate(int);
  void SetCycleFn(void CycleCB(void*,void*,int, MonMap *, MonAdder *), void *tis){CycleFn = CycleCB; CycleCBarg = tis;return;}
  void SetPauseFn(void Pause(void*),void *tis){PauseFn = Pause;PauseArg=tis;return;}
  void setPause(bool dopause) {m_doPause=dopause;return;}
  void setParent(AdderSvc *parent){m_parentAdderSvc = parent;return;}
  void setSaveonUpdate(bool SaveonUpdate){m_SaveonUpdate = SaveonUpdate;return;}
  INServiceDescr *findINService(std::string);
//  OUTServiceDescr *findOUTService(std::string servc);
  virtual void Configure();
//  virtual void TaskHandler(char *, int);
//  virtual void ServiceHandler(DimInfo *, std::string &, char *, int);
  virtual void NewService(DimInfo *myInfo, std::string &TaskName, std::string &ServiceName);
  virtual void RemovedService(DimInfo *, std::string &TaskName, std::string &ServiceName);
  virtual void SynchronizeServices(std::string taskname, std::vector<std::string>&service_list);
  virtual void TaskDied(std::string & task);
  unsigned long long gettime();
  virtual void TimeoutHandler();
  void setIsSaver(bool p)
  {
    if (p)
    {
      m_lockid = new BRTLLock();
      m_lockid->m_name = m_name+"AdderLock";
    }
    m_isSaver = p;
    return;
  };
  int Lock()
  {
    if (m_lockid != 0)
    {
      int status = m_lockid->lockMutex();
      return status;
    }
    return 0;
  };
  int UnLock()
  {
    if (m_lockid != 0)
    {
      int status = m_lockid->unlockMutex();
      return status;
    }
    return 0;
  };

  int LockMap()
  {
      int status = m_maplock.lockMutex();
      return status;
  };
  int UnLockMap()
  {
    int status = m_maplock.unlockMutex();
    return status;
  };
  void dumpServices();
  void start();
  void stop();
  virtual void Update()=0;
  virtual void i_update();
};


class TimeoutCmd : public DimCommand
{
public:
  MonAdder *m_adder;
  TimeoutCmd(DimServerDns *dns, char *nam,MonAdder *tis): DimCommand(dns,nam,"I")
  {
    m_adder = tis;
  };
  virtual void commandHandler()
  {
//    printf("Received timeout command Handler called... Calling timeout handler for %s\n", m_adder->m_name.c_str());
    m_adder->TimeoutHandler();
  }
};
#endif
