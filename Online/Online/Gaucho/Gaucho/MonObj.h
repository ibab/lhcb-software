#ifndef ONLINE_GAUCHO_MONOBJ_H
#define ONLINE_GAUCHO_MONOBJ_H

class DimService;

class MonObj
{
  public:
  virtual ~MonObj(){}
  virtual int datasize() const =0;
  virtual int xmitbuffersize() =0;
  virtual int hdrlen() const =0;
  virtual int serialize(void* &ptr)=0;
  virtual const char *name() const=0;
  virtual void clear(void)=0;
  virtual int namelength() const =0;
  virtual int type() const =0;
  virtual void *cpyName(void*) const =0;
  virtual void *cpytitle(void *ptr) const =0;
  virtual void List()=0;
  virtual DimService *getDimService() const { return 0; }
};

#endif  // ONLINE_GAUCHO_MONOBJ_H
