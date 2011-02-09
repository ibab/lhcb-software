#include "Gaucho/ObjService.h"
#include <vector>
#include "Gaucho/MonSys.h"
//#include "iCCPCHist.h"
#ifdef WIN32
#include "windows.h"
#endif
ObjService::ObjService()
{
};

ObjService::~ObjService()
{
};

ObjService::ObjService (ObjSerializer *s,const char *name, char *format, void *buff, int siz)
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

ObjService::ObjService (const char *name, char *format, void *buff, int siz, void **extbuff, int *extbuffs)
:DimService(name, format, buff, siz)
{
  setup();
  m_extBuffer = true;
  m_buffer = extbuff;
  m_buffersize = extbuffs;
}

ObjService::ObjService (ObjSerializer *s,const char *name, char *format, void *buff, int siz, void **extbuff, int *extbuffs)
:DimService(name, format, buff, siz)
{
  setup();
  m_extBuffer = true;
  m_buffer = extbuff;
  m_buffersize = extbuffs;
  m_ser = s;
}
void ObjService::setup()
{
  m_extBuffer = false;
  m_buffer=0;
  m_buffersize =0;
  m_ser = 0;
  m_runnr = 0;
  m_EORservice = false;
}

void ObjService::Updater()
{
  void* ptr;
  int  siz;
  void *pp;
//  printf("Service Handler...\n");
  if (!m_extBuffer)
  {
    int bs=sizeof(m_hdr);
    //CCPCHSys &s=CCPCHSys::m_instance();
    ptr = m_ser->SerializeObj(pp,bs);
    m_hdr.buffersize = bs-sizeof(m_hdr);
    memcpy(ptr,&m_hdr,sizeof(m_hdr));
    siz = bs;
  }
  else
  {
    ptr = *m_buffer;
    siz = *m_buffersize;
  }
  updateService(ptr,siz);
//  setData(ptr,siz);
  if (this->m_expandservice)
  {
    m_ser->UpdateExpansions();
  }
  return;
}
void ObjService::setEORflag(bool val)
{
  m_EORservice = val;
}
//void ObjService::setRunnr(int runo)
//{
//   m_runnr= runo;
//}
