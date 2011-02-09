#ifndef MonAdder_H
#define MonAdder_H
#include "Gaucho/MonTypes.h"
//#include "MonInfo.h"
#include "Gaucho/ObjService.h"
//#include "ObjRPC.h"
#include <map>
#include <string>
#include <boost/regex.hpp>
//#include "CCPCSerializer.h"
//char hist_prefix[]="_HIST_";


typedef std::map<std::string, void*> HistMap;
typedef HistMap::iterator HistIter;

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
    last_update = 0;
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
  void *CycleCBarg;
  void (*CycleFn)(void*,void*,int, HistMap *, MonAdder *);
  void *m_buffer;
  int m_buffersize;
  int m_usedSize;
  INServiceMap m_inputServicemap;
  OUTServiceMap m_outputServicemap;
  TaskServiceMap m_TaskServiceMap;
  void TaskName(std::string &server, std::string &tname, std::string &tgen);
  void ServiceName(std::string server, std::string &svc);
  std::string m_taskname;
  std::string m_srcnode;
  std::string m_taskPattern;
  std::string m_servicePattern;
  boost::regex m_taskexp;
  boost::regex m_serviceexp;
  long long m_reference;
  HistMap m_hmap;
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
  virtual void add(void *buffer, int siz, MonInfo *h)=0;
  MonAdder(){CycleFn = 0;m_IsEOR = false;};
  void *Allocate(int siz);
  void *ReAllocate(int);
  void SetCycleFn(void CycleCB(void*,void*,int, HistMap *, MonAdder *), void *tis){CycleFn = CycleCB; CycleCBarg = tis;return;}
  INServiceDescr *findINService(std::string);
  OUTServiceDescr *findOUTService(std::string servc);
  void setSrcNode(std::string &src){m_srcnode = src;return;};
  void setOutDNS(std::string &src){m_outdns = src;return;};
  virtual void Configure(void)=0;
};
#endif
