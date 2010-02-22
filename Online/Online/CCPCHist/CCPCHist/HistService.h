#ifndef HistService_h
#define HistService_h
#include "dis.hxx"
#include "ObjSerializer.h"
#include "ObjService.h"
class CCPCHisto;
class HistService : public ObjService,DimService
{
public:
  //void *p;
  long long m_runnr;
  HistService ();
  virtual ~HistService();
  //HistService (void *h, const char *name, char *format, void *buff, int siz);
  HistService (ObjSerializer *,const char *name, char *format, void *buff, int siz);
  HistService (ObjSerializer *,const char *name, char *format, void *buff, int siz, void **extbuff, int *extbufs);
  HistService (const char *name, char *format, void *buff, int siz, void **extbuff, int *extbufs);
  void Updater();
  void setHeader(int hdr){m_hdr=hdr;};
  void setHeader(long long hdr){m_hdr=hdr;};
  void setEORflag(bool val);
  void setup(void);
};
#endif
