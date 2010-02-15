#ifndef ObjService_h
#define ObjService_h
//#include "dis.hxx"
#include "ObjSerializer.h"
//class CCPCHisto;
class ObjService 
{
protected:
  void **m_buffer;
  int *m_buffersize;
  bool m_extBuffer;
  ObjSerializer *m_ser;
  bool m_EORservice;
  virtual void setup(void)=0;
  long long m_hdr;
public:
  //void *p;
  ObjService (){};
  virtual ~ObjService(){};
  //HistService (void *h, const char *name, char *format, void *buff, int siz);
  ObjService (ObjSerializer *,const char *name, char *format, void *buff, int siz);
  ObjService (ObjSerializer *,const char *name, char *format, void *buff, int siz, void **extbuff, int *extbufs);
  ObjService (const char *name, char *format, void *buff, int siz, void **extbuff, int *extbufs);
  virtual void Updater()=0;
  virtual void setEORflag(bool val)=0;
  virtual void setHeader(int hdr)=0;
  virtual void setHeader(long long hdr)=0;
};
#endif
