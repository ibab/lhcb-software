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
  MONSUBSYS_Counter
};
typedef std::map<std::string,MonObj*> ObjMap;

class MonSubSys
{
protected:
  std::string m_name;
  std::string m_pname;
  int bufsiz;
  int buffersize;
  void *buffer;
  int m_numObjs;
  lib_rtl_lock_t m_lockid;
  ObjMap m_Objmap;
  ObjRPC *m_rpc;
  ObjSerializer *m_ser;
  bool start_done;
  int m_lockcnt;
  int m_lockcount;
  int m_unlockcount;
public:
  int m_type;
  std::string m_expandInfix;
  MonSubSys(int intv = 10);
  ~MonSubSys();
  MonTimer *m_updateTimer;
  void *Allocate(int);
  void setup(char *n, bool expand=false);
  void start();
  void addObj(MonObj *h);
  void removeObj(MonObj *h);
  void calcBufferSize();
  MonObj *findobj(const char *name);
  ObjService *m_genSrv;
  ObjService *m_EORsvc;
  int serBufferSize(){return bufsiz;};
  void setRunNo(int runno);
  void Clear(char *name);
  void Clear();
  void EORUpdate(int runo);
  int Lock(void);
  int unLock(void);
  void List();
  void stop();
  int m_runno;
  bool m_expandnames;
};

#endif
