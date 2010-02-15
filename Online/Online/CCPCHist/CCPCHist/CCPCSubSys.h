#ifndef CCPCHSUBSYS_H
#define CCPCHSUBSYS_H
#include <vector>
#include <map>
#include <string>
#include "RTL/rtl.h"

//class ObjServer;
class ObjRPC;
class ObjSerializer;
class ObjService;
class ObjServer;
class HistTimer;
class CCPCObj;
typedef std::map<std::string,CCPCObj*> ObjMap;

class CCPCSubSys
{
protected:
  std::string m_name;
  CCPCSubSys();
  ~CCPCSubSys();
  int bufsiz;
  int buffersize;
  void *buffer;
  int m_numObjs;
  HistTimer *m_updateTimer;
  lib_rtl_lock_t m_lockid;
  ObjMap m_Objmap;
  ObjRPC *m_rpc;
  ObjSerializer *m_ser;
public:
  void *Allocate(int);
  virtual void setup(char *n)=0;
  void start();
  void addObj(CCPCObj *h);
  void removeObj(CCPCObj *h);
  void calcBufferSize();
  CCPCObj *findobj(const char *name);
  ObjService *m_genSrv;
  ObjService *m_EORsvc;
  int serBufferSize(){return bufsiz;};
  void Clear(char *name);
  void EORUpdate(int runo);
  int Lock(void);
  int unLock(void);
};

#endif