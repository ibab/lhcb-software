#include <string.h>
#include <stdlib.h>
#include "dimhist.h"
#include "CCPCHsys.h"
#include "iCCPCHist.h"
#define dimtype double
#include "CCPCHist.h"
#include "HistService.h"
#include "HistServer.h"

#include <math.h>
#include <iterator>
 
#define MIN( x , y)  =   (( (x) < (y) ) ? (x) : (y) ) 
#define MAX( x , y)  =   (( (x) > (y) ) ? (x) : (y) )

//static DimHistbuff1 dumbuf1;
//static DimHistbuff2 dumbuf2;

void CCPCHisto::setup(HTYPE typ, Histo *ext,char *name, char *title, 
                     int nx, float xmin, float xmax, 
                     int ny, float ymin, float ymax )
{
  _type  = typ;
  m_nentries  = 0;
  m_nx  = 0;
  m_xmin  = 0.0;
  m_xmax  = 0.0;
  m_binsx  = 0.0;
  m_ny  = 0;
  m_ymin  = 0.0;
  m_ymax  = 0.0;
  m_binsy  = 0.0;
  m_titlen = 0;
  m_title = 0;
  m_contsiz    = 0;
  m_contents  = 0;
  m_dimservname  = 0;
  m_Tdimservname  = 0;
  m_sumw = 0.0;
  m_sumwx = 0.0;
  m_sumwx2 = 0.0;
  m_sumwx3 = 0.0;
  m_sumwx4 = 0.0;
  m_sumwy = 0.0;
  m_sumwy2 = 0.0;
  m_sumwy3 = 0.0;
  m_sumwy4 = 0.0;
  m_addoff = 0;

  extid = ext;
  if (nx == 0) return;
  setname(name);
  Init(title,nx,xmin,xmax, ny, ymin, ymax);
  makedimname(name,&m_dimservname);
//  serv = new HistService (this, m_dimservname,"F", &dumbuf1,sizeof(dumbuf1));
  int l = strlen(m_dimservname)+strlen("/gauchocomment")+1;
  m_Tdimservname  = (char*)malloc(l);
  strcpy(m_Tdimservname,m_dimservname);
  strcat(m_Tdimservname,"/gauchocomment");
  m_Tdimservname[l-1] = 0;
  //Tserv = new DimService(m_Tdimservname,m_title);
  return;
}
CCPCHisto::CCPCHisto()
{
  setup(H_ILLEGAL, 0,"","",0,0.0,0.0,0,0.0,0.0);
//  CCPCHSys::m_instance().add(this);
}
CCPCHisto::CCPCHisto(Histo *ext)
{
  setup(H_ILLEGAL, ext,"","",0,0.0,0.0,0,0.0,0.0);
//  CCPCHSys::m_instance().add(this);
}
CCPCHisto::CCPCHisto(char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_1DIM, 0,name, title, nx, xmin, xmax,0, 0.0,0.0); 
  CCPCHSys::m_instance().addObj(this);
}
CCPCHisto::CCPCHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_1DIM, ext, name, title, nx, xmin, xmax,0, 0.0,0.0); 
  CCPCHSys::m_instance().addObj(this);
}
CCPCHisto::CCPCHisto(char *name, char *title, int nx, float xmin, float xmax, 
          int ny, float ymin, float ymax )
{
  setup (H_2DIM, 0, name, title, nx, xmin, xmax, ny, ymin, ymax );
  CCPCHSys::m_instance().addObj(this);
}
CCPCHisto::CCPCHisto(Histo *ext, char *name, char *title, 
                     int nx, float xmin, float xmax, 
                     int ny, float ymin, float ymax )
{
  setup (H_2DIM, ext, name, title, nx, xmin, xmax, ny, ymin, ymax );
  CCPCHSys::m_instance().addObj(this);
}
int CCPCHisto::Init(char *title, int nx, float xmin, float xmax )
{
  Init(title, nx, xmin, xmax, 0, 0.0, 0.0);
  return 0;
}
int CCPCHisto::Init(char *title, int nx, float xmin, float xmax, 
          int ny, float ymin, float ymax )
{
  m_nentries  = 0;
  m_nx  = nx;
  m_xmin  = xmin;
  m_xmax  = xmax;
  m_binsx = 0.0;
  if (nx>0)
  {
    m_binsx  = (m_xmax-m_xmin)/m_nx;
  }
  m_ny  = ny;
  m_ymin  = ymin;
  m_ymax  = ymax;
  m_binsy = 0.0;
  if (m_ny >0)
  {
    m_binsy  = (m_ymax-m_ymin)/m_ny;
  }
  m_titlen = strlen(title);
  m_title = (char*)malloc(m_titlen+1);
  strcpy(m_title,title);
  m_title[m_titlen]  = 0;
  m_contents  = 0;
  if (_type == H_2DIM)
  {
    m_contsiz    = (ny+2)*(nx+2)*sizeof(bintype);
    m_contents  = (bintype*)malloc(m_contsiz);
    buffersize = 2*(m_nx+2)*(m_ny+2)*sizeof(dimtype);
  }
  else if (_type == H_1DIM)
  {
    m_contsiz    = (nx+2)*sizeof(bintype);
    m_contents  = (bintype*)malloc(m_contsiz);
    buffersize = 2*(m_nx+2)*sizeof(dimtype);
  }
  else if (_type == H_PROFILE)
  {
    m_contsiz    = (nx+2)*sizeof(bindesc);
    m_contents  = (bintype*)malloc(m_contsiz);
    buffersize = 3*(m_nx+2)*sizeof(dimtype);//+(m_nx+2)*sizeof(int);
  }
  if (m_contents != 0)
  {
    memset(m_contents,0,m_contsiz);
  }
  m_dimservname   = 0;
  return 0;
}
CCPCHisto::~CCPCHisto()
{
  CCPCHSys::m_instance().removeObj(this);
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
  if (m_contsiz != 0)
  {
    free(m_contents);
    m_contsiz  = 0;
  }
  if (m_dimservname != 0)
  {
    free (m_dimservname);
    m_dimservname = 0;
  }
  if (m_Tdimservname != 0)
  {
    free (m_Tdimservname);
    m_Tdimservname = 0;
  }
}
int CCPCHisto::setname ( char* name)
{
  int allocsiz = strlen(name)+1;
  m_name = (char*)malloc(allocsiz);
//  memset(m_name,0,allocsiz);
  strcpy(m_name,name);
  m_namelen = strlen(name);
  m_name[m_namelen]=0;
  return 0;
}
char *CCPCHisto::name()
{
  return m_name;
}
void *CCPCHisto::cpyName(void *ptr)
{
  strncpy((char*)ptr,m_name,this->m_namelen);
  ptr = (char*)ptr+m_namelen;
  return ptr;
}
void CCPCHisto::clear(void)
{
  m_nentries  = 0;
  m_sumw = 0.0;
  m_sumwx = 0.0;
  m_sumwx2 = 0.0;
  m_sumwx3 = 0.0;
  m_sumwx4 = 0.0;
  m_sumwy = 0.0;
  m_sumwy2 = 0.0;
  m_sumwy3 = 0.0;
  m_sumwy4 = 0.0;

  if (m_contents != 0)
  {
    memset(m_contents,0,m_contsiz);
  }
  return;
}
int CCPCHisto::put (float *from )
{
  memcpy(m_contents,from,m_contsiz);
  return 0;
}
int CCPCHisto::put (double *from )
{
  memcpy(m_contents,from,m_contsiz);
  return 0;
}
int CCPCHisto::putnents (int ne )
{
  m_nentries  = ne;
  return 0;
}
void CCPCHisto::CopyData (float *to, float *c )
{
  memcpy(to,c,m_contsiz);
}
void CCPCHisto::CopyData (double *to, double *c )
{
  memcpy(to,c,m_contsiz);
}
void CCPCHisto::CopyData (float *to, double *c )
{
  unsigned int i;
  for (i=0;i<m_contsiz>>3;i++)
  {
    to[i] = (float)c[i];
  }
}
void CCPCHisto::CopyData (double *to, float *c )
{
  unsigned int i;
  for (i=0;i<m_contsiz>>2;i++)
  {
    to[i] = c[i];
  }
}
int CCPCHisto::get (float *to )
{
  CopyData(to, m_contents);
  return 0;
}
int CCPCHisto::get (double *to )
{
  CopyData(to, m_contents);
  return 0;
}
int CCPCHisto::getnents ( )
{
  return m_nentries;
}
int CCPCHisto::geterr (float *to )
{
  int i;
  for (i=0; i<m_nx+2; i++)
  {
    to[i]= (float)sqrt(m_contents[i]);
  }
  return 0;
}
int CCPCHisto::geterr (double *to )
{
  int i;
  for (i=0; i<m_nx+2; i++)
  {
    to[i]=sqrt(m_contents[i]);
  }
  return 0;
}
int CCPCHisto::info (char *title, int *nx, float *xmin, float *xmax, float *bins)
{
  strcpy(title,m_title);
  *nx  = m_nx;
  *xmin  = m_xmin;
  *xmax  = m_xmax;
  *bins  = m_binsx;
  return 0;
}
int CCPCHisto::datasize()
{
  return buffersize;
}
int CCPCPHisto::datasize()
{
  return buffersize;
}
int CCPCHisto::hdrlen()
{
  int s;
  switch (_type)
  {
  case   H_1DIM:
    {
      s = sizeof(DimHistbuff1)+titlen()+1+namelen()+1;
      break;
    }
  case   H_2DIM:
    {
      s = sizeof(DimHistbuff2)+titlen()+1+namelen()+1;
      break;
    }
  default:
    {
      s = 0;
      break;
    }
  }
  s = (s + 7)&~7;   //align to 8-byte boundary...
  return s;
}
int CCPCHisto::xmitbuffersize()
{
  int s;
  s = hdrlen();
  s += datasize();
  return s;
}
int CCPCPHisto::hdrlen()
{
  int s = sizeof(DimHistbuff1)+titlen()+1+namelen()+1;
  s = (s + 7)&~7;   //align to 8-byte boundary...
  return s;
}
int CCPCPHisto::xmitbuffersize()
{
  int s;
  s = hdrlen();
//  sizeof(DimHistbuff1)+titlen()+1+namelen()+1;
  s +=datasize();//2*(p->m_nx+2)*sizeof(dimtype);
  return s;
}
int CCPCHisto::info (char *title, int * nx, float *xmin,float *xmax, float *binsx,
            int * ny, float *ymin,float *ymax, float *binsy)
{
  strcpy(title,m_title);
  *nx  = m_nx;
  *xmin  = m_xmin;
  *xmax  = m_xmax;
  *binsx  = m_binsx;
  *ny  = m_ny;
  *ymin  = m_ymin;
  *ymax  = m_ymax;
  *binsy  = m_binsy;
  return 0;
}
int CCPCHisto::titlen(void)
{
  return (m_titlen+1);//+3)&(~3); //align to 32-bits
}
int CCPCHisto::namelen(void)
{
  return (m_namelen+1);//+3)&(~3); //align to 32 bits
}
void CCPCHisto::cpytitle(char *ptr)
{
  memcpy(ptr,m_title,m_titlen);
  ptr[m_titlen] = 0;
}
int CCPCHisto::serialize(void* &ptr)
{
  int siz;
  int status;
  switch (_type)
  {
  case   H_1DIM:
    {
      int titl = titlen();
      int naml = namelen();
      int hdrl = hdrlen();
      siz = hdrl+datasize();//2*(p->m_nx+2)*sizeof(dimtype);
      DimHistbuff1 *pp = (DimHistbuff1*)ptr;
      pp->dataoff = hdrl;
      pp->addoffset = m_addoff;
      pp->reclen = siz;
      pp->titlen = titl;
      pp->namelen = naml;
      pp->nentries = (float)getnents();
      pp->dim = 1;
      pp->type = H_1DIM;
      pp->m_sumw  = m_sumw;
      pp->m_sumw2 = m_sumw2;
      pp->m_sumwx = m_sumwx;
      pp->m_sumwx2  = m_sumwx2;
      pp->m_sumwx3  = m_sumwx3;
      pp->m_sumwx4  = m_sumwx4;
      pp->m_sumwy = m_sumwy;
      pp->m_sumwy2  = m_sumwy2;
      pp->m_sumwy3  = m_sumwy3;
      pp->m_sumwy4  = m_sumwy4;
      pp->nxbin = m_nx;
      pp->xmin  = m_xmin;
      pp->xmax  = m_xmax;
      char *tit;
      char *nam;
      dimtype *ntries;
      pp->nameoff = sizeof(DimHistbuff1);
      pp->titoff  = pp->nameoff+namelen();
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      cpyName(nam);
      cpytitle(tit);
      ntries = (dimtype*)AddPtr(ptr,pp->dataoff);
      get(ntries);
      dimtype *errp;
      errp  = (ntries)+(m_nx+2);
      geterr(errp);
      status  = 0;
      break;
    }  
  case H_2DIM:
    {
      int titl = titlen();
      int naml = namelen();
      int hdrl = hdrlen();//sizeof(DimHistbuff2)+titl+1+naml+1;
      siz = hdrl+datasize();//2*(p->m_nx+2)*sizeof(dimtype);
      DimHistbuff2 *pp = (DimHistbuff2*)ptr;
      pp->dataoff = hdrl;
      pp->reclen = siz;
      pp->titlen = titl;
      pp->namelen = naml;
      pp->addoffset = m_addoff;
      pp->nentries = (float)getnents();
      pp->nxbin = m_nx;
      pp->xmin  = m_xmin;
      pp->xmax  = m_xmax;
      pp->dim = 2;
      pp->type = H_2DIM;
      pp->nxbin = m_nx;
      pp->xmin  = m_xmin;
      pp->xmax  = m_xmax;
      pp->nybin = m_ny;
      pp->ymin  = m_ymin;
      pp->ymax  = m_ymax;
      pp->m_sumw  = m_sumw;
      pp->m_sumw2 = m_sumw2;
      pp->m_sumwx = m_sumwx;
      pp->m_sumwx2  = m_sumwx2;
      pp->m_sumwx3  = m_sumwx3;
      pp->m_sumwx4  = m_sumwx4;
      pp->m_sumwy = m_sumwy;
      pp->m_sumwy2  = m_sumwy2;
      pp->m_sumwy3  = m_sumwy3;
      pp->m_sumwy4  = m_sumwy4;
      char *tit;
      char *nam;
      dimtype *ntries;
      pp->nameoff = sizeof(DimHistbuff2);
      pp->titoff  = pp->nameoff+namelen();
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      cpyName(nam);
      cpytitle(tit);
      ntries = (dimtype*)AddPtr(ptr,pp->dataoff);
      get(ntries);
      dimtype *errp;
      errp  = (ntries)+(m_nx+2);
      geterr(errp);
      status  = 0;
      break;
    }  
  default:
    {
      status  = -2;
      siz = 0;
      break;
    }
  }
  ptr = (void*)((char*)ptr+siz);
  return siz;
}
int CCPCHisto::analyze(void *buff, int &dim, char *&tit, int &nentries)
{
  return 0;
}
int CCPCHisto::analyze(void *buff, int &dim, char *&tit, int &nentries,
                    int &nx, float &xmin, float &xmax)
{
  return 0;
}
int CCPCHisto::analyze(void *nuff, int &dim, char *&tit, int &nentries,
                    int &nx, float &xmin, float &xmax,
                    int &ny, float &ymin, float &ymax)
{
  return 0;
}
int CCPCHisto::analyze(void *buff, int &dim, char *&tit, int &nentries,
                    int &nx, float &xmin, float &xmax,
                    int &ny, float &ymin, float &ymax,
                    void *&cont, void *&err)
{
  return 0;
}


int CCPCHisto::modify (char *title, int nx, float xmin, float xmax)
{
  if (m_titlen != 0)
  {
    free(title);
    m_titlen  = 0;
  }
  if (m_contsiz != 0)
  {
    free(m_contents);
    m_contsiz  = 0;
  }
  Init(title,nx,xmin,xmax);
  CCPCHSys::m_instance().calcBufferSize();

  return 0;
}
int CCPCHisto::modify (char *title, int nx, float xmin, float xmax,
              int ny, float ymin, float ymax)
{
  if (m_titlen != 0)
  {
    free(title);
    m_titlen  = 0;
  }
  if (m_contsiz != 0)
  {
    free(m_contents);
    m_contsiz  = 0;
  }
  Init(title,nx,xmin,xmax,ny,ymin,ymax);
  CCPCHSys::m_instance().calcBufferSize();
  return 0;
}
int CCPCHisto::fill(float x)
{
  fill (x,1.0);
  return 0;
}
int CCPCHisto::fill (float x, float weight)
{
  int xbinnr;
  int binnr;
  if (_type == H_1DIM)
  {
    if (x < m_xmin)
    {
      xbinnr  = 0;
    }
    else if (x>m_xmax)
    {
      xbinnr  = m_nx+1;
    }
    else
    {
      xbinnr  = (int)((x-m_xmin)/m_binsx + 1);
    }
    binnr  = xbinnr;
    m_contents[xbinnr] += weight;
    m_nentries++;
    {
      m_sumw  += weight;
      double xn = weight*x;
      m_sumwx += xn;
      xn *= x;
      m_sumwx2 += xn;
      xn  *= x;
      m_sumwx3 += xn;
      xn  *= x;
      m_sumwx4 += xn;
    }
    return 0;
  }
  if (_type == H_2DIM)
  {
    fill(x,(float)weight,1.0);
    return 0;
  }
  else
  {
    return 1;
  }
}
int CCPCHisto::fill (float x, float y, float weight)
{
  int xbinnr;
  int ybinnr;
  int binnr;
  if (_type == H_2DIM)
  {
    if (x < m_xmin)
    {
      xbinnr  = 0;
    }
    else if (x>m_xmax)
    {
      xbinnr  = m_nx+1;
    }
    else
    {
      xbinnr  = (int)((x-m_xmin)/m_binsx + 1);
    }
    if (y < m_ymin)
    {
      ybinnr  = 0;
    }
    else if (y>m_ymax)
    {
      ybinnr  = m_ny+1;
    }
    else
    {
      ybinnr  = (int)((y-m_ymin)/m_binsy + 1);
    }
    binnr  = xbinnr*(m_ny+2)+ybinnr;
    m_contents[binnr] += weight;
    m_nentries++;
    m_sumw  += weight;
    {
      double xn = weight*x;
      m_sumwx += xn;
      xn *= x;
      m_sumwx2 += xn;
      xn  *= x;
      m_sumwx3 += xn;
      xn  *= x;
      m_sumwx4 += xn;
    }
    {
      double yn = weight*y;
      m_sumwy += yn;
      yn *= y;
      m_sumwy2 += yn;
      yn  *= x;
      m_sumwy3 += yn;
      yn  *= x;
      m_sumwy4 += yn;
    }
  }
  return 0;
}
void *CCPCHisto::getextid (void)
{
  return extid;
}

bool CCPCHisto::nameeq(char *nam, int namlen)
{
  bool r;
  r = (namlen == m_namelen);
  r = r && (strcmp(m_name,nam) == 0);
  return r;
}

void CCPCHisto::makedimname(char *name, char **outp)
{
  int olen=0;
  int inc;
  char *out;
  inc = strlen ("H1D/");
  olen += inc;
  inc = strlen (CCPCHSys::m_instance().m_name.c_str());
  olen += inc;
  inc = strlen ("/CCPCAlg/");
  olen += inc;
  inc = strlen (name);
  olen += inc;
  olen++;
  out   = (char *)malloc(olen);
  *outp = out;
  out[0]=0;
  if (_type == H_1DIM)
  {
    strcpy(out,"H1D/");
  }
  else if (_type == H_2DIM)
  {
    strcpy(out,"H2D/");
  }
  else if (_type == H_PROFILE)
  {
    strcpy(out,"P1D/");
  }
  strcat(out,CCPCHSys::m_instance().m_name.c_str());
  strcat(out,"/CCPCAlg/");
  strcat(out,name);

}
CCPCPHisto::CCPCPHisto() : CCPCHisto()
{
}
void CCPCPHisto::clear() 
{
  CCPCHisto::clear();
  //m_sumwx   = 0.0;
  //m_sumwx2  = 0.0;
  //m_sumwy   = 0.0;
  //m_sumwy2  = 0.0;
}
CCPCPHisto::CCPCPHisto(char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_PROFILE, 0, name, title, nx, xmin,xmax,0, 0.0, 0.0);
/*  m_sumwx   = 0.0;
  m_sumwx2  = 0.0;
  m_sumwy   = 0.0;
  m_sumwy2  = 0.0;
 */ CCPCHSys::m_instance().addObj(this);
}
CCPCPHisto::CCPCPHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_PROFILE, (Histo*)ext, name, title, nx, xmin,xmax,0, 0.0, 0.0);
  //m_sumwx   = 0.0;
  //m_sumwx2  = 0.0;
  //m_sumwy   = 0.0;
  //m_sumwy2  = 0.0;
  CCPCHSys::m_instance().addObj(this);
}
int CCPCPHisto::serialize(void* &ptr)
{
  int siz;
  int status;
  int titl = titlen();
  int naml = namelen();
  int hdrl = hdrlen();//sizeof(DimHistbuff1)+titl+1+naml+1;
  siz = hdrl+datasize();//2*(p->m_nx+2)*sizeof(dimtype);
  DimHistbuff1 *pp = (DimHistbuff1*)ptr;
  pp->dataoff = hdrl;
  pp->reclen = siz;
  pp->titlen = titl;
  pp->namelen = naml;
  pp->addoffset = m_addoff;
  pp->nentries = (float)getnents();
  pp->dim = 1;
  pp->type = H_PROFILE;
  pp->nxbin = m_nx;
  pp->xmin  = m_xmin;
  pp->xmax  = m_xmax;
  pp->m_sumw  = m_sumw;
  pp->m_sumw2 = m_sumw2;
  pp->m_sumwx = m_sumwx;
  pp->m_sumwx2  = m_sumwx2;
  pp->m_sumwx3  = m_sumwx3;
  pp->m_sumwx4  = m_sumwx4;
  pp->m_sumwy = m_sumwy;
  pp->m_sumwy2  = m_sumwy2;
  pp->m_sumwy3  = m_sumwy3;
  pp->m_sumwy4  = m_sumwy4;
  char *tit;
  char *nam;
  dimtype *ntries;
  dimtype *nents;
  dimtype *sum;
  dimtype *sum2;
  pp->nameoff = sizeof(DimHistbuff1);
  pp->titoff  = pp->nameoff+namelen();
  nam = (char*)AddPtr(pp,pp->nameoff);
  tit = (char*)AddPtr(pp,pp->titoff);
  cpyName(nam);
  cpytitle(tit);
  ntries = (dimtype*)AddPtr(ptr,hdrl);
//  get(ntries);
  nents = ntries;
  sum   = ntries+(m_nx+2);
  sum2  = ntries+2*(m_nx+2);
  getentries(nents);
  getsums(sum);
  getsum2s(sum2);
  status  = 0;
  status  = 0;
  ptr = (void*)((char*)ptr+siz);
  return siz;
}  
CCPCRHisto::CCPCRHisto(char *name, char *title, int nx, float xmin, float xmax ) : CCPCPHisto()
{
  setup(H_RATE, 0, name, title, nx, xmin,xmax,0, 0.0, 0.0);
  m_addoff = 9;
  //m_sumwx   = 0.0;
  //m_sumwx2  = 0.0;
  //m_sumwy   = 0.0;
  //m_sumwy2  = 0.0;
  CCPCHSys::m_instance().addObj(this);
}
CCPCPHisto::~CCPCPHisto()
{
}
int CCPCPHisto::fill(float x, float y)
{
  return fill((double)x,(double)y, 1.0);
}
int CCPCPHisto::fill(float x, float y, float w)
{
  return fill((double)x, (double)y, (double)w);
}
int CCPCPHisto::fill(double x, double y)
{
  return fill(x,y,1.0);
}
int CCPCPHisto::fill(double x, double y, double w)
{
  bindesc *pcont = (bindesc*)m_contents;
  int xbinnr;
  int binnr;
  if (w<0.0) w=-w;
  if (_type == H_PROFILE)
  {
    if (x < m_xmin)
    {
      xbinnr  = 0;
    }
    else if (x>m_xmax)
    {
      xbinnr  = m_nx+1;
    }
    else
    {
      xbinnr  = (int)((x-m_xmin)/m_binsx + 1);
    }
    binnr  = xbinnr;
    pcont[xbinnr].netries += w;
    pcont[xbinnr].sum += y;
    pcont[xbinnr].sum2 += (y*y);
    m_sumw += w;
    m_sumw2 += w*w;
    m_sumwx += w*x;
    m_sumwx2  += x*x*w;
    m_sumwy += y*w;
    m_sumwy2  += y*y*w;
    m_nentries++;
    return 0;
  }
  else
  {
    return 1;
  }
}
int CCPCPHisto::getsums (float *to) 
  {
    bindesc *pcont = (bindesc*)m_contents;
    int i;
    for (i=0; i<m_nx+2; i++)
    {
      to[i] = (float)pcont[i].sum;
    }
    return 0;
  }
int CCPCPHisto::getsum2s(float *to )
  {
    bindesc *pcont = (bindesc*)m_contents;
    int i;
    for (i=0; i<m_nx+2; i++)
    {
      to[i] = (float)pcont[i].sum2;
    }
    return 0;
  }
int CCPCPHisto::getsums (double *to) 
  {
    bindesc *pcont = (bindesc*)m_contents;
    int i;
    for (i=0; i<m_nx+2; i++)
    {
      to[i] = pcont[i].sum;
    }
    return 0;
  }
int CCPCPHisto::getsum2s(double *to )
  {
    bindesc *pcont = (bindesc*)m_contents;
    int i;
    for (i=0; i<m_nx+2; i++)
    {
      to[i] = pcont[i].sum2;
    }
    return 0;
  }
int CCPCPHisto::getentries(float *to)
  {
    bindesc *pcont = (bindesc*)m_contents;
    int i;
    for (i=0; i<m_nx+2; i++)
    {
      to[i] = (float)pcont[i].netries;
    }
    return 0;
  }
int CCPCPHisto::getentries(double *to)
  {
    bindesc *pcont = (bindesc*)m_contents;
    int i;
    for (i=0; i<m_nx+2; i++)
    {
      to[i] = (double)pcont[i].netries;
    }
    return 0;
  }
CCPCRHisto::CCPCRHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax ) : CCPCPHisto()
{
  setup(H_RATE, (Histo*)ext, name, title, nx, xmin,xmax,0, 0.0, 0.0);
  m_addoff = 9;
  CCPCHSys::m_instance().addObj(this);
  m_sumwx   = 0.0;
  m_sumwx2  = 0.0;
  m_sumwy   = 0.0;
  m_sumwy2  = 0.0;
}
CCPCRHisto::~CCPCRHisto()
{
}

HSys::HSys()
  {
    m_sys = &CCPCHSys::m_instance();
  }

Histo *HSys::findhisto(char *name)
{
  CCPCHisto *h;
  h = (CCPCHisto *)m_sys->findobj(name);
  if (h != 0)
  {
    return (Histo*)h->getextid();
  }
  else
  {
    return 0;
  }
}
void *hccpc_init(char *nam)
  {
    static int inited=0;
    //static HSys *hsys;
    static CCPCHSys *hsi;
    if (inited == 0)
    {
      DimServer::setWriteTimeout(20);
      hsi=&CCPCHSys::m_instance();
      hsi->setup(nam);
      //hsi->m_serv->autoStartOn();
      //hsi->start();
      inited =1;
    }
    return hsi;
  }

