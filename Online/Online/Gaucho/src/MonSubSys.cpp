#include "Gaucho/MonSubSys.h"
//#include "HistServer.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonTimer.h"
#include "Gaucho/ObjSerializer.h"
#include "Gaucho/MonObj.h"
#include "Gaucho/MonSys.h"
#include "Gaucho/MonCounter.h"
#include "Gaucho/RateMgr.h"
//static int mpty;
typedef std::pair<std::string, MonObj*> SysPair;
typedef ObjMap::iterator SysIter;
static long mpty;
MonSubSys::MonSubSys(int intv)
{
  //m_serv = new ObjServer();
//  bufsiz = 0;
//  buffersize = 0;
  m_numObjs = 0;
  start_done = false;
  m_updateTimer = new MonTimer(this,intv);
  char pid[255];
  sprintf(pid,"MON_SUBSYS_%0X_%0lX",lib_rtl_pid(),(unsigned long)this);
  m_lockcnt = 0;
  m_lockcount=0;
  m_unlockcount=0;
//  lib_rtl_create_lock(pid,&m_lockid);
//  BRTL_create_mutex(&m_lockid);
  m_rpc = 0;
  m_ser = 0;
  m_RPCser = 0;
  m_EORser = 0;
  m_genSrv = 0;
  m_EORsvc = 0;
  m_runno = 0;
  m_RateMgr = 0;
  m_dontclear = false;

}
MonSubSys::~MonSubSys()
{
  delete m_updateTimer;
  Lock();
  if (m_expandnames)
  {
    if (m_type == MONSUBSYS_Counter)
    {
      for (SysIter i =m_Objmap.begin();i!=m_Objmap.end();i++)
      {
        MonCounter *h = (MonCounter*)i->second;
        h->delete_OutputService();
      }
    }
  }
  for (SysIter i =m_Objmap.begin();i!=m_Objmap.end();i++)
  {
    MonObj *h = i->second;
    delete h;
  }
  m_Objmap.clear();
  deletePtr(m_rpc);
  deletePtr(m_genSrv);
  deletePtr(m_EORsvc);
  deletePtr(m_ser);
  deletePtr(m_RPCser);
  deletePtr(m_EORser);

  unLock();
  MonSys::m_instance().remSubSys(this);
//  lib_rtl_delete_lock (m_lockid);

}
void MonSubSys::makeRates(unsigned long long dt)
{
  if (m_RateMgr != 0)
  {
    m_RateMgr->makeRates(dt);
  }
}
void MonSubSys::start()
{
  if (!start_done)
  {
    //DimServer::start(m_name.c_str());
//    printf("%s %s: Starting Update Timer\n",m_pname.c_str(),m_name.c_str());
//    ::fflush(stdout);
    m_updateTimer->Start();
    start_done = true;
  }
}
void MonSubSys::stop()
{
  if (start_done)
  {
    //DimServer::start(m_name.c_str());
//    printf("%s %s: Stopping Update Timer\n",m_pname.c_str(),m_name.c_str());
//    ::fflush(stdout);
    m_updateTimer->Stop();
    start_done = false;
  }
}
void MonSubSys::addObj(MonObj* h)
{
  std::string n(h->name());
//  this->Lock();
  m_Objmap.insert(SysPair(n,h));
  m_numObjs++;
//  m_Objmap.push_back(h);
//  calcBufferSize();
//  this->unLock();
}
void MonSubSys::removeAll()
{
  ObjMap::iterator i;
  for (i=m_Objmap.begin();i!= m_Objmap.end();i++)
  {
    delete i->second;
  }
  m_Objmap.clear();
  m_numObjs = 0;
}
void MonSubSys::removeObj(MonObj* h)
{
  if (h == 0) return;
  std::string n(h->name());
//  this->Lock();
  m_Objmap.erase(n);
//  calcBufferSize();
  m_numObjs--;
//  this->unLock();
}
/*
void MonSubSys::calcBufferSize()
{
//  std::vector <int>::size_type i;
  SysIter i;
//  bufsiz = 0;
  for (i =m_Objmap.begin();i!=m_Objmap.end();i++)
  {
    MonObj *h = i->second;//m_Objmap[i];
    bufsiz += h->xmitbuffersize();
  }
}
*/
MonObj *MonSubSys::findobj(const char *nam)
{
  SysIter i;
  //std::vector <int>::size_type i;
  //int namlen;
  std::string n(nam);
  i = m_Objmap.find(n);
  if (i == m_Objmap.end())
  {
    return 0;
  }
  else
  {
    MonObj *h;
    std::string x;
    h = i->second;
    x = i->first;
    return h;
  }
}
/*
 void *MonSubSys::Allocate(int siz)
{
  if (buffersize < siz)
  {
//    printf("re-allocaing %d\n",siz);
    free(buffer);
    buffer=malloc(siz);
    buffersize = siz;
  }
  return buffer;
}
*/
void MonSubSys::setup(char *n, bool expandnames)
{
  m_expandnames = expandnames;
  std::string nam;
  m_name  = n;
  m_pname = RTL::processName();
  std::string nodename;
  nodename = RTL::nodeNameShort();
  nam = /*nodename+"_*/std::string("MON_")+m_pname+"/"+m_name+"/HistCommand";
  if (m_expandnames)
  {
    if (m_type == MONSUBSYS_Counter)
    {
      for (SysIter i =m_Objmap.begin();i!=m_Objmap.end();i++)
      {
        MonCounter *h = (MonCounter*)i->second;
        h->create_OutputService(m_expandInfix);
      }
    }
  }
  if (m_RPCser == 0) m_RPCser = new ObjSerializer(&m_Objmap,m_expandnames);
  if (m_rpc == 0) m_rpc = new ObjRPC(m_RPCser, (char*)nam.c_str(), (char*)"I:1;C",(char*)"C");
  nam = std::string("MON_")+m_pname+"/"+m_name+"/Data";
  m_lockid.m_name = nam;
  if (m_ser == 0) m_ser = new ObjSerializer(&m_Objmap,m_expandnames);
  if ( m_genSrv == 0) m_genSrv = new ObjService(m_ser,(char*)nam.c_str(),(char*)"C",(void*)&mpty, 4);
  if (m_expandnames)
  {
    m_genSrv->m_expandservice = m_expandnames;
  }
  nam = m_name;
  nam = std::string("MON_")+m_pname+"/"+m_name+"/EOR";
  if (m_EORser == 0) m_EORser = new ObjSerializer(&m_Objmap,false);
  if (m_EORsvc == 0) m_EORsvc = new ObjService(m_EORser,(char*)nam.c_str(),(char*)"C",(void*)&mpty,4);
  m_EORsvc->setEORflag(true);
  MonSys::m_instance().addSubSys(this);
//  unLock();
  return;
}

void MonSubSys::Clear(char *name)
{
  if (m_dontclear) return;
  MonObj *h =findobj(name);
  if (h != 0)
  {
    h->clear();
  }
}
void MonSubSys::Clear()
{
  SysIter i;
  for (i =m_Objmap.begin();i!=m_Objmap.end();i++)
  {
    MonObj *h = i->second;//m_Objmap[i];
    h->clear();
  }
}
void MonSubSys::EORUpdate(int runo)
{
  Lock();
  m_EORsvc->setRunNo(runo);
  m_EORsvc->Serialize();
  unLock();
//  dim_lock();
  m_EORsvc->Update();
//  if (m_type == MONSUBSYS_Histogram)
//  {
//    printf("[WARNING]: in EORUpdate: Dump of Object Map\n");
//    m_Objmap.dump();
//  }
//  dim_unlock();
}
void MonSubSys::Update()
{
  Lock();
  m_genSrv->Serialize();
  unLock();
//  dim_lock();
  m_genSrv->Update();
//  dim_unlock();
}
void MonSubSys::Update(unsigned long ref)
{
  Lock();
  long long r;
  r = ref;
  m_genSrv->setTime(r);
  m_genSrv->Serialize();
  unLock();
//  dim_lock();
  m_genSrv->Update();
//  dim_unlock();
}
int MonSubSys::Lock(void)
{
  int status = 1;
  while (status != 0)
  {
//    printf("Locking subsustem %s\n",m_name.c_str());
    status = m_lockid.lockMutex();
    if (status != 0)
    {
//      printf("Status from lock not success......\n");
    }
//    printf("Locking subsustem %s Done...............\n",m_name.c_str());
  }
  m_lockcnt++;
  m_lockcount++;
  if (m_lockcnt != 1)
  {
//    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!Lock Counter != 1 after locking %d %d %d\n",m_lockcnt,m_lockcount,m_unlockcount);
  }
//  printf("+++++++++++Monitor Sub System LockED. Lock Address %0X\n",m_lockid);
  return status;
}
int MonSubSys::unLock(void)
{
//  printf("Monitor Sub System Un-Locking\n");
  int status = 1;
  m_lockcnt--;
  m_unlockcount++;
  while (status != 0)
  {
//    printf("UNLocking subsustem %s\n",m_name.c_str());
    status = m_lockid.unlockMutex();
    if (status != 0)
    {
//      printf("Status from unlock not success......\n");
    }
//    printf("UNLocking subsustem %s Done...............\n",m_name.c_str());
  }
//  printf("-----------Monitor Sub System Un-LockED. Lock Address %0X\n",m_lockid);
  return status;
}
void MonSubSys::List()
{
  SysIter i;
  for (i =m_Objmap.begin();i!=m_Objmap.end();i++)
  {
    MonObj *h = i->second;//m_Objmap[i];
    h->List();
  }
}
void MonSubSys::setRunNo(int runno)
{
  this->m_runno = runno;
  this->m_genSrv->setRunNo(runno);
}
void MonSubSys::addRateMgr(RateMgr* m)
{
  m_RateMgr = m;
  this->m_updateTimer->setExtLastDelta(&m_RateMgr->m_deltaT);
}
