#ifndef MONHSUBSYS_H
#define MONHSUBSYS_H
#include <vector>
#include <map>
#include <string>
#include "RTL/rtl.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonTimer.h"
#include "Gaucho/ObjSerializer.h"
#include "Gaucho/MonObj.h"
#include "Gaucho/ObjMap.h"
#include "Gaucho/ObjRPC.h"

//class ObjServer;
//class ObjRPC;
//class ObjSerializer;
//class ObjService;
//class ObjServer;
//class MonTimer;
//class MonObj;
enum
{
  MONSUBSYS_Histogram = 1,
  MONSUBSYS_Counter,
  MONSUBSYS_String
};
//typedef std::map<std::string,MonObj*> ObjMap;

class RateMgr;
class MonSubSys
{
public:

  struct _Lock {
    MonSubSys* m_sys;
    _Lock(MonSubSys* sys) : m_sys(sys)
    {
      m_sys->Lock();
    }
    ~_Lock()
    {
      m_sys->unLock();
    }
  };

  std::string m_name;
  std::string m_pname;
//  int bufsiz;
//  int buffersize;
  void *buffer;
  int m_numObjs;
  BRTLLock m_lockid;
  ObjMap m_Objmap;
  ObjRPC *m_rpc;
  ObjSerializer *m_ser;
  ObjSerializer *m_RPCser;
  ObjSerializer *m_EORser;
  bool start_done;
  int m_lockcnt;
  int m_lockcount;
  int m_unlockcount;
public:
  RateMgr *m_RateMgr;
  int m_type;
  bool m_dontclear;
  std::string m_expandInfix;
  MonSubSys(int intv = 10);
  virtual ~MonSubSys();
  MonTimer *m_updateTimer;
//  void *Allocate(int);
  void setup(char *n, bool expand=false);
  void start();
  void addObj(MonObj *h);
  void removeObj(MonObj *h);
//  void calcBufferSize();
  MonObj *findobj(const char *name);
  ObjService *m_genSrv;
  ObjService *m_EORsvc;
//  int serBufferSize(){return bufsiz;};
  void setRunNo(int runno);
  void Clear(char *name);
  void Clear();
  void EORUpdate(int runo);
  void Update();
  void Update(unsigned long ref);
  int Lock(void);
  int unLock(void);
  void List();
  void stop();
  int m_runno;
  bool m_expandnames;
  void removeAll();
  void makeRates(unsigned long long dt);
  void addRateMgr(RateMgr*);
};

#endif
