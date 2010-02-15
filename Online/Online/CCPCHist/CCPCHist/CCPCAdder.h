#ifndef CCPCADDER_H
#define CCPCADDER_H
#include "hist_types.h"
//#include "HistInfo.h"
#include "HistService.h"
//#include "ObjRPC.h"
#include <map>
#include <string>
//#include "CCPCSerializer.h"
//char hist_prefix[]="_HIST_";

typedef std::map<std::string, void*> HistMap;
typedef HistMap::iterator HistIter;

class HistService;
class HistServer;
class ObjRPC;
class HistInfo;
static int empty=-1;

class INServiceDescr
{
public:
  std::string m_serviceName;
  HistInfo *m_Info;
  long long last_update;
  INServiceDescr(char *name, HistInfo* info)
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
  HistService *m_svc;
  void *m_buffer;
  int m_buffersize;
  OUTServiceDescr(char *name, int typ, char *tname)
  {
    switch(typ)
    {
      case H_1DIM:
        {
          m_serviceName = "MonH1D/";
          break;
        }
      case H_2DIM:
        {
          m_serviceName = "MonH2D/";
          break;
        }
      case H_PROFILE:
        {
          m_serviceName = "MonH1P/";
          break;
        }
    }
    m_serviceName += tname;
    m_serviceName += "/";
    m_serviceName += name;
    last_update = 0;
    m_svc = new HistService(m_serviceName.c_str(),"C",&empty,4,&m_buffer,&m_buffersize);
  }
  ~OUTServiceDescr()
  {
    delete m_svc;
  }
};
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

class CCPCAdder
{
protected:
  void *m_buffer;
  int m_buffersize;
  int m_usedSize;
  INServiceMap m_inputServicemap;
  OUTServiceMap m_outputServicemap;
  void TaskName(std::string server, std::string &tname, std::string &tgen);
  void ServiceName(std::string server, std::string &svc);
  std::string m_taskname;
  std::string m_srcnode;
  long long m_reference;
  HistMap m_hmap;
  int m_expected;
  int m_received;
  std::string m_name;
  HistService *m_outservice;
  ObjRPC *m_rpc;
  ObjSerializer *m_ser;
  std::string m_serviceName;
  bool m_expand;
  std::string hist_prefix;

public:
  virtual void add(void *buffer, int siz, HistInfo *h)=0;
  CCPCAdder(){};
  void *Allocate(int siz);
  void *ReAllocate(int);
  INServiceDescr *findINService(std::string);
  OUTServiceDescr *findOUTService(std::string servc);
};
#endif
