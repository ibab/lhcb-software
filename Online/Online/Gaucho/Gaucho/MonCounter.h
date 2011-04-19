#ifndef CNTER_H
#define CNTER_H
#include "Gaucho/MonTypes.h"
#include "Gaucho/MonObj.h"
#include <vector>
#include <string>
#include "dim/dis.hxx"
#include "Utilities.h"

class CntrDescr
{
public:
  std::string name;
  int type;
  long long data;
};

class MonCounter : public MonObj
{
//friend class HistService;
protected:
  MONTYPE m_type;
  const void *m_contents;
  std::string m_name;
  std::string m_title;
  int buffersize;     /* buffer size of the data if serialized*/
  unsigned int m_contsiz;      /* Allocated length in Bytes for the bin contents */
  int m_addoff;

public:
  const char *name() const { return m_name.c_str(); }
  bool m_expandService;
  DimService *m_service;
  std::string m_srvcprefix;
  std::string m_fmt;
  //HistService *serv;
  int type() const {return (int)m_type;};
  MonCounter(const char *name, const char *title, const int *data );
  MonCounter(const char *name, const char *title, const long *data );
  MonCounter(const char *name, const char *title, const long long *data );
  MonCounter(const char *name, const char *title, const float *data );
  MonCounter(const char *name, const char *title, const double *data );
  MonCounter(const char *name, const char *title, const std::string& fmt, const void *data , int size);
  MonCounter();
  void setup(MONTYPE typ, const void *ext, const char *name, const char *title);
  virtual ~MonCounter();
  //int setname (const char* name);
  void clear(void);
  int titlen();
  int namelen();
  void *cpytitle(void *ptr);
  //int Init(const char *title);
  void *getextid (void);
  int namelength(void) const  {    return m_name.length()+1;  }
  void *cpyName(void *ptr);
  int datasize();
  int xmitbuffersize();
  int hdrlen();
  int serialize(void* &ptr);
  void List();
  static void *de_serialize(void *, char *nam = 0);
  void SetExpand(bool expand){this->m_expandService = expand;return;};
  void create_OutputService(std::string);
  void delete_OutputService();
  DimService *getDimService();
};

#endif
