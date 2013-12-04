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

ObjService::ObjService (DimServerDns *dns,ObjSerializer *s,const char *name, char *format, void *buff, int siz)
:DimService(dns,name, format, buff, siz)
{
  setup();
  //m_extBuffer = false;
  //m_buffer=0;
  //m_buffersize =0;
  m_ser = s;
  //m_runnr = 0;
  //m_EORservice = false;
}
ObjService::ObjService (DimServerDns *dns,ObjSerializer *s,const char *name, char *format, void *buff, int siz, void **extbuff, int *extbuffs)
:DimService(dns,name, format, buff, siz)
{
  setup();
  m_extBuffer = true;
  m_buffer = extbuff;
  m_buffersize = extbuffs;
  m_ser = s;
}


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

//ObjService::ObjService (const char *name, char *format, void *buff, int siz, void **extbuff, int *extbuffs)
//:DimService(name, format, buff, siz)
//{
//  setup();
//  m_extBuffer = true;
//  m_buffer = extbuff;
//  m_buffersize = extbuffs;
//}

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
//  m_runnr = 0;
  m_EORservice = false;
}
void ObjService::Serialize()
{
  void *pp;
//  printf("Service Handler...\n");
  if (!m_extBuffer)
  {
    int bs=sizeof(m_hdr);
    //CCPCHSys &s=CCPCHSys::m_instance();
    m_serptr = m_ser->SerializeObj(pp,bs);
    m_hdr.m_magic = SERIAL_MAGIC;
    m_hdr.buffersize = bs-sizeof(m_hdr);
    memcpy(m_serptr,&m_hdr,sizeof(m_hdr));
    m_sersiz = bs;
  }
  else
  {
    m_serptr = *m_buffer;
    m_sersiz = *m_buffersize;
  }
}
void ObjService::Update()
{
  updateService(m_serptr,m_sersiz);
  if (this->m_expandservice)
  {
    m_ser->UpdateExpansions();
  }
}
void ObjService::setEORflag(bool val)
{
  m_EORservice = val;
}
