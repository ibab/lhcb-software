#ifndef MONOBJ_H
#define MONOBJ_H
class MonObj
{
  public:
  virtual int datasize()=0;
  virtual int xmitbuffersize()=0;
  virtual int hdrlen()=0;
  virtual int serialize(void* &ptr)=0;
  virtual char *name()=0;
  virtual void clear(void)=0;
  virtual int namelength()=0;
  virtual int type()=0;
  virtual void *cpyName(void*)=0;
  virtual void *cpytitle(void *ptr)=0;
  virtual void List()=0;
//  virtual void *de_serialize(void *ptr, char *nam=0)=0;
};

#endif
