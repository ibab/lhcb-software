#include "CCPCAdder.h"
//#include "hist_types.h"
//#include "HistService.h"
//#include "HistServer.h"
//#include "HistRPC.h"
#include "dimhist.h"
#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)
static int mpty;
typedef std::pair<std::string, CCPCObj*> HistPair;
extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, HistInfo *h)
  {
    CCPCAdder *adder = (CCPCAdder*)tis;
    adder->add(buff, siz,h);
  }
}
void *CCPCAdder::Allocate(int siz)
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
void *CCPCAdder::ReAllocate(int incsiz)
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
        void *hstart = AddPtr(newbuffer,sizeof(long long));
        DimBuffBase *pp = (DimBuffBase*)hstart;
        while (pp<bend)
        {
          char *nam = (char*)AddPtr(pp,pp->nameoff);
          m_hmap.insert(HistPair(std::string(nam),(CCPCObj*)pp));
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
void CCPCAdder::TaskName(std::string server, std::string &tname, std::string &tgen)
{
  int pos;
  std::string spatt;
  spatt = hist_prefix+m_taskname;
  pos = server.find(spatt);
  if (pos != -1)
  {
    tname = server.substr(pos+strlen(hist_prefix.c_str()));
    if (this->m_srcnode != "")
    {
      if (server.find(m_srcnode) == -1)
      {
        tname = "";
        tgen  = "";
        return;
      }
    }
    int atpos = tname.find("@");
    tgen = tname.substr(0,atpos);
    int undpos = tgen.find("_");
    if (undpos != -1)
    {
      tgen = tgen.substr(undpos+1);
    }
    else
    {
      tgen = "";
    }
    std::string s;
    s = tname.substr(0,undpos);
    tname = s;
  }
  else
  {
    tname = "";
    tgen  = "";
  }
}
void CCPCAdder::ServiceName(std::string server, std::string &svc)
{
  int atpos = server.find_first_of("@");
  if (atpos == -1)
  {
    atpos = server.length();
  }
  svc = server.substr(0,atpos)+m_serviceName+std::string("Data");
}
INServiceDescr *CCPCAdder::findINService(std::string servc)
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

OUTServiceDescr *CCPCAdder::findOUTService(std::string servc)
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

