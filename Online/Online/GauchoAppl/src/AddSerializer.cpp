#include "GauchoAppl/AddSerializer.h"
#include "stdlib.h"
//#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)

AddSerializer::AddSerializer(void)
{
  buffer = 0;
  buffersize = 0;
  m_objmap = 0;
}
AddSerializer::AddSerializer(ObjMap *m)
{
  buffer = 0;
  buffersize = 0;
  m_objmap = m;
}

AddSerializer::~AddSerializer(void)
{
  if (buffer != 0)
  {
    free(buffer);
    buffer = 0;
    buffersize = 0;
  }
}
void *AddSerializer::SerializeObj(void *&ptr, int &siz, bool )
{
  //std::vector <int>::size_type i;
  SysIter i;
  void *pp =0;
  int bs=siz;
  printf("AddSerializer: Dump of Object Map\n");
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    printf("AddSerializer:Histogram %s\n",i->first.c_str());
    SerHist *h = (SerHist*)i->second;
    bs += h->reclen;
    if (pp>h)
    {
      pp=h;
    }
  }
  siz = bs;
  return ptr;
}
void *AddSerializer::SerializeObj(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear)
{
  int bs=siz;
  void *pp;
  for (unsigned int i=0;i<nams.size();i++)
  {
    SerHist *h;
    h = (SerHist*)findhisto(nams[i].c_str());
    if (h != 0)
    {
      bs += h->reclen;
    }
  }
  pp=ptr = Allocate(bs);
  pp = AddPtr(pp,siz);
  siz = bs;
  for (unsigned int i=0;i<nams.size();i++)
  {
    SerHist *h;
    h = (SerHist *)findhisto(nams[i].c_str());
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
void *AddSerializer::SerializeObj(char *name, void *&ptr, int &siz, bool clear)
{
  void *pp;
  int bs=siz;
  SerHist *h = (SerHist*)findhisto(name);
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
void *AddSerializer::SerializeDir(void *&ptr, int &siz)
{
  SysIter i;
  void *pp;
  int bs=siz;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    //printf("%s\n",i->first.c_str());
    SerHist *h = (SerHist*)i->second;
    bs += h->namelength()+1+sizeof(int);
  }
  ptr = Allocate(bs);
  pp = ptr;
  pp = AddPtr(pp,siz);
  siz = bs;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    SerHist *h = (SerHist*)i->second;
    *(int*)pp = h->type;
    pp=AddPtr(pp,sizeof(int));
    pp=h->cpyName(pp);
    *(char*)pp = 0;
    pp=AddPtr(pp,1);
  }
  return ptr;
}
void *AddSerializer::Allocate(int siz)
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
void *AddSerializer::findhisto(const char *nam)
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
    void *h;
    std::string x;
    h = i->second;
    x = i->first;
    return h;
  }
}
void AddSerializer::DeSerializeObj(char *,void *&)
{
}
void AddSerializer::DeSerializeObj(std::vector<std::string> &,void *&)
{
}








