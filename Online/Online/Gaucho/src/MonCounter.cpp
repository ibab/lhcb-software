#include <string.h>
#include <stdlib.h>
#include "Gaucho/dimhist.h"
#include "Gaucho/MonCounter.h"

#include <math.h>
#include <iterator>

#define MIN( x , y)  =   (( (x) < (y) ) ? (x) : (y) )
#define MAX( x , y)  =   (( (x) > (y) ) ? (x) : (y) )


MonCounter::MonCounter()
{
  setup(H_ILLEGAL,0,0,0);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const int *data )
{
  setup(C_INT,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const long *data )
{
  setup(C_LONGLONG,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const long long *data )
{
  setup(C_LONGLONG,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const float *data )
{
  setup(C_FLOAT,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const double *data )
{
  setup(C_DOUBLE,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const std::string& fmt, const void *data , int size)
{
  setup(C_VOIDSTAR,data,name,title);
  m_fmt = fmt;
  m_contsiz = size;
  buffersize = 0;
}

MonCounter::~MonCounter()
{
  deletePtr(m_service);
}

void MonCounter::setup(MONTYPE typ, const void *data,const std::string& name, const std::string& title)
{
  m_type  = typ;
  m_title = "";
  m_contsiz    = 0;
  m_addoff = 0;
  m_contents = data;
  m_service = 0;
  m_name = name;
  m_title = title;
  m_srvcprefix = "";
  switch (m_type)
  {
  case   C_INT:
    {
      m_contsiz    = 4;
      buffersize = 8;
//      m_srvcprefix = std::string("MonI/");
      break;
    }
  case   C_LONGLONG:
    {
      m_contsiz    = 8;
      buffersize = 8;
//      m_srvcprefix = std::string("MonL/");
      break;
    }
  case   C_FLOAT:
    {
      m_contsiz    = 4;
      buffersize = 8;
//      m_srvcprefix = std::string("MonF/");
      break;
    }
  case   C_DOUBLE:
    {
      m_contsiz    = 8;
      buffersize = 8;
//      m_srvcprefix = std::string("MonD/");
      break;
    }
  case   C_VOIDSTAR:
    {
      m_srvcprefix = "";
      break;
    }
  default:
    {
      break;
    }
  }
  return;
}

void *MonCounter::cpyName(void *p) const
{
  ::memcpy(p, m_name.c_str(), m_name.length()+1);
  return AddPtr(p,m_name.length());
}

void MonCounter::clear(void)
{
  if (m_contents)
  {
    ::memset((void*)m_contents,0,m_contsiz);
  }
}

int MonCounter::hdrlen()  const
{
  int s = sizeof(DimBuffBase)+titlen()+1+namelen()+1;
  s = (s + 7)&~7;   //align to 8-byte boundary...
  return s;
}

int MonCounter::xmitbuffersize()
{
  if (m_type == C_VOIDSTAR)
  {
    return 0;
  }
  int s = hdrlen();
  s += datasize();
  return s;
}

void *MonCounter::cpytitle(void *p) const
{
  memcpy(p,m_title.c_str(),m_title.length()+1);
  return AddPtr(p,m_title.length());
}

int MonCounter::serialize(void* &ptr)
{
  if (m_type == C_VOIDSTAR)
  {
    return 0;
  }
  int titl = titlen();
  int naml = namelen();
  int hdrl = hdrlen();
  int siz = hdrl+datasize();
  DimBuffBase *pp = (DimBuffBase*)ptr;
  pp->dataoff = hdrl;
  pp->addoffset = m_addoff;
  pp->reclen = siz;
  pp->titlen = titl;
  pp->namelen = naml;
  pp->type = m_type;
  char *tit;
  char *nam;
  pp->nameoff = sizeof(DimBuffBase);
  pp->titoff  = pp->nameoff+namelen();
  nam = (char*)AddPtr(pp,pp->nameoff);
  tit = (char*)AddPtr(pp,pp->titoff);
  cpyName(nam);
  cpytitle(tit);
  switch (m_type)
  {
  case   C_INT:
    {
      long long *dst = (long long *)AddPtr(pp,pp->dataoff);
      *dst = *(int*)m_contents;
      break;
    }
  case   C_LONGLONG:
    {
      long long *dst = (long long *)AddPtr(pp,pp->dataoff);
      *dst = *(long long*)m_contents;
      break;
    }
  case   C_FLOAT:
    {
      double *dst = (double *)AddPtr(pp,pp->dataoff);
      *dst = *(float*)m_contents;
      break;
    }
  case   C_DOUBLE:
    {
      double *dst = (double *)AddPtr(pp,pp->dataoff);
      *dst = *(double*)m_contents;
      break;
    }
  default:
    {
      break;
    }
  }
  ptr = (void*)((char*)ptr+siz);
  return siz;
}

void MonCounter::List()
{
  m_srvcprefix = "";
  std::string typ;
  switch (m_type)
  {
  case   C_INT:
    {
      typ = "C_INT";
      break;
    }
  case   C_LONGLONG:
    {
      typ = "C_LONGLONG";
      break;
    }
  case   C_FLOAT:
    {
      typ = "C_FLOAT";
      break;
    }
  case   C_DOUBLE:
    {
      typ = "C_DOUBLE";
      break;
    }
  case   C_VOIDSTAR:
    {
      typ = "C_VOIDSTAR";
      break;
    }
  default:
    {
//      typ = "UNKNOWN";
      break;
    }
  }
//  printf("Counter Type: %s Name %s\n",typ.c_str(),name());
}
void MonCounter::create_OutputService(std::string infix)
{
  std::string nam;
  if (m_service != 0) return;
  nam = m_srvcprefix+infix+std::string(m_name);
  if (nam.length()>128 /* maximum length of a DIM service Name*/)
  {
    m_service = 0;
    return;
  }
  switch(m_type)
  {
    case C_INT:
    {
      this->m_service = new DimService(nam.c_str(),*(int*)m_contents);
      break;
    }
    case C_LONGLONG:
    {
      this->m_service = new DimService(nam.c_str(),*(long long*)m_contents);
      break;
    }
    case C_FLOAT:
    {
      this->m_service = new DimService(nam.c_str(),*(float*)m_contents);
      break;
    }
    case C_DOUBLE:
    case C_RATE:
    {
      this->m_service = new DimService(nam.c_str(),*(double*)m_contents);
      break;
    }
    case C_VOIDSTAR:
    {
      this->m_service = new DimService(nam.c_str(),(char*)m_fmt.c_str(),(void*)m_contents, m_contsiz);
      break;
    }
    default:
    {
      m_service = 0;
      break;
    }
  }
  return;
}

void MonCounter::delete_OutputService()
{
  deletePtr(m_service);
}

DimService *MonCounter::getDimService()  const
{
  return m_service;
}

void *MonCounter::de_serialize(void *ptr, char *nam)
{
  DimBuffBase *p = (DimBuffBase*)ptr;
//  this->m_type = (MONTYPE)p->type;
  if (nam == 0)
  {
    nam = (char*)AddPtr(p,p->nameoff);
  }
  CntrDescr *h = new CntrDescr;
  h->data = 0;
  void * dat = AddPtr(p,p->dataoff);
  switch (p->type)
  {
    case C_INT:
    {
      h->name = nam;
      h->type = p->type;
      h->data = *(int*)dat;
      break;
    }
    case C_LONGLONG:
    {
      h->name = nam;
      h->type = p->type;
      h->data = *(long long*)dat;
      break;
    }
    case C_FLOAT:
    {
      h->name = nam;
      h->type = p->type;
      float *a = (float*)&h->data;
      *a = *(float*)dat;
      break;
    }
    case C_DOUBLE:
    case C_RATE:
    {
      h->name = nam;
      h->type = p->type;
      double *a = (double*)&h->data;
      *a = *(double*)dat;
      break;
    }
  }
  return h;
}

