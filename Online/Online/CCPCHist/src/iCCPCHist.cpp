#include <string.h>
#include <stdlib.h>
#include "dimhist.h"
#include "CCPCHist.h"

#include <math.h>
#include <iterator>
 
#define MIN( x , y)  =   (( (x) < (y) ) ? (x) : (y) ) 
#define MAX( x , y)  =   (( (x) > (y) ) ? (x) : (y) )

static DimHistbuff1 dumbuf1;
static DimHistbuff2 dumbuf2;
static int mpty;

CCPCHSys::CCPCHSys()
{
  m_serv = new HistServer();
}
CCPCHSys::~CCPCHSys()
{
}
void CCPCHSys::setname (char *n)
{
  char *nodename;
#ifdef WIN32
  nodename  = getenv("COMPUTERNAME");
#else
  nodename  = getenv("HOSTNAME");
#endif
  if (nodename != 0)
  {
    strcpy(m_name,nodename);
    strcat(m_name,"_");
    strcat(m_name,n);
  }
  else
  {
    strcpy(m_name,n);
  }
  return;
}

CCPCHSys& CCPCHSys::m_instance()  {
  static CCPCHSys s;
  return s;
}

void CCPCHSys::start()
{
  char nam[1024];
  strcpy(nam,m_name);
  strcat(nam,"/HistCommand");
  m_rpc = new HistRPC(this, nam, "I:1;C","F");
  strcpy(nam,m_name);
  strcat(nam,"/HistData");
  m_genSrv = new DimService(nam, "F",(void*)&mpty, 4);

  m_serv->start(m_name);
}
void CCPCHSys::add(CCPCHisto* h)
{
  m_hists.push_back(h);
}

CCPCHisto *CCPCHSys::findhisto(char *nam)
{
  std::vector <int>::size_type i;
  int namlen;
  namlen  = strlen(nam);
  for (i =0;i<m_hists.size();i++)
  {
    CCPCHisto *h = m_hists[i];
    if (h->nameeq(nam,namlen))
    {
      return h;
    }
  }
  return 0;
}
void CCPCHisto::setup(HTYPE typ, Histo *ext,char *name, char *title, 
                     int nx, float xmin, float xmax, 
                     int ny, float ymin, float ymax )
{
	_type	= typ;
	m_nentries	= 0;
	m_nx	= 0;
	m_xmin	= 0.0;
	m_xmax	= 0.0;
	m_binsx	= 0.0;
	m_ny	= 0;
	m_ymin	= 0.0;
	m_ymax	= 0.0;
	m_binsy	= 0.0;
	m_titlen = 0;
	m_title = 0;
	m_contsiz		= 0;
	m_contents	= 0;
  m_dimservname  = 0;
  m_Tdimservname  = 0;
  m_sumw = 0.0;
  m_sumx = 0.0;
  m_sumx2 = 0.0;
  m_sumx3 = 0.0;
  m_sumx4 = 0.0;
  m_sumy = 0.0;
  m_sumy2 = 0.0;
  m_sumy3 = 0.0;
  m_sumy4 = 0.0;

  extid = ext;
  if (nx == 0) return;
  //switch (ny)
  //{
  //case 0:
  //  {
  //  _type = H_1DIM;
  //  break;
  //  }
  //default: 
  //  {
  //    _type = H_2DIM;
  //  }
  //}
	setname(name);
	Init(title,nx,xmin,xmax, ny, ymin, ymax);
	makedimname(name,&m_dimservname);
  serv = new HistService (this, m_dimservname,"F", &dumbuf1,sizeof(dumbuf1));
  int l = strlen(m_dimservname)+strlen("/gauchocomment")+1;
  m_Tdimservname  = (char*)malloc(l);
  strcpy(m_Tdimservname,m_dimservname);
  strcat(m_Tdimservname,"/gauchocomment");
  m_Tdimservname[l] = 0;
  Tserv = new DimService(m_Tdimservname,m_title);
  return;
}
CCPCHisto::CCPCHisto()
{
  setup(H_ILLEGAL, 0,"","",0,0.0,0.0,0,0.0,0.0);
}
CCPCHisto::CCPCHisto(Histo *ext)
{
  setup(H_ILLEGAL, ext,"","",0,0.0,0.0,0,0.0,0.0);
}
CCPCHisto::CCPCHisto(char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_1DIM, 0,name, title, nx, xmin, xmax,0, 0.0,0.0); 
}
CCPCHisto::CCPCHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_1DIM, ext, name, title, nx, xmin, xmax,0, 0.0,0.0); 
}
CCPCHisto::CCPCHisto(char *name, char *title, int nx, float xmin, float xmax, 
					int ny, float ymin, float ymax )
{
  setup (H_2DIM, 0, name, title, nx, xmin, xmax, ny, ymin, ymax );
}
CCPCHisto::CCPCHisto(Histo *ext, char *name, char *title, 
                     int nx, float xmin, float xmax, 
                     int ny, float ymin, float ymax )
{
  setup (H_2DIM, ext, name, title, nx, xmin, xmax, ny, ymin, ymax );
}
int CCPCHisto::Init(char *title, int nx, float xmin, float xmax )
{
  Init(title, nx, xmin, xmax, 0, 0.0, 0.0);
	return 0;
}
int CCPCHisto::Init(char *title, int nx, float xmin, float xmax, 
					int ny, float ymin, float ymax )
{
  CCPCHSys::m_instance().add(this);
	m_nentries	= 0;
	m_nx	= nx;
	m_xmin	= xmin;
	m_xmax	= xmax;
  m_binsx = 0.0;
  if (nx>0)
  {
	  m_binsx	= (m_xmax-m_xmin)/m_nx;
  }
	m_ny	= ny;
	m_ymin	= ymin;
	m_ymax	= ymax;
  m_binsy = 0.0;
  if (m_ny >0)
  {
	  m_binsy	= (m_ymax-m_ymin)/m_ny;
  }
	m_titlen = strlen(title);
	m_title = (char*)malloc(m_titlen+1);
	strcpy(m_title,title);
	m_title[m_titlen]	= 0;
  m_contents  = 0;
  if (_type == H_2DIM)
  {
	  m_contsiz		= (ny+2)*(nx+2)*sizeof(bintype);
	  m_contents	= (bintype*)malloc(m_contsiz);
  }
  else if (_type == H_1DIM)
  {
	  m_contsiz		= (nx+2)*sizeof(bintype);
	  m_contents	= (bintype*)malloc(m_contsiz);
  }
  else if (_type == H_PROFILE)
  {
	  m_contsiz		= (nx+2)*sizeof(bindesc);
	  m_contents	= (bintype*)malloc(m_contsiz);
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
	if (m_titlen != 0)
	{
		free(m_title);
		m_titlen	= 0;
	}
	if (m_contsiz != 0)
	{
		free(m_contents);
		m_contsiz	= 0;
	}
  if (m_dimservname != 0)
  {
    free (m_dimservname);
  }
  if (m_Tdimservname != 0)
  {
    free (m_Tdimservname);
  }
}
int CCPCHisto::setname ( char* name)
{
	memset(m_name,0,sizeof(m_name));
	strncpy(m_name,name,sizeof(m_name)-1);
  m_namelen = strlen(m_name);
	return 0;
}

void CCPCHisto::clear(void)
{
  m_nentries  = 0;
  m_sumw = 0.0;
  m_sumx = 0.0;
  m_sumx2 = 0.0;
  m_sumx3 = 0.0;
  m_sumx4 = 0.0;
  m_sumy = 0.0;
  m_sumy2 = 0.0;
  m_sumy3 = 0.0;
  m_sumy4 = 0.0;

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
	*nx	= m_nx;
	*xmin	= m_xmin;
	*xmax	= m_xmax;
	*bins	= m_binsx;
	return 0;
}
int CCPCHisto::info (char *title, int * nx, float *xmin,float *xmax, float *binsx,
						int * ny, float *ymin,float *ymax, float *binsy)
{
	strcpy(title,m_title);
	*nx	= m_nx;
	*xmin	= m_xmin;
	*xmax	= m_xmax;
	*binsx	= m_binsx;
	*ny	= m_ny;
	*ymin	= m_ymin;
	*ymax	= m_ymax;
	*binsy	= m_binsy;
	return 0;
}

int CCPCHisto::modify (char *title, int nx, float xmin, float xmax)
{
	if (m_titlen != 0)
	{
		free(title);
		m_titlen	= 0;
	}
	if (m_contsiz != 0)
	{
		free(m_contents);
		m_contsiz	= 0;
	}
	Init(title,nx,xmin,xmax);
	return 0;
}
int CCPCHisto::modify (char *title, int nx, float xmin, float xmax,
							int ny, float ymin, float ymax)
{
	if (m_titlen != 0)
	{
		free(title);
		m_titlen	= 0;
	}
	if (m_contsiz != 0)
	{
		free(m_contents);
		m_contsiz	= 0;
	}
	Init(title,nx,xmin,xmax,ny,ymin,ymax);
	return 0;
}
int CCPCHisto::fill(float x)
{
	fill (x,1.0);
	return 0;
}
int CCPCHisto::fill (float x, bintype weight)
{
	int xbinnr;
	int binnr;
	if (_type == H_1DIM)
	{
		if (x < m_xmin)
		{
			xbinnr	= 0;
		}
		else if (x>m_xmax)
		{
			xbinnr	= m_nx+1;
		}
		else
		{
			xbinnr	= (int)((x-m_xmin)/m_binsx + 1);
		}
		binnr	= xbinnr;
		m_contents[xbinnr] += weight;
		m_nentries++;
    {
      m_sumw  += weight;
      double xn = weight*x;
      m_sumx += xn;
      xn *= x;
      m_sumx2 += xn;
      xn  *= x;
      m_sumx3 += xn;
      xn  *= x;
      m_sumx4 += xn;
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
int CCPCHisto::fill (float x, float y, bintype weight)
{
	int xbinnr;
	int ybinnr;
	int binnr;
	if (_type == H_2DIM)
	{
		if (x < m_xmin)
		{
			xbinnr	= 0;
		}
		else if (x>m_xmax)
		{
			xbinnr	= m_nx+1;
		}
		else
		{
			xbinnr	= (int)((x-m_xmin)/m_binsx + 1);
		}
		if (y < m_ymin)
		{
			ybinnr	= 0;
		}
		else if (y>m_ymax)
		{
			ybinnr	= m_ny+1;
		}
		else
		{
			ybinnr	= (int)((y-m_ymin)/m_binsy + 1);
		}
		binnr	= xbinnr*(m_ny+2)+ybinnr;
		m_contents[binnr] += weight;
		m_nentries++;
    m_sumw  += weight;
    {
      double xn = weight*x;
      m_sumx += xn;
      xn *= x;
      m_sumx2 += xn;
      xn  *= x;
      m_sumx3 += xn;
      xn  *= x;
      m_sumx4 += xn;
    }
    {
      double yn = weight*y;
      m_sumy += yn;
      yn *= y;
      m_sumy2 += yn;
      yn  *= x;
      m_sumy3 += yn;
      yn  *= x;
      m_sumy4 += yn;
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

//int CCPCHisto::SetupPublishing(char *servicename)
//{
//	return 0;
//}
void CCPCHisto::makedimname(char *name, char **outp)
{
  int olen=0;
  char *out;
  olen += strlen ("H1D/");
  olen += strlen (CCPCHSys::m_instance().m_name);
  olen += strlen ("/CCPCAlg/");
  olen += strlen (name);
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
		strcpy(out,"HPD/");
	}
  strcat(out,CCPCHSys::m_instance().m_name);
	strcat(out,"/");
	strcat(out,"CCPCAlg/");
  strcat(out,name);
}

CCPCPHisto::CCPCPHisto(char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_PROFILE, 0, name, title, nx, xmin,xmax,0, 0.0, 0.0);

}
CCPCPHisto::CCPCPHisto(PHisto *ext, char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_PROFILE, (Histo*)ext, name, title, nx, xmin,xmax,0, 0.0, 0.0);
}
CCPCPHisto::~CCPCPHisto()
{
}
int CCPCPHisto::fill(float x, float y)
{
	bindesc *pcont = (bindesc*)m_contents;
	int xbinnr;
	int binnr;
	if (_type == H_PROFILE)
	{
		if (x < m_xmin)
		{
			xbinnr	= 0;
		}
		else if (x>m_xmax)
		{
			xbinnr	= m_nx+1;
		}
		else
		{
			xbinnr	= (int)((x-m_xmin)/m_binsx + 1);
		}
		binnr	= xbinnr;
		pcont[xbinnr].netries++;
		pcont[xbinnr].sum += y;
		pcont[xbinnr].sum2 += (y*y);
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
HSys::HSys()
  {
    m_sys = &CCPCHSys::m_instance();
  }

  Histo *HSys::findhisto(char *name)
  {
    CCPCHisto *h;
    h = m_sys->findhisto(name);
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
    static HSys *hsys;
    if (inited == 0)
    {
      hsys  = new HSys();
      CCPCHSys &hsi=CCPCHSys::m_instance();
      hsi.setname(nam);
      hsi.m_serv->autoStartOn();
      hsi.start();
      inited =1;
    }
    return hsys;
  }

