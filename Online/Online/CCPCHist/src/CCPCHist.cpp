#include <string.h>
#include "hist_struct.h"
#include "src/dimhist.h"
#include "CCPCHist/CCPCHist.h"

//#include "util.h"
//#include <rtl.h>
#include <math.h>
#define MICRO    1       /* conditional compilation            */ 
 
#define MIN( x , y)  =   (( (x) < (y) ) ? (x) : (y) ) 
#define MAX( x , y)  =   (( (x) > (y) ) ? (x) : (y) )

#define derel(add,base)  ((int)add+(int)base)
#define rel(add,base)  ((int)add-(int)base)
 
 
/*  ------------------------------------------------------------------- */    
/*                   
                         global variables 
                                                                        */    
/*  ------------------------------------------------------------------- */    
 
	 int cut_blank(char*);

static DimHistbuff1 dumbuf1;
static DimHistbuff2 dumbuf2;
HSys::HSys()
{
  serv = new HistServer();
}
HSys::~HSys()
{
}
void HSys::setname (char *n)
{
  strcpy(name,n);
  return;
}

HSys& HSys::instance()  {
  static HSys s;
  return s;
}

void HSys::start()
{
  serv->start(name);
}

aver_sigma2(bintype *ave, bintype *sigm, bintype *x,int n,double per) 
{ 
    int i; 
    double diff; 
    	
    if( n <= 0 )return( 1 ); 
    i = 0; 
    while( i < n ) 
    { 
    		diff = ave[i] - x[i]; 
    		ave[i] = (bintype)(ave[i] - diff/per); 
    		sigm[i] = (bintype) (sigm[i] - ( sigm[i] - diff*diff )/per); 
    	    ++i; 
    } 
    return( 0 ); 
} 


Histo::Histo()
{
	_type	= H_ILLEGAL;
	nentries	= 0;
	nx	= 0;
	xmin	= 0.0;
	xmax	= 0.0;
	binsx	= 0.0;
	ny	= 0;
	ymin	= 0.0;
	ymax	= 0.0;
	binsy	= 0.0;
	titlen = 0;
	title = 0;
	contsiz		= 0;
	contents	= 0;
}
Histo::Histo(char *name, char *title, int nx, bintype xmin, bintype xmax )
{
	setname(name);
	Init(title,nx,xmin,xmax);
	makedimname(name,&dimservname);
  serv = new HistService (this, dimservname,"I:2;F:2;I:1;F", &dumbuf1,sizeof(dumbuf1));
}
Histo::Histo(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					int ny, bintype ymin, bintype ymax )
{
	setname(name);
	Init(title,nx,xmin,xmax,ny,ymin,ymax);
	makedimname(name,&dimservname);
  serv = new HistService (this, dimservname,"I:2;F:2;I:1;F:2;I:1;F", &dumbuf2,sizeof(dumbuf2));
}
int Histo::Init(char *title, int nx, bintype xmin, bintype xmax )
{
	_type	= H_1DIM;
	nentries	= 0;
	this->nx	= nx;
	this->xmin	= xmin;
	this->xmax	= xmax;
	binsx	= (xmax-xmin)/nx;
	this->ny	= 0;
	this->ymin	= 0.0;
	this->ymax	= 0.0;
	binsy	= 0.0;
	titlen = strlen(title);
	this->title = (char*)malloc(titlen+1);
	strcpy(this->title,title);
	this->title[titlen]	= 0;
	contsiz		= (nx+2)*sizeof(bintype);
	contents	= (bintype*)malloc(contsiz);
	memset(contents,0,contsiz);
	return 0;
}
int Histo::Init(char *title, int nx, bintype xmin, bintype xmax, 
					int ny, bintype ymin, bintype ymax )
{
	_type	= H_2DIM;
	nentries	= 0;
	title	= 0;
	titlen	= 0;
	contents	= 0;
	contsiz	= 0;
	this->nx	= nx;
	this->xmin	= xmin;
	this->xmax	= xmax;
	binsx	= (xmax-xmin)/nx;
	this->ny	= ny;
	this->ymin	= ymin;
	this->ymax	= ymax;
	binsy	= (ymax-ymin)/ny;
	titlen = strlen(title);
	this->title = (char*)malloc(titlen+1);
	strcpy(this->title,title);
	this->title[titlen]	= 0;
	contsiz		= (ny+2)*(nx+2)*sizeof(bintype);
	contents	= (bintype*)malloc(contsiz);
	memset(contents,0,contsiz);
	return 0;
  dimservname   = 0;
}
Histo::~Histo()
{
	if (titlen != 0)
	{
		free(title);
		titlen	= 0;
	}
	if (contsiz != 0)
	{
		free(contents);
		contsiz	= 0;
	}
  if (dimservname != 0)
  {
    free (dimservname);
  }
}
int Histo::setname ( char* name)
{
	memset(this->name,0,sizeof(this->name));
	strcpy(this->name,name);
	return 0;
}

void Histo::clear(void)
{
	if (contents != 0)
	{
		memset(contents,0,contsiz);
	}
	return;
}
int Histo::put (int ne, bintype *from )
{
	nentries	= ne;
	memcpy(contents,from,contsiz);
	return 0;
}
int Histo::get (int *ne, bintype *to )
{
	*ne	= nentries;
	memcpy(to,contents,contsiz);
	return 0;
}
int Histo::geterr (int *ne, bintype *to )
{
  int i;
	*ne	= nentries;
  for (i=0; i<nx+2; i++)
  {
    to[i]=sqrt(contents[i]);
  }
  return 0;
}
int Histo::info (char *title, int *nx, bintype *xmin, bintype *xmax, bintype *bins)
{
	strcpy(title,this->title);
	*nx	= this->nx;
	*xmin	= this->xmin;
	*xmax	= this->xmax;
	*bins	= this->binsx;
	return 0;
}
int Histo::info (char *title, int * nx, bintype *xmin,bintype *xmax, bintype *binsx,
						int * ny, bintype *ymin,bintype *ymax, bintype *binsy)
{
	strcpy(title,this->title);
	*nx	= this->nx;
	*xmin	= this->xmin;
	*xmax	= this->xmax;
	*binsx	= this->binsx;
	*ny	= this->ny;
	*ymin	= this->ymin;
	*ymax	= this->ymax;
	*binsy	= this->binsy;
	return 0;
}

int Histo::modify (char *title, int nx, bintype xmin, bintype xmax)
{
	if (titlen != 0)
	{
		free(title);
		titlen	= 0;
	}
	if (contsiz != 0)
	{
		free(contents);
		contsiz	= 0;
	}
	Init(title,nx,xmin,xmax);
	return 0;
}
int Histo::modify (char *title, int nx, bintype xmin, bintype xmax,
							int ny, bintype ymin, bintype ymax)
{
	if (titlen != 0)
	{
		free(title);
		titlen	= 0;
	}
	if (contsiz != 0)
	{
		free(contents);
		contsiz	= 0;
	}
	Init(title,nx,xmin,xmax,ny,ymin,ymax);
	return 0;
}
int Histo::fill(bintype x)
{
	fill (x,1.0);
	return 0;
}
int Histo::fill (bintype x, bintype weight)
{
	int xbinnr;
	int binnr;
	if (_type == H_1DIM)
	{
		if (x < xmin)
		{
			xbinnr	= 0;
		}
		else if (x>xmax)
		{
			xbinnr	= nx+1;
		}
		else
		{
			xbinnr	= (int)((x-xmin)/binsx + 1);
		}
		binnr	= xbinnr;
		contents[xbinnr] += weight;
		nentries++;
		return 0;
	}
	if (_type == H_2DIM)
	{
		fill(x,weight,1.0);
		return 0;
	}
	else
	{
		return 1;
	}
}
int Histo::fill (bintype x,bintype y, bintype weight)
{
	int xbinnr;
	int ybinnr;
	int binnr;
	if (_type == H_2DIM)
	{
		if (x < xmin)
		{
			xbinnr	= 0;
		}
		else if (x>xmax)
		{
			xbinnr	= nx+1;
		}
		else
		{
			xbinnr	= (int)((x-xmin)/binsx + 1);
		}
		if (y < ymin)
		{
			ybinnr	= 0;
		}
		else if (y>ymax)
		{
			ybinnr	= ny+1;
		}
		else
		{
			ybinnr	= (int)((y-ymin)/binsy + 1);
		}
		binnr	= ybinnr*(nx+2)+xbinnr;
		contents[binnr] += weight;
		nentries++;
	}
	return 0;
}
int Histo::SetupPublishing(char *servicename)
{
	return 0;
}
void Histo::makedimname(char *name, char **outp)
{
  int olen=0;
  char *out;
  olen += strlen ("H1_");
  olen += strlen (HSys::instance().name);
  olen += strlen ("/CCPCAlg/");
  olen += strlen (name);
  out   = (char *)malloc(olen);
  *outp = out;
	out[0]=0;
	if (_type == H_1DIM)
	{
		strcpy(out,"H1_");
	}
	else if (_type == H_2DIM)
	{
		strcpy(out,"H2_");
	}
	else if (_type == H_PROFILE)
	{
		strcpy(out,"HP_");
	}
  strcat(out,HSys::instance().name);
	strcat(out,"/");
	strcat(out,"CCPCAlg/");
  strcat(out,name);
}

PHisto::PHisto(char *name, char *title, int nx, bintype xmin, bintype xmax )
{
	_type	= H_PROFILE;
	nentries	= 0;
	this->nx	= nx;
	this->xmin	= xmin;
	this->xmax	= xmax;
	binsx	= (xmax-xmin)/nx;
	this->ny	= 0;
	this->ymin	= 0.0;
	this->ymax	= 0.0;
	binsy	= 0.0;
	titlen = strlen(title);
	this->title = (char*)malloc(titlen+1);
	strcpy(this->title,title);
	this->title[titlen]	= 0;
	contsiz		= (nx+2)*sizeof(bindesc);
	contents	= (bintype*)malloc(contsiz);
	memset(contents,0,contsiz);
	makedimname(name,&dimservname);
  serv = new HistService (this, dimservname,"I:2;F:2;I:1;F", &dumbuf1, sizeof(dumbuf1));
}
PHisto::~PHisto()
{
}
int PHisto::fill(bintype x, bintype y)
{
	bindesc *pcont = (bindesc*)contents;
	int xbinnr;
	int binnr;
	if (_type == H_PROFILE)
	{
		if (x < xmin)
		{
			xbinnr	= 0;
		}
		else if (x>xmax)
		{
			xbinnr	= nx+1;
		}
		else
		{
			xbinnr	= (int)((x-xmin)/binsx + 1);
		}
		binnr	= xbinnr;
		pcont[xbinnr].netries++;
		pcont[xbinnr].sum += y;
		pcont[xbinnr].sum2 += (y*y);
		nentries++;
		return 0;
	}
	else
	{
		return 1;
	}
}
  PHisto::geterr(int *ne, bintype *to )
{
  int i;
  bindesc *pcont = (bindesc*)contents;
	*ne	= nentries;
  for (i=0; i<nx+2; i++)
  {
    bintype xm  = pcont[i].sum/pcont[i].netries;
    to[i] = (bintype)sqrt(pcont[i].sum2/pcont[i].netries - xm*xm);
  }
  return 0;
}

  PHisto::get(int *ne, bintype *to )
{
  int i;
  bindesc *pcont = (bindesc*)contents;
	*ne	= nentries;
  for (i=0; i<nx+2; i++)
  {
    bintype xm  = pcont[i].sum/pcont[i].netries;
    to[i] = xm;
  }
  return 0;
}
