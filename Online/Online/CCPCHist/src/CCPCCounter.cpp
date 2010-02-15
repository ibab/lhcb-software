#include <string.h>
#include <stdlib.h>
#include "dimhist.h"
#include "CCPCCsys.h"
#include "CCPCCounter.h"
#define dimtype double
//#include "CCPCHist.h"
#include "HistService.h"
#include "HistServer.h"

#include <math.h>
#include <iterator>
 
#define MIN( x , y)  =   (( (x) < (y) ) ? (x) : (y) ) 
#define MAX( x , y)  =   (( (x) > (y) ) ? (x) : (y) )

//static DimHistbuff1 dumbuf1;
//static DimHistbuff2 dumbuf2;

void CCPCCounter::setup(HTYPE typ, void *ext,char *name, char *title)
{
  _type  = typ;
  m_titlen = 0;
  m_title = 0;
  m_contsiz    = 0;
  m_addoff = 0;
  m_contents = ext;
  setname(name);
  Init(title);
  return;
}
CCPCCounter::CCPCCounter()
{
  setup(H_ILLEGAL,0,0,0);
//  CCPCHSys::m_instance().add(this);
}
CCPCCounter::CCPCCounter(char *name, char *title, int *data )
{
  setup(C_INT,data,name,title);
  CCPCCSys::m_instance().addObj(this);
}
CCPCCounter::CCPCCounter(char *name, char *title, long long *data )
{
  setup(C_LONGLONG,data,name,title);
  CCPCCSys::m_instance().addObj(this);
}
CCPCCounter::CCPCCounter(char *name, char *title, float *data )
{
  setup(C_FLOAT,data,name,title);
  CCPCCSys::m_instance().addObj(this);
}
CCPCCounter::CCPCCounter(char *name, char *title, double *data )
{
  setup(C_DOUBLE,data,name,title);
  CCPCCSys::m_instance().addObj(this);
}
int CCPCCounter::Init(char *title)
{
  m_titlen = strlen(title);
  m_title = (char*)malloc(m_titlen+1);
  strcpy(m_title,title);
  m_title[m_titlen]  = 0;
  if (_type == C_INT)
  {
    m_contsiz    = 4;
    buffersize = 8;
  }
  else if (_type == C_LONGLONG)
  {
    m_contsiz    = 8;
    buffersize = 8;
  }
  else if (_type == C_FLOAT)
  {
    m_contsiz    = 4;
    buffersize = 8;
  }
  else if (_type == C_DOUBLE)
  {
    m_contsiz    = 8;
    buffersize = 8;
  }
  return 0;
}
CCPCCounter::~CCPCCounter()
{
  ////////CCPCHSys::m_instance().removeObj(this);
  //delete Tserv;
  //delete serv;
  if (m_titlen != 0)
  {
    free(m_title);
    m_titlen  = 0;
  }
  if (m_namelen != 0)
  {
    free(m_name);
    m_namelen = 0;
  }
}
int CCPCCounter::setname ( char* name)
{
  int allocsiz = strlen(name)+1;
  m_name = (char*)malloc(allocsiz);
//  memset(m_name,0,allocsiz);
  strcpy(m_name,name);
  m_namelen = strlen(name);
  m_name[m_namelen]=0;
  return 0;
}
char *CCPCCounter::name()
{
  return m_name;
}
void *CCPCCounter::cpyName(char *ptr)
{
  strncpy(ptr,m_name,this->m_namelen);
  ptr[m_namelen] = 0;
  ptr = (char*)ptr+m_namelen;
  return ptr;
}
void CCPCCounter::clear(void)
{
  if (m_contents != 0)
  {
    memset(m_contents,0,m_contsiz);
  }
  return;
}
//void CCPCCounter::CopyData (float *to, float *c )
//{
//  memcpy(to,c,m_contsiz);
//}
//void CCPCCounter::CopyData (double *to, double *c )
//{
//  memcpy(to,c,m_contsiz);
//}
//void CCPCCounter::CopyData (float *to, double *c )
//{
//  unsigned int i;
//  for (i=0;i<m_contsiz>>3;i++)
//  {
//    to[i] = (float)c[i];
//  }
//}
//void CCPCCounter::CopyData (double *to, float *c )
//{
//  unsigned int i;
//  for (i=0;i<m_contsiz>>2;i++)
//  {
//    to[i] = c[i];
//  }
//}
//int CCPCCounter::get (float *to )
//{
//  CopyData(to, m_contents);
//  return 0;
//}
//int CCPCCounter::get (double *to )
//{
//  CopyData(to, m_contents);
//  return 0;
//}
//int CCPCCounter::getnents ( )
//{
//  return m_nentries;
//}
//int CCPCCounter::geterr (float *to )
//{
//  int i;
//  for (i=0; i<m_nx+2; i++)
//  {
//    to[i]= (float)sqrt(m_contents[i]);
//  }
//  return 0;
//}
//int CCPCCounter::geterr (double *to )
//{
//  int i;
//  for (i=0; i<m_nx+2; i++)
//  {
//    to[i]=sqrt(m_contents[i]);
//  }
//  return 0;
//}
//int CCPCCounter::info (char *title, int *nx, float *xmin, float *xmax, float *bins)
//{
//  strcpy(title,m_title);
//  *nx  = m_nx;
//  *xmin  = m_xmin;
//  *xmax  = m_xmax;
//  *bins  = m_binsx;
//  return 0;
//}
int CCPCCounter::datasize()
{
  return buffersize;
}
int CCPCCounter::hdrlen()
{
  int s;
  s = sizeof(DimBuffBase)+titlen()+1+namelen()+1;
  s = (s + 7)&~7;   //align to 8-byte boundary...
  return s;
}
int CCPCCounter::xmitbuffersize()
{
  int s;
  s = hdrlen();
  s += datasize();
  return s;
}
//int CCPCCounter::info (char *title, int * nx, float *xmin,float *xmax, float *binsx,
//            int * ny, float *ymin,float *ymax, float *binsy)
//{
//  strcpy(title,m_title);
//  *nx  = m_nx;
//  *xmin  = m_xmin;
//  *xmax  = m_xmax;
//  *binsx  = m_binsx;
//  *ny  = m_ny;
//  *ymin  = m_ymin;
//  *ymax  = m_ymax;
//  *binsy  = m_binsy;
//  return 0;
//}
int CCPCCounter::titlen(void)
{
  return (m_titlen+1);//+3)&(~3); //align to 32-bits
}
int CCPCCounter::namelen(void)
{
  return (m_namelen+1);//+3)&(~3); //align to 32 bits
}
void CCPCCounter::cpytitle(char *ptr)
{
  memcpy(ptr,m_title,m_titlen);
  ptr[m_titlen] = 0;
}
int CCPCCounter::serialize(void* &ptr)
{
  int siz;
  int titl = titlen();
  int naml = namelen();
  int hdrl = hdrlen();
  siz = hdrl+datasize();//2*(p->m_nx+2)*sizeof(dimtype);
  DimBuffBase *pp = (DimBuffBase*)ptr;
  pp->dataoff = hdrl;
  pp->addoffset = m_addoff;
  pp->reclen = siz;
  pp->titlen = titl;
  pp->namelen = naml;
  pp->type = _type;
  char *tit;
  char *nam;
  pp->nameoff = sizeof(DimBuffBase);
  pp->titoff  = pp->nameoff+namelen();
  nam = (char*)AddPtr(pp,pp->nameoff);
  tit = (char*)AddPtr(pp,pp->titoff);
  cpyName(nam);
  cpytitle(tit);
  switch (_type)
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
  }
  ptr = (void*)((char*)ptr+siz);
  return siz;
}
//int CCPCCounter::analyze(void *buff, int &dim, char *&tit, int &nentries)
//{
//  return 0;
//}
//int CCPCCounter::analyze(void *buff, int &dim, char *&tit, int &nentries,
//                    int &nx, float &xmin, float &xmax)
//{
//  return 0;
//}
//int CCPCCounter::analyze(void *nuff, int &dim, char *&tit, int &nentries,
//                    int &nx, float &xmin, float &xmax,
//                    int &ny, float &ymin, float &ymax)
//{
//  return 0;
//}
//int CCPCCounter::analyze(void *buff, int &dim, char *&tit, int &nentries,
//                    int &nx, float &xmin, float &xmax,
//                    int &ny, float &ymin, float &ymax,
//                    void *&cont, void *&err)
//{
//  return 0;
//}
//
//
//int CCPCCounter::modify (char *title, int nx, float xmin, float xmax)
//{
//  if (m_titlen != 0)
//  {
//    free(title);
//    m_titlen  = 0;
//  }
//  if (m_contsiz != 0)
//  {
//    free(m_contents);
//    m_contsiz  = 0;
//  }
//  Init(title,nx,xmin,xmax);
//  CCPCHSys::m_instance().calcBufferSize();
//
//  return 0;
//}
//int CCPCCounter::modify (char *title, int nx, float xmin, float xmax,
//              int ny, float ymin, float ymax)
//{
//  if (m_titlen != 0)
//  {
//    free(title);
//    m_titlen  = 0;
//  }
//  if (m_contsiz != 0)
//  {
//    free(m_contents);
//    m_contsiz  = 0;
//  }
//  Init(title,nx,xmin,xmax,ny,ymin,ymax);
//  CCPCHSys::m_instance().calcBufferSize();
//  return 0;
//}
//int CCPCCounter::fill(float x)
//{
//  fill (x,1.0);
//  return 0;
//}
//int CCPCCounter::fill (float x, float weight)
//{
//  int xbinnr;
//  int binnr;
//  if (_type == H_1DIM)
//  {
//    if (x < m_xmin)
//    {
//      xbinnr  = 0;
//    }
//    else if (x>m_xmax)
//    {
//      xbinnr  = m_nx+1;
//    }
//    else
//    {
//      xbinnr  = (int)((x-m_xmin)/m_binsx + 1);
//    }
//    binnr  = xbinnr;
//    m_contents[xbinnr] += weight;
//    m_nentries++;
//    {
//      m_sumw  += weight;
//      double xn = weight*x;
//      m_sumwx += xn;
//      xn *= x;
//      m_sumwx2 += xn;
//      xn  *= x;
//      m_sumwx3 += xn;
//      xn  *= x;
//      m_sumwx4 += xn;
//    }
//    return 0;
//  }
//  if (_type == H_2DIM)
//  {
//    fill(x,(float)weight,1.0);
//    return 0;
//  }
//  else
//  {
//    return 1;
//  }
//}
//int CCPCCounter::fill (float x, float y, float weight)
//{
//  int xbinnr;
//  int ybinnr;
//  int binnr;
//  if (_type == H_2DIM)
//  {
//    if (x < m_xmin)
//    {
//      xbinnr  = 0;
//    }
//    else if (x>m_xmax)
//    {
//      xbinnr  = m_nx+1;
//    }
//    else
//    {
//      xbinnr  = (int)((x-m_xmin)/m_binsx + 1);
//    }
//    if (y < m_ymin)
//    {
//      ybinnr  = 0;
//    }
//    else if (y>m_ymax)
//    {
//      ybinnr  = m_ny+1;
//    }
//    else
//    {
//      ybinnr  = (int)((y-m_ymin)/m_binsy + 1);
//    }
//    binnr  = xbinnr*(m_ny+2)+ybinnr;
//    m_contents[binnr] += weight;
//    m_nentries++;
//    m_sumw  += weight;
//    {
//      double xn = weight*x;
//      m_sumwx += xn;
//      xn *= x;
//      m_sumwx2 += xn;
//      xn  *= x;
//      m_sumwx3 += xn;
//      xn  *= x;
//      m_sumwx4 += xn;
//    }
//    {
//      double yn = weight*y;
//      m_sumwy += yn;
//      yn *= y;
//      m_sumwy2 += yn;
//      yn  *= x;
//      m_sumwy3 += yn;
//      yn  *= x;
//      m_sumwy4 += yn;
//    }
//  }
//  return 0;
//}
//void *CCPCCounter::getextid (void)
//{
//  return extid;
//}
//
bool CCPCCounter::nameeq(char *nam, int namlen)
{
  bool r;
  r = (namlen == m_namelen);
  r = r && (strcmp(m_name,nam) == 0);
  return r;
}

//void CCPCCounter::makedimname(char *name, char **outp)
//{
//  int olen=0;
//  int inc;
//  char *out;
//  inc = strlen ("H1D/");
//  olen += inc;
//  inc = strlen (CCPCHSys::m_instance().m_name.c_str());
//  olen += inc;
//  inc = strlen ("/CCPCAlg/");
//  olen += inc;
//  inc = strlen (name);
//  olen += inc;
//  olen++;
//  out   = (char *)malloc(olen);
//  *outp = out;
//  out[0]=0;
//  if (_type == H_1DIM)
//  {
//    strcpy(out,"H1D/");
//  }
//  else if (_type == H_2DIM)
//  {
//    strcpy(out,"H2D/");
//  }
//  else if (_type == H_PROFILE)
//  {
//    strcpy(out,"P1D/");
//  }
//  strcat(out,CCPCHSys::m_instance().m_name.c_str());
//  strcat(out,"/CCPCAlg/");
//  strcat(out,name);
//
//}
//CSys::CSys()
//  {
//    m_sys = &CCPCCSys::m_instance();
//  }
//
//Histo *CSys::findhisto(char *name)
//{
//  CCPCCounter *h;
//  h = (CCPCCounter *)m_sys->findobj(name);
//  if (h != 0)
//  {
//    return (Histo*)h->getextid();
//  }
//  else
//  {
//    return 0;
//  }
//}
void *cccpc_init(char *nam)
  {
    static int inited=0;
    //static HSys *hsys;
    static CCPCCSys *hsi;
    if (inited == 0)
    {
      DimServer::setWriteTimeout(20);
      hsi=&CCPCCSys::m_instance();
      hsi->setup(nam);
      //hsi->m_serv->autoStartOn();
      //hsi->start();
      inited =1;
    }
    return hsi;
  }

