#ifndef HBOOK_CCPC_internals
#define HBOOK_CCPC_internals
#include "src/hist_struct.h"
#include "src/hist_types.h"

class HistServer;
class HistService;

class HSys
{
friend class Histo;
protected:
	char name[255];
	HSys();
	~HSys();
public:
	HistServer *serv;
  static HSys& instance();
  void setname(char *n);
  void start();
  void autoinit(char *nam);
};

typedef enum
{
	H_ILLEGAL,
  H_1DIM,
  H_2DIM,
  H_PROFILE
}HTYPE;

class Histo
{
friend class HistService;
private:
protected:
	HTYPE _type;      /* Type of Histogram 1-dim, 2-dim Profile */
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
	bintype *contents;/* Pointer to bin Content */
	int titlen;       /* Allocated length in Bytes for the title */
	int contsiz;      /* Allocated length in Bytes for the bin contents */
	int nentries;     /* Total Number of entries */
	char name[32];    /* Name of the histogram */

  // Methods

	int Init(char *title, int nx, bintype xmin, bintype xmax );
	int Init(char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	void makedimname(char *name, char **out);
	HistService *serv;
public:
  HTYPE type() {return _type;};
//Constructor for 1-dim histogram
	Histo(char *name, char *title, int nx, bintype xmin, bintype xmax );
//Constructor for 2-dim histogram
	Histo(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
//Constructor generic histogram
	Histo();
	virtual ~Histo();
	int setname ( char* name);
	void clear(void);
	int put (bintype *from) ;
	int putnents (int ne) ;
	int get (bintype *to) ;
	int geterr (bintype *to) ;
	int getnents () ;

	int info (char *title, int *nx, bintype *xmin, bintype *xmax, bintype *bins) ;
	int info (char *title, int * nx, bintype *xmin,bintype *xmax, bintype *binsx,
						   int * ny, bintype *ymin,bintype *ymax, bintype *binsy) ;

	int modify (char *title, int nx, bintype xmin, bintype xmax);
	int modify (char *title, int nx, bintype xmin, bintype xmax,
							 int ny, bintype ymin, bintype ymax);
	int fill (bintype x);
	int fill (bintype x, bintype weight);
	int fill (bintype x,bintype y, bintype weight);
//	int SetupPublishing(char *servicename);
};
class PHisto : public Histo
{
public:
	PHisto(char *name, char *title, int nx, bintype xmin, bintype xmax );
	virtual ~PHisto();
	int fill(bintype x, bintype y);
	int getsums (bintype *to) ;
  int getsum2s(bintype *to );
  int getentries(float *to);
};
#ifdef __cplusplus
extern "C"{
#endif
  int hccpc_init(char *);
#ifdef __cplusplus
}
#endif

#endif
