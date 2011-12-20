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
  setup(H_ILLEGAL,0,"","");
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
MonCounter::MonCounter(const std::string& name, const std::string& title, const float *data, MONTYPE typ )
{
  setup (typ,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const double *data, MONTYPE typ )
{
  setup (typ,data,name,title);
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const int *data , int size)
{
  m_type = C_INTSTAR;
  setup(m_type,data,name,title,size);
  m_contsiz = size;
  buffersize = size;
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const float *data , int size)
{
  m_type = C_FLOATSTAR;
  setup(m_type,data,name,title,size);
  m_contsiz = size;
  buffersize = size;
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const long long *data , int size)
{
  m_type = C_LONGSTAR;
  setup(m_type,data,name,title,size);
  m_contsiz = size;
  buffersize = size;
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const double *data , int size)
{
  m_type = C_DOUBLESTAR;
  setup(m_type,data,name,title,size);
  m_contsiz = size;
  buffersize = size;
}
MonCounter::MonCounter(const std::string& name, const std::string& title, const std::string& fmt, const void *data , int size)
{
  setup(m_type,data,name,title,size);
  char fch = fmt[0];
  fch = tolower(fch);
  switch (fch)
  {
    case 'x':
    {
      m_type = C_LONGSTAR;
      m_contsiz = size;
      buffersize = size;
      break;
    }
    case 'i':
    {
      m_type = C_INTSTAR;
      m_contsiz = size;
      buffersize = size;
      break;
    }
    case 'f':
    {
      m_type = C_FLOATSTAR;
      m_contsiz = size;
      buffersize = size;
      break;
    }
    case 'd':
    {
      m_type = C_DOUBLESTAR;
      m_contsiz = size;
      buffersize = size;
      break;
    }
    default:
    {
      m_type = H_ILLEGAL;
      m_contsiz = 0;
      buffersize = 0;
      break;
    }
  }
  m_fmt = fmt;
}

MonCounter::~MonCounter()
{
  deletePtr(m_service);
}
void MonCounter::setup(MONTYPE typ, const void *data,const std::string& name, const std::string& title,int )
{
  setup(typ,data,name,title);

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
  case C_RATEFLOAT:
    {
      m_contsiz    = 4;
      buffersize = 8;
//      m_srvcprefix = std::string("MonF/");
      break;
    }
  case   C_DOUBLE:
  case C_RATEDOUBLE:
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
  if (m_type == C_VOIDSTAR|| m_type == H_ILLEGAL)
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
  if (m_type == C_VOIDSTAR || m_type == H_ILLEGAL)
  {
    return 0;
  }
  if (m_contents == 0)
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
  case C_FLOAT:
  case C_RATEFLOAT:
    {
      double *dst = (double *)AddPtr(pp,pp->dataoff);
      *dst = *(float*)m_contents;
      break;
    }
  case C_DOUBLE:
  case C_RATEDOUBLE:
    {
      double *dst = (double *)AddPtr(pp,pp->dataoff);
      *dst = *(double*)m_contents;
      break;
    }
  case C_INTSTAR:
  case C_LONGSTAR:
  case C_FLOATSTAR:
  case C_DOUBLESTAR:
  case C_RATEDOUBLESTAR:
  {
    void *dst = AddPtr(pp,pp->dataoff);
    memcpy(dst,m_contents,this->m_contsiz);
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
    case C_RATEFLOAT:
    {
      this->m_service = new DimService(nam.c_str(),*(float*)m_contents);
      break;
    }
    case C_DOUBLE:
    case C_RATEDOUBLE:
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
  h->i_data = 0;
  h->l_data = 0;
  h->f_data = 0.0;
  h->d_data = 0.0;
  h->name = std::string(nam);
  void * dat = AddPtr(p,p->dataoff);
  h->type = p->type;
  switch (p->type)
  {
    case C_INT:
    {
      h->name = nam;
      h->type = p->type;
      int d;
      d = *(int*)dat;
      h->i_data = d;
      break;
    }
    case C_LONGLONG:
    {
      h->name = nam;
      h->type = p->type;
      long d;
      d = *(long*)dat;
      h->l_data = d;
      break;
    }
    case C_FLOAT:
    case C_RATEFLOAT:
    {
      h->name = nam;
      h->type = p->type;
      double dd;
      float d;
      dd = *(double*)dat;
      d = (float)dd;
      h->f_data = d;
      break;
    }
    case C_DOUBLE:
    case C_RATEDOUBLE:
    {
      h->name = nam;
      h->type = p->type;
      double d;
      d = *(double*)dat;
      h->d_data = d;
      break;
    }
    case C_INTSTAR:
    {
      void *pp = AddPtr(p,p->dataoff);
      int bsiz = p->reclen - p->dataoff;
      h->nel = bsiz/sizeof(int);
      h->ptr = malloc(bsiz);
      memcpy(h->ptr,pp,bsiz);
      break;
    }
    case C_LONGSTAR:
    {
      void *pp = AddPtr(p,p->dataoff);
      int bsiz = p->reclen - p->dataoff;
      h->nel = (bsiz)/sizeof(long long);
      h->ptr = malloc(bsiz);
      memcpy(h->ptr,pp,bsiz);
      break;
    }
    case C_FLOATSTAR:
    {
      void *pp = AddPtr(p,p->dataoff);
      int bsiz = p->reclen - p->dataoff;
      h->nel = (bsiz)/sizeof(float);
      h->ptr = malloc(bsiz);
      memcpy(h->ptr,pp,bsiz);
      break;
    }
    case C_DOUBLESTAR:
    case C_RATEDOUBLESTAR:
    {
      void *pp = AddPtr(p,p->dataoff);
      int bsiz = p->reclen - p->dataoff;
      h->nel = (bsiz)/sizeof(double);
      h->ptr = malloc(bsiz);
      memcpy(h->ptr,pp,bsiz);
      break;
    }
  }
  return h;
}
