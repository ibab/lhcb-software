#ifndef CNTER_H
#define CNTER_H
#include "Gaucho/MonTypes.h"
#include "Gaucho/MonObj.h"
#include <vector>
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)


class MonCounter : public MonObj
{
//friend class HistService;
protected:
  MONTYPE m_type;
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
  int type() {return (int)m_type;};
  MonCounter(char *name, char *title, int *data );
  MonCounter(char *name, char *title, long long *data );
  MonCounter(char *name, char *title, float *data );
  MonCounter(char *name, char *title, double *data );
  MonCounter();
  void setup(MONTYPE typ, void *ext, char *name, char *title);
  /*virtual*/ ~MonCounter();
  int setname ( char* name);
  void clear(void);
  int titlen();
  int namelen();
  void *cpytitle(void *ptr);
  int Init(char *title);
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
  void List();
  void *de_serialize(void *, char *nam=0){nam=nam;return 0;};

};
//#ifdef __cplusplus
//extern "C"{
//#endif
//  void *cccpc_init(char* n);
//#ifdef __cplusplus
//}
//#endif
#endif
