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
  // int i_typ;
  //double m_fLastCount;
  MonRateBase(const std::string &name, const std::string &tit) :
    MonCounter((char*)name.c_str(),(char*)tit.c_str(),&m_rate,C_RATEDOUBLE) {}
  virtual void makeRate(unsigned long long dt) = 0;
};

template <typename T> class MonRate : public MonRateBase
{
public:
  const T* m_ptr;
  T m_lastCount;
  MonRate(const std::string &name, const std::string &title,  const T &);
  void makeRate(unsigned long long dt);
  void ZeroLast();
};

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
template <typename T> inline void MonRate<T>::makeRate(unsigned long long dt)
{
  m_rate = (double)*m_ptr;
  m_rate = (m_rate - m_lastCount)/dt;
  if (m_rate <0.0)m_rate = 0.0;
  m_rate = m_rate*1.0e9;
  m_lastCount = *m_ptr;
}

template <> inline void MonRate<StatEntity>::makeRate(unsigned long long dt)
{
  m_rate = ((double)(m_ptr->nEntries())-m_lastCount.nEntries())/dt;
  m_rate *= 1.0e09;
  if (m_rate <0.0)m_rate = 0.0;
  this->m_lastCount = *m_ptr;
}


#endif /* MONRATE_H_ */
