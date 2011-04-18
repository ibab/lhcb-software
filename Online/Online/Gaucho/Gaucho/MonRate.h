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
  unsigned long long m_lastCount;
  //double m_fLastCount;
  MonRateBase(const std::string &name, const std::string &tit) :
    MonCounter((char*)name.c_str(),(char*)tit.c_str(),&m_rate), m_lastCount(0) {}
  virtual void makeRate(unsigned long long dt) = 0;
};

template <typename T> class MonRate : public MonRateBase
{
public:
  const T* m_ptr;
  MonRate(const std::string &name, const std::string &title,  const T &);
  void makeRate(unsigned long long dt);
};

template <typename T> MonRate<T>::MonRate(const std::string &name, const std::string &title,  const T & e)
: MonRateBase(name, title), m_ptr(&e)
{
    m_type = C_RATE;
//    i_typ = R_STATENT;
}

template <typename T> inline void MonRate<T>::makeRate(unsigned long long dt)
{
  m_rate = (double)*m_ptr;
  m_rate = (m_rate - m_lastCount)/dt;
  m_rate = m_rate*1.0e9;
  m_lastCount = *m_ptr;
}

template <> inline void MonRate<StatEntity>::makeRate(unsigned long long dt)
{
  m_rate = ((double)(m_ptr->nEntries())-m_lastCount)/dt;
  this->m_lastCount = m_ptr->nEntries();
}


#endif /* MONRATE_H_ */
