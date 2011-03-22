#ifndef MonAdder_H
#define MonAdder_H
#include "Gaucho/MonTypes.h"
//#include "MonInfo.h"
#include "Gaucho/ObjService.h"
//#include "ObjRPC.h"
#include <map>
#include <string>
#include <boost/regex.hpp>
#include "dic.hxx"
#include "Gaucho/MonInfo.h"
#include "RTL/rtl.h"
#define ADD_HISTO 1
#define ADD_COUNTER 2

//#include "CCPCSerializer.h"
//char hist_prefix[]="_HIST_";


typedef std::map<std::string, void*> MonMap;
typedef MonMap::iterator MonIter;

class ObjService;
class HistServer;
class ObjRPC;
class MonInfo;
class DimInfo;

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
  long long last_update;
  INServiceDescr(char *name, MonInfo* info)
  {
    m_serviceName = name;
    m_Info = info;
    last_update = -1;
  }
};
class OUTServiceDescr
{
public:
  std::string m_serviceName;
  long long last_update;
  void *m_svc;
  void *m_buffer;
  int m_buffersize;
  OUTServiceDescr(char *name, void *srvc)
  {
    m_serviceName = std::string(name);
    m_svc = srvc;//new ObjService(m_serviceName.c_str(),(char*)"C",&empty,4,&m_buffer,&m_buffersize);
  }
  ~OUTServiceDescr()
  {
//    delete m_svc;
  }
};
typedef std::map<std::string, TaskSvcDescr*> TaskServiceMap;
typedef std::pair<std::string, TaskSvcDescr*> TaskServicePair;
typedef TaskServiceMap::iterator TaskServIter;

typedef std::map<std::string, INServiceDescr*> INServiceMap;
typedef std::pair<std::string, INServiceDescr*> INServicePair;
typedef INServiceMap::iterator INServIter;

typedef std::map<std::string, OUTServiceDescr*> OUTServiceMap;
typedef std::pair<std::string, OUTServiceDescr*> OUTServicePair;
typedef OUTServiceMap::iterator OUTServIter;

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

class MonAdder
{
public:
  MonMap m_TaskMap;
  int m_type;
  void *CycleCBarg;
  void (*CycleFn)(void*,void*,int, MonMap *, MonAdder *);
  void *m_buffer;
  int m_buffersize;
  int m_usedSize;
  std::string m_MyName;
  std::string m_NamePrefix;
  std::string m_outsvcname;
  INServiceMap m_inputServicemap;
  OUTServiceMap m_outputServicemap;
  TaskServiceMap m_TaskServiceMap;
  void TaskName(std::string &server, std::string &tname, std::string &tgen);
//  void ServiceName(std::string server, std::string &svc);
  std::string m_taskname;
  std::string m_srcnode;
  std::string m_taskPattern;
  std::string m_servicePattern;
  boost::regex m_taskexp;
  boost::regex m_serviceexp;
  long long m_reference;
  MonMap m_hmap;
  int m_expected;
  int m_received;
  int m_added;
  std::string m_name;
  ObjService *m_outservice;
  ObjRPC *m_rpc;
  ObjSerializer *m_ser;
  std::string m_serviceName;
  bool m_expandRate;
  std::string hist_prefix;
  std::string m_outdns;
  bool m_IsEOR;
  bool m_noRPC;

public:
  lib_rtl_lock_t m_lockid;
  lib_rtl_lock_t m_maplock;
  virtual void add(void *buffer, int siz, MonInfo *h)=0;
  bool m_isSaver;
  MonAdder(){CycleFn = 0;m_IsEOR = false;};
  virtual ~MonAdder(){}
  void *Allocate(int siz);
  void *ReAllocate(int);
  void SetCycleFn(void CycleCB(void*,void*,int, MonMap *, MonAdder *), void *tis){CycleFn = CycleCB; CycleCBarg = tis;return;}
  INServiceDescr *findINService(std::string);
  OUTServiceDescr *findOUTService(std::string servc);
  void setSrcNode(std::string &src){m_srcnode = src;return;};
  void setOutDNS(std::string &src){m_outdns = src;return;};
  virtual void Configure(void)=0;
//  virtual void TaskHandler(char *, int);
//  virtual void ServiceHandler(DimInfo *, std::string &, char *, int);
  virtual void NewService(DimInfo *myInfo, std::string &TaskName, std::string &ServiceName);
  virtual void RemovedService(DimInfo *, std::string &TaskName, std::string &ServiceName);
  virtual void TaskDied(std::string & task);
  void setIsSaver(bool p)
  {
    if (p)
    {
      lib_rtl_create_lock(0,&m_lockid);
    }
    m_isSaver = p;
    return;
  };
  int Lock()
  {
    if (m_lockid != 0)
    {
      int status;
//      printf("HistAdder Locking\n");
      status = lib_rtl_lock(m_lockid);
//      printf("HistAdder Locked\n");
      return status;
    }
    else
    {
      return 0;
    }
  };
  int UnLock()
  {
    if (m_lockid != 0)
    {
      int status;
//      printf("HistAdder Un-Locking\n");
      status = lib_rtl_unlock(m_lockid);
//      printf("HistAdder UnLocked\n");
      return status;
    }
    else
    {
      return 0;
    }
  };
  int LockMap()
  {
    if (m_maplock != 0)
    {
      int status;
      status = lib_rtl_lock(m_maplock);
      return status;
    }
    else
    {
      return 0;
    }
  };
  int UnLockMap()
  {
    if (m_maplock != 0)
    {
      int status;
      status = lib_rtl_unlock(m_maplock);
      return status;
    }
    else
    {
      return 0;
    }
  };
};
#endif
