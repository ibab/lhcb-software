#ifndef CCPCHIST_TAG
#define CCPCHIST_TAG
#include "hist_types.h"
#define H_IllegalID 1


class CCPCHisto;
class CCPCPHisto;
class CCPCHSys;
class Histo;


class HSys
{
protected:
  CCPCHSys *sys;
public:
	HSys();
	~HSys();
  Histo *findhisto(char *name);
};


class Histo
{
private:
  CCPCHisto *h;
public:
//Constructor for 1-dim histogram
  Histo(char *name, char *title, int nx, bintype xmin, bintype xmax );
//Constructor for 2-dim histogram
	Histo(char *name, char *title, int nx, bintype xmin, bintype xmax, 
    int ny, bintype ymin, bintype ymax );
//Constructor generic histogram
  Histo();
	virtual ~Histo();
	//int setname ( char* name);
	void clear(void);
	int put (bintype *from) ;
	int putnents (int ne) ;
	int get (bintype *to) ;
	int geterr (bintype *to) ;
	int getnents() ;

	//int info (char *title, int *nx, bintype *xmin, bintype *xmax, bintype *bins) ;
	//int info (char *title, int * nx, bintype *xmin,bintype *xmax, bintype *binsx,
	//					   int * ny, bintype *ymin,bintype *ymax, bintype *binsy) ;

	//int modify (char *title, int nx, bintype xmin, bintype xmax);
	//int modify (char *title, int nx, bintype xmin, bintype xmax,
	//						 int ny, bintype ymin, bintype ymax);
	int fill (bintype x);
	int fill (bintype x, bintype weight);
	int fill (bintype x, bintype y, bintype weight);
};
class PHisto // public Histo
{
private:
  CCPCPHisto *h;
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
  HSys *hccpc_init(char *);
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

