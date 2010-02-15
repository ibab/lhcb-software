#include "CCPCSubSys.h"
//#include "HistServer.h"
#include "ObjService.h"
#include "CCPCObj.h"
#include "HistTimer.h"
#include "ObjSerializer.h"
//static int mpty;
typedef std::pair<std::string, CCPCObj*> SysPair;
typedef ObjMap::iterator SysIter;

CCPCSubSys::CCPCSubSys()
{
  //m_serv = new ObjServer();
  bufsiz = 0;
  buffersize = 0;
  m_numObjs = 0;
  m_updateTimer = new HistTimer(this);
}
CCPCSubSys::~CCPCSubSys()
{
  //delete m_updateTimer;
  //delete m_ser;
}
void CCPCSubSys::start()
{
  //DimServer::start(m_name.c_str());
  m_updateTimer->Start();
}
void CCPCSubSys::addObj(CCPCObj* h)
{
  std::string n(h->name());
  m_Objmap.insert(SysPair(n,h));
  m_numObjs++;
//  m_Objmap.push_back(h);
  calcBufferSize();
}
void CCPCSubSys::removeObj(CCPCObj* h)
{
  std::string n(h->name());
  m_Objmap.erase(n);
  calcBufferSize();
  m_numObjs--;
}
void CCPCSubSys::calcBufferSize()
{
//  std::vector <int>::size_type i;
  SysIter i;
  bufsiz = 0;
  for (i =m_Objmap.begin();i!=m_Objmap.end();i++)
  {
    CCPCObj *h = i->second;//m_Objmap[i];
    bufsiz += h->xmitbuffersize();
  }
}
CCPCObj *CCPCSubSys::findobj(const char *nam)
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
    CCPCObj *h;
    std::string x;
    h = i->second;
    x = i->first;
    return h;
  }
}
void *CCPCSubSys::Allocate(int siz)
{
  if (buffersize < siz)
  {
    printf("re-allocaing %d\n",siz);
    free(buffer);
    buffer=malloc(siz);
    buffersize = siz;
  }
  return buffer;
}
void CCPCSubSys::Clear(char *name)
{
  CCPCObj *h =findobj(name);
  if (h != 0)
  {
    h->clear();
  }
}
void CCPCSubSys::EORUpdate(int runo)
{
  m_EORsvc->setHeader(runo);
  m_EORsvc->Updater();
}
int CCPCSubSys::Lock(void)
{
  return lib_rtl_lock(m_lockid);
}
int CCPCSubSys::unLock(void)
{
  return lib_rtl_unlock(m_lockid);
}