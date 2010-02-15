#ifndef CNTER_H
#define CNTER_H
#include "hist_types.h"
#include "CCPCObj.h"
#include <vector>
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)


class CCPCCounter : public CCPCObj
{
//friend class HistService;
protected:
  HTYPE _type;
  void *m_contents;
  int m_namelen;
  char *m_title;      /** Pointer to Histogram Title **/
  int m_titlen;       /* Allocated length in Bytes for the title */
  int buffersize;     /* buffer size of the data if serialized*/
  unsigned int m_contsiz;      /* Allocated length in Bytes for the bin contents */
  int m_addoff;

  // Methods

  //void CopyData(int*,int*);
  //void CopyData(long long *,long long*);
  //void CopyData(float*,float*);
  //void CopyData(double*,double*);
public:
  char* m_name;    /** Name of the histogram **/
  char *name();
  //HistService *serv;
  HTYPE type() {return _type;};
  CCPCCounter(char *name, char *title, int *data );
  CCPCCounter(char *name, char *title, long long *data );
  CCPCCounter(char *name, char *title, float *data );
  CCPCCounter(char *name, char *title, double *data );
  CCPCCounter();
  void setup(HTYPE typ, void *ext, char *name, char *title);
  virtual ~CCPCCounter();
  int setname ( char* name);
  void clear(void);
  int titlen();
  int namelen();
  void cpytitle(char *ptr);
  int Init(char *title);
  //virtual int analyze(void *buff, int &dim, char *&tit, int &nentries);
  //virtual int analyze(void *buff, int &dim, char *&tit, int &nentries,
  //                    int &nx, float &xmin, float &xmax);
  //virtual int analyze(void *nuff, int &dim, char *&tit, int &nentries,
  //                    int &nx, float &xmin, float &xmax,
  //                    int &ny, float &ymin, float &ymax);
  //virtual int analyze(void *buff, int &dim, char *&tit, int &nentries,
  //                    int &nx, float &xmin, float &xmax,
  //                    int &ny, float &ymin, float &ymax,
  //                    void *&cont, void *&err);


  //int info (char *title, int *nx, float *xmin, float *xmax, float *bins) ;
  //int info (char *title, int * nx, float *xmin, float *xmax, float *binsx,
  //             int * ny, float *ymin, float *ymax, float *binsy) ;

  //int modify (char *title, int nx, float xmin, float xmax);
  //int modify (char *title, int nx, float xmin, float xmax,
  //             int ny, float ymin, float ymax);
  //int fill (float x);
  //int fill (float x, float weight);
  //int fill (float x, float y, float weight);
  void *getextid (void);
  bool nameeq(char *nam, int namlen);
  int namelength(void)
  {
    return m_namelen;
  }
  void *cpyName(char *ptr);
  int datasize();
  int xmitbuffersize();
  int hdrlen();
  int serialize(void* &ptr);

};
#ifdef __cplusplus
extern "C"{
#endif
  void *cccpc_init(char* n);
#ifdef __cplusplus
}
#endif
#endif
