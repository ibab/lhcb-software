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
	char m_name[255];
	CCPCHSys();
	~CCPCHSys();
public:
  std::vector<CCPCHisto*> m_hists;
	HistServer *m_serv;
	HistRPC *m_rpc;
  static CCPCHSys& m_instance();
  void setname(char *n);
  void start();
  void add(CCPCHisto *h);
  CCPCHisto *findhisto(char *name);
  DimService *m_genSrv;
};


class CCPCHisto
{
friend class HistService;
protected:
	HTYPE _type;      /** Type of Histogram 1-dim, 2-dim, Profile **/
  Histo *extid;
  int m_namelen;
	char m_name[32];    /** Name of the histogram **/
	char *m_title;      /** Pointer to Histogram Title **/
	int m_nx;           /** Number of x-bins **/
	float m_xmin;     /** Minimum x **/
	float m_xmax;     /** Maximum y **/
	float	m_binsx;    /** Bin size in x **/
	int m_ny;           /* Number of y bins */
	float m_ymin;     /* Minimum y */
	float m_ymax;     /* Maximum y */
	float m_binsy;    /* Bin size in y */
	char *m_dimservname;/* DIM Service Name */
	char *m_Tdimservname;/* Title DIM Service Name */
	bintype *m_contents;/* Pointer to bin Content */
	int m_titlen;       /* Allocated length in Bytes for the title */
	unsigned int m_contsiz;      /* Allocated length in Bytes for the bin contents */
	int m_nentries;     /* Total Number of entries */
  double m_sumw;
  double m_sumx;
  double m_sumx2;
  double m_sumx3;
  double m_sumx4;
  double m_sumy;
  double m_sumy2;
  double m_sumy3;
  double m_sumy4;

  // Methods

	int Init(char *title, int nx, float xmin, float xmax );
	int Init(char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax );
	void makedimname(char *name, char **out);
  DimService  *Tserv;
  void CopyData(float*,float*);
  void CopyData(double*,float*);
  void CopyData(float*,double*);
  void CopyData(double*,double*);
public:
	HistService *serv;
  HTYPE type() {return _type;};
	CCPCHisto(char *name, char *title, int nx, float xmin, float xmax );
	CCPCHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax );
	CCPCHisto(char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax );
	CCPCHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax );
	CCPCHisto(Histo *ext);
	CCPCHisto();
	void setup(HTYPE typ, Histo *ext, char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax );
	virtual ~CCPCHisto();
	int setname ( char* name);
	void clear(void);
	int put (double *from) ;
	int put (float *from) ;
	int putnents (int ne) ;
	int get (float *to) ;
	int geterr (float *to) ;
	int get (double *to) ;
	int geterr (double *to) ;
	int getnents() ;

	int info (char *title, int *nx, float *xmin, float *xmax, float *bins) ;
	int info (char *title, int * nx, float *xmin, float *xmax, float *binsx,
						   int * ny, float *ymin, float *ymax, float *binsy) ;

	int modify (char *title, int nx, float xmin, float xmax);
	int modify (char *title, int nx, float xmin, float xmax,
							 int ny, float ymin, float ymax);
	int fill (float x);
	int fill (float x, bintype weight);
	int fill (float x, float y, bintype weight);
  void *getextid (void);
  bool nameeq(char *nam, int namlen);
};
class CCPCPHisto : public CCPCHisto
{
public:
	CCPCPHisto(char *name, char *title, int nx, float xmin, float xmax );
	CCPCPHisto(PHisto *ext, char *name, char *title, int nx, float xmin, float xmax );
	virtual ~CCPCPHisto();
	int fill(float x, float y);
	int getsums (float *to) ;
  int getsum2s(float *to );
  int getentries(float *to);
	int getsums (double *to) ;
  int getsum2s(double *to );
};
#ifdef __cplusplus
extern "C"{
#endif
  void *hccpc_init(char* n);
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
