#include "HistService.h"
#include <vector>
#include "CCPCHsys.h"
#include "iCCPCHist.h"
#ifdef WIN32
#include "windows.h"
#endif
HistService::HistService()
{
};

HistService::~HistService()
{
};

HistService::HistService (ObjSerializer *s,const char *name, char *format, void *buff, int siz)
:DimService(name, format, buff, siz)
{
  setup();
  //m_extBuffer = false;
  //m_buffer=0;
  //m_buffersize =0;
  m_ser = s;
  //m_runnr = 0;
  //m_EORservice = false;
}

HistService::HistService (const char *name, char *format, void *buff, int siz, void **extbuff, int *extbuffs)
:DimService(name, format, buff, siz)
{
  setup();
  m_extBuffer = true;
  m_buffer = extbuff;
  m_buffersize = extbuffs;
}

HistService::HistService (ObjSerializer *s,const char *name, char *format, void *buff, int siz, void **extbuff, int *extbuffs)
:DimService(name, format, buff, siz)
{
  setup();
  m_extBuffer = true;
  m_buffer = extbuff;
  m_buffersize = extbuffs;
  m_ser = s;
}
void HistService::setup()
{
  m_extBuffer = false;
  m_buffer=0;
  m_buffersize =0;
  m_ser = 0;
  m_runnr = 0;
  m_EORservice = false;
}

void HistService::Updater()
{
  void* ptr;
  int  siz;
  void *pp;
  //printf("Service Handler...\n");
  if (!m_extBuffer)
  {
    int bs=sizeof(long long);
    //CCPCHSys &s=CCPCHSys::m_instance();
    ptr = m_ser->SerializeHist(pp,bs);
    if (!m_EORservice)
    {
      *(long long*)ptr = m_hdr;
    }
    else
    {
      *(long long *)ptr = m_hdr;
    }
    siz = bs;
  }
  else
  {
    ptr = *m_buffer;
    siz = *m_buffersize;
  }
  updateService(ptr,siz);
//  setData(ptr,siz);
  return;
}
void HistService::setEORflag(bool val)
{
  m_EORservice = val;
}
//void HistService::setRunnr(int runo)
//{
//   m_runnr= runo;
//}
