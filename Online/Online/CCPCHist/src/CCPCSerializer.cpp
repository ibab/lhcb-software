#include "../CCPCHist/CCPCSerializer.h"
#include "iCCPCHist.h"
//typedef std::pair<std::string, CCPCHisto*> SysPair;
//typedef HistMap::iterator SysIter;
//#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)

CCPCSerializer::CCPCSerializer(void)
{
  buffer = 0;
  buffersize = 0;
  m_objmap = 0;
}
CCPCSerializer::CCPCSerializer(ObjMap *m)
{
  buffer = 0;
  buffersize = 0;
  m_objmap = m;
}

CCPCSerializer::~CCPCSerializer(void)
{
}
void *CCPCSerializer::SerializeHist(void *&ptr, int &siz, bool clear)
{
  //std::vector <int>::size_type i;
  SysIter i;
  void *pp;
  void *p1;
  int bs=siz;
  int numh=0;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    int inc;
    CCPCHisto *h = (CCPCHisto *)i->second;
    inc = h->xmitbuffersize();
    //printf ("%s lendgth %d\n",h->m_name,inc);
    bs += inc;
    numh++;
  }
  //printf("Number of Histos %d\n",numh);
  ptr = Allocate(bs);
  pp = ptr;
  pp=AddPtr(pp,siz);
  siz = bs;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    CCPCHisto *h = (CCPCHisto *)i->second;
    h->serialize(pp);
    p1 = AddPtr(ptr,bs);
    if (pp > p1)
    {
      throw;
    }
    if (clear)
    {
      h->clear();
    }
  }
  siz = bs;
  return ptr;
}
void *CCPCSerializer::SerializeHist(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear)
{
  int bs=siz;
  void *pp;
  for (unsigned int i=0;i<nams.size();i++)
  {
    CCPCHisto *h;
    h = findhisto(nams[i].c_str());
    if (h != 0)
    {
      bs += h->xmitbuffersize();
    }
  }
  pp=ptr = Allocate(bs);
  pp = AddPtr(pp,siz);
  siz = bs;
  for (unsigned int i=0;i<nams.size();i++)
  {
    CCPCHisto *h;
    h = findhisto(nams[i].c_str());
    if (h != 0)
    {
      h->serialize(pp);
      if (clear)
      {
        h->clear();
      }
    }
  }
  return ptr;
}
void *CCPCSerializer::SerializeHist(char *name, void *&ptr, int &siz, bool clear)
{
  void *pp;
  int bs=siz;
  CCPCHisto *h = findhisto(name);
  if (h == 0)
  {
    pp = Allocate(siz);
    ptr = pp;
    return ptr;
  }
  bs += h->xmitbuffersize();
  ptr = Allocate(bs);
  pp = ptr;
  pp=AddPtr(pp,siz);
  siz = bs;
  h->serialize(pp);
  if (clear)
  {
    h->clear();
  }
  return ptr;
}
void *CCPCSerializer::SerializeDir(void *&ptr, int &siz)
{
  SysIter i;
  void *pp;
  int bs=siz;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    //printf("%s\n",i->first.c_str());
    CCPCHisto *h = (CCPCHisto *)i->second;
    bs += h->namelength()+1+sizeof(int);
  }
  ptr = Allocate(bs);
  pp = ptr;
  pp = AddPtr(pp,siz);
  siz = bs;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    CCPCHisto *h = (CCPCHisto *)i->second;
    *(int*)pp = h->type();
    pp=AddPtr(pp,sizeof(int));
    pp=h->cpyName(pp);
//        s->m_genSrv->selectiveUpdateService(pp, hs, &clid);
    *(char*)pp = 0;
    pp=AddPtr(pp,1);
  }
  return ptr;
}
void *CCPCSerializer::Allocate(int siz)
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
CCPCHisto *CCPCSerializer::findhisto(const char *nam)
{
  SysIter i;
  //std::vector <int>::size_type i;
  //int namlen;
  std::string n(nam);
  i = m_objmap->find(n);
  if (i == m_objmap->end())
  {
    return 0;
  }
  else
  {
    CCPCHisto *h;
    std::string x;
    h = (CCPCHisto *)i->second;
    x = i->first;
    return h;
  }
}
void CCPCSerializer::DeSerializeHist(char *name,void *&ptr)
{
}
void CCPCSerializer::DeSerializeHist(std::vector<std::string> &nams,void *&ptr)
{
}




