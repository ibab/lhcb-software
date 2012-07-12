#include "Gaucho/MonSys.h"
#include "Gaucho/MonSubSys.h"
#include "dis.hxx"
MonSys::MonSys()
{
  Subsyslist = new std::vector<MonSubSys*>;
  Subsyslist->clear();
  m_state = "Constructed";
}
MonSys::~MonSys()
{
//  std::vector <MonSubSys *>::iterator Iter;
//  if (Subsyslist->size() > 0)
//  {
//    for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
//   {
//      delete *Iter;
//    }
//    Subsyslist->clear();
//  }
  delete Subsyslist;
}
void MonSys::addSubSys(MonSubSys *ss)
{
  Subsyslist->insert(Subsyslist->end(),ss);
//  ss->setup((char*)m_name.c_str());
}
void MonSys::start()
{
  DimServer::setWriteTimeout(20);
  DimServer::autoStartOn();
  DimServer::start(m_name.c_str());
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    MonSubSys *ss;
    ss = *Iter;
    ss->start();
  }
  m_state = "Started";
}
void MonSys::stop()
{
  DimServer::autoStartOff();
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    MonSubSys *ss;
    ss = *Iter;
    ss->stop();
  }
  m_state = "Stopped";

}
void MonSys::remSubSys(MonSubSys *ss)
{
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    if (*Iter == ss)
    {
      Subsyslist->erase(Iter);
      break;
    }
  }
}
void MonSys::Lock()
{
  std::vector <MonSubSys *>::iterator Iter;
//  printf("Monitor System Locking\n");
  int i=0;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
	MonSubSys *ss;
    ss = *Iter;
    ss->Lock();
    i++;
  }
//  printf("+++++++++++++++Monitor sub System LockED %d\n",i);
}
void MonSys::unLock()
{
  std::vector <MonSubSys *>::iterator Iter;
  int i=0;
//  printf("Monitor System Un-Locking\n");
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
	MonSubSys *ss;
    ss = *Iter;
    ss->unLock();
    i++;
  }
//  printf("-------------Monitor sub System Un-LockED %d\n",i);
}

MonSys& MonSys::m_instance()
{
  static MonSys s;
  return s;
}

MonSys *MonSys::setup(char *n)
{
  std::string nodename;
  nodename = RTL::nodeNameShort();
  if (nodename != "")
  {
//    m_name  = nodename;
    m_name = /*std::string("MON_") + */std::string(n);
  }
  else
  {
    m_name  = std::string(n);
  }
  m_state = "Set-up";
  return this;
}
void MonSys::Clear()
{
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    MonSubSys *ss;
    ss = *Iter;
    ss->Clear();
  }
}
void MonSys::EORUpdate(int runo)
{
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    MonSubSys *ss;
    ss = *Iter;
    ss->EORUpdate(runo);
  }
}
void MonSys::List()
{
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    MonSubSys *ss;
    ss = *Iter;
    ss->List();
  }
}
void MonSys::setRunNo(int runo)
{
  std::vector <MonSubSys *>::iterator Iter;
  for ( Iter = Subsyslist->begin( ) ; Iter != Subsyslist->end( ) ; Iter++ )
  {
    MonSubSys *ss;
    ss = *Iter;
    ss->setRunNo(runo);
  }
}



