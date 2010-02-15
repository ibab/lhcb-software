#ifndef CCPCOBJ_H
#define CCPCOBJ_H
class CCPCObj
{
  public:
  virtual int datasize()=0;
  virtual int xmitbuffersize()=0;
  virtual int hdrlen()=0;
  virtual int serialize(void* &ptr)=0;
  virtual char *name()=0;
  virtual void clear(void)=0;
};

#endif