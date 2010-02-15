#ifndef HBOOK_CCPC
#define HBOOK_CCPC
#include "hist_types.h"
#include "CCPCObj.h"
#include <vector>
//enum HTYPE
//{
//  H_ILLEGAL = 0xFEED0000,
//  H_1DIM,
//  H_2DIM,
//  H_PROFILE
//};
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
typedef struct
{
  bintype netries;
  bintype sum;
  double sum2;
} bindesc;

class Histo;
//class HistServer;
//class HistService;
//class DimService;
//class PHisto;
//class RHisto;
//class CCPCHisto;
//class CCPCPHisto;
//class HistRPC;

class CCPCHisto : public CCPCObj
{
//friend class HistService;
protected:
  HTYPE _type;      /** Type of Histogram 1-dim, 2-dim, Profile **/
  Histo *extid;
  int m_namelen;
  char *m_title;      /** Pointer to Histogram Title **/
  int m_nx;           /** Number of x-bins **/
  float m_xmin;     /** Minimum x **/
  float m_xmax;     /** Maximum y **/
  float  m_binsx;    /** Bin size in x **/
  int m_ny;           /* Number of y bins */
  float m_ymin;     /* Minimum y */
  float m_ymax;     /* Maximum y */
  float m_binsy;    /* Bin size in y */
  char *m_dimservname;/* DIM Service Name */
  char *m_Tdimservname;/* Title DIM Service Name */
  bintype *m_contents;/* Pointer to bin Content */
  int m_titlen;       /* Allocated length in Bytes for the title */
  int buffersize;     /* buffer size of the data if serialized*/
  unsigned int m_contsiz;      /* Allocated length in Bytes for the bin contents */
  int m_nentries;     /* Total Number of entries */
  double m_sumw;
  double m_sumw2;
  double m_sumwx;
  double m_sumwx2;
  double m_sumwx3;
  double m_sumwx4;
  double m_sumwy;
  double m_sumwy2;
  double m_sumwy3;
  double m_sumwy4;
  int m_addoff;

  // Methods

  int Init(char *title, int nx, float xmin, float xmax );
  int Init(char *title, int nx, float xmin, float xmax, 
             int ny, float ymin, float ymax );
  void makedimname(char *name, char **out);
  //DimService  *Tserv;
  void CopyData(float*,float*);
  void CopyData(double*,float*);
  void CopyData(float*,double*);
  void CopyData(double*,double*);
public:
  char* m_name;    /** Name of the histogram **/
  char *name();
  //HistService *serv;
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
  int getnents();
  int titlen();
  int namelen();
  void cpytitle(char *ptr);
  virtual int analyze(void *buff, int &dim, char *&tit, int &nentries);
  virtual int analyze(void *buff, int &dim, char *&tit, int &nentries,
                      int &nx, float &xmin, float &xmax);
  virtual int analyze(void *nuff, int &dim, char *&tit, int &nentries,
                      int &nx, float &xmin, float &xmax,
                      int &ny, float &ymin, float &ymax);
  virtual int analyze(void *buff, int &dim, char *&tit, int &nentries,
                      int &nx, float &xmin, float &xmax,
                      int &ny, float &ymin, float &ymax,
                      void *&cont, void *&err);


  int info (char *title, int *nx, float *xmin, float *xmax, float *bins) ;
  int info (char *title, int * nx, float *xmin, float *xmax, float *binsx,
               int * ny, float *ymin, float *ymax, float *binsy) ;

  int modify (char *title, int nx, float xmin, float xmax);
  int modify (char *title, int nx, float xmin, float xmax,
               int ny, float ymin, float ymax);
  int fill (float x);
  int fill (float x, float weight);
  int fill (float x, float y, float weight);
  void *getextid (void);
  bool nameeq(char *nam, int namlen);
  int namelength(void)
  {
    return m_namelen;
  }
  void *cpyName(void *ptr);
  int datasize();
  int xmitbuffersize();
  int hdrlen();
  int serialize(void* &ptr);

};
class CCPCPHisto : public CCPCHisto
{
protected:
public:
  CCPCPHisto(char *name, char *title, int nx, float xmin, float xmax );
  CCPCPHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax );
  CCPCPHisto();
  virtual ~CCPCPHisto();
  int fill(float x, float y);
  int fill(float x, float y, float w);
  int fill(double x, double y);
  int fill(double x, double y, double w);
  int getsums (float *to) ;
  int getsum2s(float *to );
  int getentries(float *to);
  int getentries(double *to);
  int getsums (double *to) ;
  int getsum2s(double *to );
  int hdrlen();
  void clear(void);
  int datasize();
  int xmitbuffersize();
  int serialize(void* &ptr);
};
class CCPCRHisto : public CCPCPHisto
{
public:
  CCPCRHisto(char *name, char *title, int nx, float xmin, float xmax );
  CCPCRHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax );
  virtual ~CCPCRHisto();
};




#ifdef __cplusplus
extern "C"{
#endif
  void *hccpc_init(char* n);
  void *hccpc_book1(char *name, char *title, int nx, float xmin, float xmax );
  void *hccpc_profile(char *name, char *title, int nx, float xmin, float xmax );
  void *hccpc_book2(char *name, char *title, int nx, float xmin, float xmax, 
                    int ny, float ymin, float ymax );
  int hfill1 (void *id, float x, float weight);
  int hfill2 (void *id, float x, float y, float weight);
  int hfillp (void *id, float x, float y);
#ifdef __cplusplus
}
#endif
#endif
