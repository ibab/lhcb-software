#include "stdlib.h"
#include "Gaucho/ObjSerializer.h"
#include "Gaucho/MonObj.h"
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
//#include "iCCPCHist.h"

ObjSerializer::ObjSerializer(void)
{
  buffer = 0;
  buffersize = 0;
  m_objmap = 0;
}
ObjSerializer::ObjSerializer(ObjMap *m)
{
  buffer = 0;
  buffersize = 0;
  m_objmap = m;
}

ObjSerializer::~ObjSerializer(void)
{
}
void *ObjSerializer::SerializeObj(void *&ptr, int &siz, bool clear)
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
    MonObj *h = (MonObj *)i->second;
    inc = h->xmitbuffersize();
    //printf ("%s lendgth %d\n",h->m_name,inc);
    bs += inc;
    numh++;
  }
  //printf("Number of Histos %d\n",numh);
  ptr = Allocate(bs);
  pp = ptr;
  p1 = AddPtr(pp,bs);
  pp = AddPtr(pp,siz);
  siz = bs;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    MonObj *h = (MonObj *)i->second;
    h->serialize(pp);
//    p1 = AddPtr(ptr,bs);
    if (pp > p1)
    {
      printf("!!!!!!!!!!!!!!! BUFFER OVERFLOW serializing Histogram %s ...\n",h->name());
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
MonObj *ObjSerializer::findobj(const char *nam)
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
    MonObj *h;
    std::string x;
    h = (MonObj *)i->second;
    x = i->first;
    return h;
  }
}
void *ObjSerializer::SerializeObj(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear)
{
  int bs=siz;
  void *pp;
  for (unsigned int i=0;i<nams.size();i++)
  {
    MonObj *h;
    h = this->findobj(nams[i].c_str());
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
    MonObj *h;
    h = this->findobj(nams[i].c_str());
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
void *ObjSerializer::SerializeObj(char *name, void *&ptr, int &siz, bool clear)
{
  void *pp;
  int bs=siz;
  MonObj *h = this->findobj(name);
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
void *ObjSerializer::SerializeDir(void *&ptr, int &siz)
{
  SysIter i;
  void *pp;
  int bs=siz;
  int k;
//  printf("Starting Directory Serialization\n Listing...\n");
  k=0;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    //printf("%s\n",i->first.c_str());
    MonObj *h = (MonObj *)i->second;
    k++;
//    printf("Histogram %d",k);
    h->List();
  }
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    //printf("%s\n",i->first.c_str());
    MonObj *h = (MonObj *)i->second;
    bs += h->namelength()/*+1*/+sizeof(int);
  }
  ptr = Allocate(bs);
  pp = ptr;
  pp = AddPtr(pp,siz);
  siz = bs;
  for (i =m_objmap->begin();i!=m_objmap->end();i++)
  {
    MonObj *h = (MonObj *)i->second;
    *(int*)pp = h->type();
    pp=AddPtr(pp,sizeof(int));
    pp=h->cpyName(pp);
//    printf ("========================Directory Serializer Name: %s\n",h->name());
//        s->m_genSrv->selectiveUpdateService(pp, hs, &clid);
    *(char*)pp = 0;
    pp=AddPtr(pp,1);
  }
//  printf ("========================Directory Serializer Buffer size: %d\n",siz);
  return ptr;
}
void *ObjSerializer::Allocate(int siz)
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
void ObjSerializer::DeSerializeObj(char *,void *&)
{
}
void ObjSerializer::DeSerializeObj(std::vector<std::string> &,void *&)
{
}




