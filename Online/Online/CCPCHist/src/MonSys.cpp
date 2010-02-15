#include "MonSys.h"
#include "dis.hxx"
MonSys::MonSys()
{
  Subsyslist.clear();
}
MonSys::~MonSys()
{
}
void MonSys::addSubSys(CCPCSubSys *ss)
{
  Subsyslist.insert(Subsyslist.end(),ss);
  ss->setup((char*)m_name.c_str());
}
void MonSys::start()
{
  DimServer::autoStartOn();
  DimServer::start(m_name.c_str());
  std::vector <CCPCSubSys *>::iterator Iter;
  for ( Iter = Subsyslist.begin( ) ; Iter != Subsyslist.end( ) ; Iter++ )
  {
    CCPCSubSys *ss;
    ss = *Iter;
    ss->start();
  }
}
void MonSys::remSubSys(CCPCSubSys *ss)
{
  std::vector <CCPCSubSys *>::iterator Iter;
  for ( Iter = Subsyslist.begin( ) ; Iter != Subsyslist.end( ) ; Iter++ )
  {
    if (*Iter == ss)
    {
      Subsyslist.erase(Iter);
      break;
    }
  }
}
void MonSys::Lock()
{
  std::vector <CCPCSubSys *>::iterator Iter;
  for ( Iter = Subsyslist.begin( ) ; Iter != Subsyslist.end( ) ; Iter++ )
  {
    CCPCSubSys *ss;
    ss = *Iter;
    ss->Lock();
  }
}
void MonSys::unLock()
{
  std::vector <CCPCSubSys *>::iterator Iter;
  for ( Iter = Subsyslist.begin( ) ; Iter != Subsyslist.end( ) ; Iter++ )
  {
    CCPCSubSys *ss;
    ss = *Iter;
    ss->unLock();
  }
}

MonSys& MonSys::m_instance()  
{
  static MonSys s;
  return s;
}
MonSys *MonSys::setup(char *n)
{
  char *nodename;
#ifdef WIN32
  nodename  = getenv("COMPUTERNAME");
#else
  nodename  = getenv("HOSTNAME");
#endif
  if (nodename != 0)
  {
    m_name  = nodename;
    m_name += "_MON_";
    m_name  += n;
  }
  else
  {
    m_name  = n;
  }
  //m_name = n;
  return this;
}

