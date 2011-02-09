#include "Gaucho/MonAdder.h"
//#include "hist_types.h"
//#include "HistService.h"
//#include "HistServer.h"
//#include "HistRPC.h"
#include "Gaucho/MonTypes.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/Utilities.h"
//static int mpty;
typedef std::pair<std::string, MonObj*> MonPair;
extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, MonInfo *h)
  {
    MonAdder *adder = (MonAdder*)tis;
    adder->add(buff, siz,h);
  }
}
void *MonAdder::Allocate(int siz)
{
  if (m_buffersize<siz)
  {
    if (m_buffer != 0)
    {
      free (m_buffer);
    }
    m_buffer = malloc(siz);
    if (m_buffer != 0)
    {
      memset(m_buffer,0,siz);
      m_buffersize = siz;
    }
  }
  return m_buffer;
}
void *MonAdder::ReAllocate(int incsiz)
{
  void *newbuffer;
  int newsiz = m_usedSize+incsiz;
  if (m_buffersize<newsiz)
  {
    newbuffer = realloc(m_buffer,newsiz);
    if (newbuffer != 0)
    {
      if (newbuffer != m_buffer)
      {
        m_hmap.clear();
        void *bend = AddPtr(newbuffer,m_usedSize);
        void *hstart = AddPtr(newbuffer,sizeof(SerialHeader));
        DimBuffBase *pp = (DimBuffBase*)hstart;
        while (pp<bend)
        {
          char *nam = (char*)AddPtr(pp,pp->nameoff);
          m_hmap.insert(MonPair(std::string(nam),(MonObj*)pp));
          pp=(DimBuffBase*)AddPtr(pp,pp->reclen);
        }
        m_buffer = newbuffer;
      }
      m_buffersize = newsiz;
    }
    else
    {
      free(m_buffer);
      m_buffersize = 0;
    }
  }
  return m_buffer;
}

void MonAdder::TaskName(std::string &server, std::string &tname, std::string &tgen)
{
  tname = "";
  tgen = "";
  bool status;
  status = boost::regex_search(server,m_taskexp);
  if (status)
  {
    tname = server.substr(0,server.find("@"));
    tgen = "";
  }
  return;
}
void MonAdder::ServiceName(std::string server, std::string &svc)
{
  int atpos = server.find_first_of("@");
  if (atpos == -1)
  {
    atpos = server.length();
  }
  svc = server.substr(0,atpos)+"/histos/"+m_serviceName;
}
INServiceDescr *MonAdder::findINService(std::string servc)
{
  INServIter i;
  i = m_inputServicemap.find(servc);
  if (i != m_inputServicemap.end())
  {
    INServiceDescr *sd = i->second;
    return sd;
  }
  else
  {
    return 0;
  }
}

OUTServiceDescr *MonAdder::findOUTService(std::string servc)
{
  OUTServIter i;
  i = m_outputServicemap.find(servc);
  if (i != m_outputServicemap.end())
  {
    OUTServiceDescr *sd = i->second;
    return sd;
  }
  else
  {
    return 0;
  }
}

