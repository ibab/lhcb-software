#ifndef HBOOK_CCPC_internals
#define HBOOK_CCPC_internals
#include "src/hist_struct.h"
#include "src/hist_types.h"
#include <vector>

class HistServer;
class HistService;
class Histo;

class HSys
{
friend class Histo;
protected:
	 //int nservices;
	 //std::vector <HistService*> *HServices;
	 //std::vector <Histo*> *Histos;
	 char name[255];
	HSys();
	~HSys();
public:
	HistServer *serv;
  static HSys& instance();
  void setname(char *n);
  void start();
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
private:
public:
	int type;
	char *title;
	int nx;
	bintype xmin;
	bintype xmax;
	bintype	binsx;
	int ny;
	bintype ymin;
	bintype ymax;
	bintype binsy;
	char *dimservname;
	bintype *contents;
	int titlen;
	int contsiz;
	int nentries;
	int Init(char *title, int nx, bintype xmin, bintype xmax );
	int Init(char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	void makedimname(char *name, char **out);
	char name[32];
	HistService *serv;
public:
	Histo(char *name, char *title, int nx, bintype xmin, bintype xmax );
	Histo(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	Histo();
	~Histo();
	int setname ( char* name);
	void clear(void);
	int put (int ne, bintype *from) ;
	int get (int *ne, bintype *to) ;
	int geterr (int *ne, bintype *to) ;

	int info (char *title, int *nx, bintype *xmin, bintype *xmax, bintype *bins) ;
	int info (char *title, int * nx, bintype *xmin,bintype *xmax, bintype *binsx,
						   int * ny, bintype *ymin,bintype *ymax, bintype *binsy) ;

	int modify (char *title, int nx, bintype xmin, bintype xmax);
	int modify (char *title, int nx, bintype xmin, bintype xmax,
							 int ny, bintype ymin, bintype ymax);
	int fill (bintype x);
	int fill (bintype x, bintype weight);
	int fill (bintype x,bintype y, bintype weight);
	int SetupPublishing(char *servicename);
};
class PHisto : public Histo
{
public:
	PHisto(char *name, char *title, int nx, bintype xmin, bintype xmax );
	~PHisto();
	int fill(bintype x, bintype y);
	int geterr (int *ne, bintype *to) ;
  int get(int *ne, bintype *to );
};
#endif