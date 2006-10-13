#ifndef HBOOK_CCPC
#define HBOOK_CCPC
#include "hist_types.h"
#include <vector>
enum HTYPE
{
	H_ILLEGAL = 0xFEED0000,
  H_1DIM,
  H_2DIM,
  H_PROFILE
};
typedef struct
{
	int netries;
	bintype sum;
	double sum2;
} bindesc;

class HistServer;
class HistService;
class DimService;
class Histo;
class PHisto;
class CCPCHisto;
class CCPCPHisto;
class HistRPC;

class CCPCHSys
{
friend class CCPCHisto;
protected:
	char name[255];
	CCPCHSys();
	~CCPCHSys();
public:
  std::vector<CCPCHisto*> hists;
	HistServer *serv;
	HistRPC *rpc;
  static CCPCHSys& instance();
  void setname(char *n);
  void start();
  void add(CCPCHisto *h);
  CCPCHisto *findhisto(char *name);
};


class CCPCHisto
{
friend class HistService;
protected:
	HTYPE _type;      /* Type of Histogram 1-dim, 2-dim, Profile */
  Histo *extid;
  int namelen;
	char name[32];    /* Name of the histogram */
	char *title;      /* Pointer to Histogram Title */
	int nx;           /* Number of x-bins */
	bintype xmin;     /* Minimum x */
	bintype xmax;     /* Maximum y */
	bintype	binsx;    /* Bin size in x */
	int ny;           /* Number of y bins */
	bintype ymin;     /* Minimum y */
	bintype ymax;     /* Maximum y */
	bintype binsy;    /* Bin size in y */
	char *dimservname;/* DIM Service Name */
	char *Tdimservname;/* Title DIM Service Name */
	bintype *contents;/* Pointer to bin Content */
	int titlen;       /* Allocated length in Bytes for the title */
	int contsiz;      /* Allocated length in Bytes for the bin contents */
	int nentries;     /* Total Number of entries */

  // Methods

	int Init(char *title, int nx, bintype xmin, bintype xmax );
	int Init(char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	void makedimname(char *name, char **out);
  DimService  *Tserv;
//  static CCPCHSys &hsi;
public:
	HistService *serv;
  HTYPE type() {return _type;};
//Constructor for 1-dim histogram
	CCPCHisto(char *name, char *title, int nx, bintype xmin, bintype xmax );
	CCPCHisto(Histo *ext, char *name, char *title, int nx, bintype xmin, bintype xmax );
//Constructor for 2-dim histogram
	CCPCHisto(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	CCPCHisto(Histo *ext, char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
//Constructor generic histogram
	CCPCHisto(Histo *ext);
	CCPCHisto();
	void setup(HTYPE typ, Histo *ext, char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	virtual ~CCPCHisto();
	int setname ( char* name);
	void clear(void);
	int put (bintype *from) ;
	int putnents (int ne) ;
	int get (bintype *to) ;
	int geterr (bintype *to) ;
	int getnents() ;

	int info (char *title, int *nx, bintype *xmin, bintype *xmax, bintype *bins) ;
	int info (char *title, int * nx, bintype *xmin,bintype *xmax, bintype *binsx,
						   int * ny, bintype *ymin,bintype *ymax, bintype *binsy) ;

	int modify (char *title, int nx, bintype xmin, bintype xmax);
	int modify (char *title, int nx, bintype xmin, bintype xmax,
							 int ny, bintype ymin, bintype ymax);
	int fill (bintype x);
	int fill (bintype x, bintype weight);
	int fill (bintype x,bintype y, bintype weight);
  void *getextid (void);
  bool nameeq(char *nam, int namlen);
};
class CCPCPHisto : public CCPCHisto
{
public:
	CCPCPHisto(char *name, char *title, int nx, bintype xmin, bintype xmax );
	CCPCPHisto(PHisto *ext, char *name, char *title, int nx, bintype xmin, bintype xmax );
	virtual ~CCPCPHisto();
	int fill(bintype x, bintype y);
	int getsums (bintype *to) ;
  int getsum2s(bintype *to );
  int getentries(float *to);
};
#ifdef __cplusplus
extern "C"{
#endif
  void *hccpc_book1(char *name, char *title, int nx, bintype xmin, bintype xmax );
  void *hccpc_profile(char *name, char *title, int nx, bintype xmin, bintype xmax );
  void *hccpc_book2(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	int hfill1 (void *id, bintype x, bintype weight);
	int hfill2 (void *id, bintype x,bintype y, bintype weight);
  int hfillp (void *id, bintype x, bintype y);
#ifdef __cplusplus
}

#endif
#endif
