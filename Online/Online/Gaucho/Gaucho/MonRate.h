/*
 * MonRate.h
 *
 *  Created on: Apr 14, 2011
 *      Author: beat
 */

#include "Gaucho/MonCounter.h"
#include "GaudiKernel/StatEntity.h"
#ifndef MONRATE_H_
#define MONRATE_H_
enum
{
  R_INT,
  R_FLOAT,
  R_DOUBLE,
  R_STATENT
};

class MonRateBase : public MonCounter
{
public:
  double m_rate;
  double *m_ratearr;
  int m_arrsiz;
  int m_nel;
  MONTYPE m_eltyp;
  // int i_typ;
  //double m_fLastCount;
  virtual ~MonRateBase()
  {
    deletePtr(m_ratearr);
    m_arrsiz = 0;
  }
  MonRateBase(const std::string &name, const std::string &tit) :
    MonCounter((char*)name.c_str(),(char*)tit.c_str(),&m_rate,C_RATEDOUBLE), m_ratearr(0){}
  MonRateBase(const std::string &name, const std::string &tit, const std::string fmt,const int siz):m_ratearr(0)
  {
    m_type  = C_RATEDOUBLESTAR;
    m_contsiz    = 0;
    m_addoff = 0;
    m_service = 0;
    m_name = name;
    m_title = tit;
    m_srvcprefix = "";
    int allsiz;
    char fch = fmt[0];
    fch = tolower(fch);
    switch (fch)
    {
      case 'x':
      {
        m_eltyp = C_LONGLONG;
        m_nel = siz/sizeof(long long);
        break;
      }
      case 'i':
      {
        m_eltyp = C_INT;
        m_nel = siz/sizeof(int);
        break;
      }
      case 'f':
      {
        m_eltyp = C_FLOAT;
        m_nel = siz/sizeof(float);
        break;
      }
      case 'd':
      {
        m_eltyp = C_DOUBLE;
        m_nel = siz/sizeof(double);
        break;
      }
      default:
      {
        m_eltyp = H_ILLEGAL;
        m_type = H_ILLEGAL;
        m_nel = 0;
        m_contsiz = 0;
        buffersize = 0;
        break;
      }
    }
    allsiz = m_nel*sizeof(double);
    m_contsiz = allsiz;
    buffersize = allsiz;
    m_arrsiz = allsiz;
    m_ratearr = (double*)malloc(allsiz);
    m_contents = m_ratearr;
  }
  virtual void makeRate(unsigned long long dt) = 0;
  virtual void print()=0;
};
template <typename T> class MonRateA : public MonRateBase
{
public:
  T m_ptr;
  T m_lastCount;
  int m_ssiz;
  MonRateA(const std::string &name, const std::string &tit, const std::string fmt, T dat, const int siz);
  ~MonRateA()
  {
    deletePtr(m_ratearr);
    m_arrsiz = 0;
    deletePtr(m_lastCount);
  }
  void makeRate(unsigned long long dt);
  void ZeroLast();
  void print();
};
template <typename T> class MonRate : public MonRateBase
{
public:
  const T* m_ptr;
  T m_lastCount;
  int m_ssiz;
  MonRate(const std::string &name, const std::string &title,  const T &);
  MonRate(const std::string &name, const std::string &tit, const std::string fmt, T dat, const int siz);
  ~MonRate()
  {
  }
  void makeRate(unsigned long long dt);
  void ZeroLast();
  void print();
};

template <typename T> void MonRateA<T>::print()
{
  int i;
  for (i=0;i<m_nel;i++)
  {
    printf("%s %d %f\n",m_name.c_str(),i,m_ratearr[i]);
  }
}
template <typename T> void MonRate<T>::print()
{
  printf("%s %f\n",this->m_name.c_str(),m_rate);
}
template <typename T> MonRateA<T>::MonRateA(const std::string &name, const std::string &tit, const std::string fmt,const T dat, const int siz):
    MonRateBase(name,tit,fmt,siz)
{
  m_ptr = dat;
  m_lastCount = (T)malloc(siz);
  m_ssiz = siz;
  ZeroLast();
}
template <typename T> MonRate<T>::MonRate(const std::string &name, const std::string &title,  const T & e) : MonRateBase(name, title), m_ptr(&e)
{
    ZeroLast();
}
template <typename T> inline void MonRate<T>::ZeroLast()
{
  m_lastCount = 0;
}
template <> inline void MonRate<StatEntity>::ZeroLast()
{
  m_lastCount.reset();
}
template <typename T> inline void MonRateA<T>::ZeroLast()
{
  memset(m_lastCount,0,m_ssiz);
}

template <typename T> inline void MonRateA<T>::makeRate(unsigned long long dt)
{
  int i;
  for (i=0;i<m_nel;i++)
  {
    long long tmp;
    tmp = m_ptr[i]-m_lastCount[i];
    m_ratearr[i] = (double)(tmp);
    m_ratearr[i] = (m_ratearr[i])/dt;
    m_ratearr[i] *= 1.0e9;
    if (m_ratearr[i]<0.0)m_ratearr[i] = 0;
  }
  memcpy(m_lastCount,m_ptr,m_ssiz);
}


template <typename T> inline void MonRate<T>::makeRate(unsigned long long dt)
{
  T tmp;
  tmp = *m_ptr-m_lastCount;
  m_rate = (double)(tmp);
  m_rate = m_rate/dt;
  if (m_rate <0.0)m_rate = 0.0;
  m_rate = m_rate*1.0e9;
  m_lastCount = *m_ptr;
}

template <> inline void MonRate<StatEntity>::makeRate(unsigned long long dt)
{
  long long tmp;
  tmp = m_ptr->nEntries()-m_lastCount.nEntries();
  m_rate = (double)tmp;
  m_rate = m_rate/dt;
  m_rate *= 1.0e09;
  if (m_rate <0.0)m_rate = 0.0;
  this->m_lastCount = *m_ptr;
}


#endif /* MONRATE_H_ */
