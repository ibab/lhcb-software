#ifndef CCPCHIST_TAG
#define CCPCHIST_TAG
#include "hist_types.h"
#define H_IllegalID 1

#ifdef __cplusplus
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
  Histo(char *name, char *title, int nx, float xmin, float xmax );
//Constructor for 2-dim histogram
	Histo(char *name, char *title, int nx, float xmin, float xmax, 
    int ny, float ymin, float ymax );
//Constructor generic histogram
  Histo();
	virtual ~Histo();
	//int setname ( char* name);
	void clear(void);
	int put (float *from) ;
	int put (double *from) ;
	int putnents (int ne) ;
	int get (float *to) ;
	int geterr (float *to) ;
	int get (double *to) ;
	int geterr (double *to) ;
	int getnents() ;

	int fill (float x);
	int fill (float x, bintype weight);
	int fill (float x, float y, bintype weight);
};
class PHisto // public Histo
{
private:
  CCPCPHisto *h;
public:
  PHisto(char *name, char *title, int nx, float xmin, float xmax );
	virtual ~PHisto();
	int fill(float x, float y);
	int getsums (float *to) ;
  int getsum2s(float *to );
	int getsums (double *to) ;
  int getsum2s(double *to );
  int getentries(float *to);
  void clear(void);
};
#endif
#ifdef __cplusplus
extern "C"{
#endif
  void *hccpc_init(char*);
  void *hccpc_book1(char *name, char *title, int nx, float xmin, float xmax );
  void *hccpc_profile(char *name, char *title, int nx, float xmin, float xmax );
  void *hccpc_book2(char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax );
	int hfill1 (void *id, float x, bintype weight);
	int hfill2 (void *id, float x, float y, bintype weight);
  int hfillp (void *id, float x, float y);
#ifdef __cplusplus
}
#endif

#endif

