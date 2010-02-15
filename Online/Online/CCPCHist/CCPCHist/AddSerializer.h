#pragma once
#include "ObjSerializer.h"
#include "dimhist.h"
#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)

class SerHist : public DimHistbuff1
{
public:
  int xmitbuffersize()
  {
    return reclen;
  }
  void serialize(void *&p)
  {
    memcpy(p,this,reclen);
    p = AddPtr(p,reclen);
  }
  void clear()
  {
    memset(AddPtr(this,this->dataoff),0,reclen-dataoff);
  }
  int namelength()
  {
    return namelen-1;
  }
  void *cpyName(void *ptr)
  {
    strncpy((char*)ptr,(char*)AddPtr(this,nameoff),namelen-1);
    ptr = (char*)ptr+namelen-1;
    return ptr;
  }

};
#include <map>

class AddSerializer : public ObjSerializer
{
  void *Allocate(int);
  void *findhisto(const char *name);
public:
  AddSerializer(void);
  AddSerializer(ObjMap *);
  virtual ~AddSerializer(void);
  void *SerializeHist(void * &ptr,int &siz, bool clear=false);
  void *SerializeHist(char *name,void * &ptr,int &siz, bool clear=false);
  void *SerializeHist(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear=false);
  void *SerializeDir(void *&ptr, int &siz);
  void DeSerializeHist(char *name,void *&ptr);
  void DeSerializeHist(std::vector<std::string> &nams,void *&ptr);
};
