#ifndef CNTER_H
#define CNTER_H
#include "Gaucho/MonTypes.h"
#include "Gaucho/MonObj.h"
#include <vector>
#include <string>
#include "dis.hxx"
#include "Utilities.h"

class CntrDescr
{
public:
  CntrDescr(){ptr = 0;}
  ~CntrDescr(){deallocPtr(ptr);}
  std::string name;
  int type;
  int i_data;
  long long l_data;
  float f_data;
  double d_data;
  void *ptr;
  int nel;
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

  int type() const {return (int)m_type;};
  MonCounter(const std::string& name, const std::string& title, const int *data );
  MonCounter(const std::string& name, const std::string& title, const long *data );
  MonCounter(const std::string& name, const std::string& title, const long long *data );
  MonCounter(const std::string& name, const std::string& title, const float *data );
  MonCounter(const std::string& name, const std::string& title, const double *data);
  MonCounter(const std::string& name, const std::string& title, const float *data, MONTYPE typ );
  MonCounter(const std::string& name, const std::string& title, const double *data, MONTYPE typ );
  MonCounter(const std::string& name, const std::string& title, const std::string& fmt, const void *data , int size);
  MonCounter(const std::string& name, const std::string& title, const int *data , int size);
  MonCounter(const std::string& name, const std::string& title, const float *data , int size);
  MonCounter(const std::string& name, const std::string& title, const long long *data , int size);
  MonCounter(const std::string& name, const std::string& title, const double *data , int size);
  MonCounter();
  void setup(MONTYPE typ, const void *ext, const std::string& name, const std::string& title);
  void setup(MONTYPE typ, const void *data,const std::string& name, const std::string& title,int size);
  virtual ~MonCounter();
  void clear(void);
  void *cpytitle(void *ptr) const;
  void *getextid(void);
  int titlen() const      {    return m_title.length()+1; }
  int namelen() const     {    return m_name.length()+1;  }
  int namelength() const  {    return m_name.length()+1;  }
  int datasize()   const  {    return buffersize;         }
  void *cpyName(void *ptr) const;
  int xmitbuffersize();
  int hdrlen() const;
  int serialize(void* &ptr);
  void List();
  static void *de_serialize(void *, char *nam = 0);
  void SetExpand(bool expand){this->m_expandService = expand;return;};
  void create_OutputService(std::string);
  void delete_OutputService();
  DimService *getDimService() const;
};

#endif
