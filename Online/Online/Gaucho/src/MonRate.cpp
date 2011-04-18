/*
 * MonRate.cpp
 *
 *  Created on: Apr 14, 2011
 *      Author: beat
 */
#include "Gaucho/MonRate.h"
#if 0

MonRate::MonRate(char *name, char *title, int &se) : MonCounter(name, title,&m_rate)
{

}
MonRate::MonRate(char *name, char *title, StatEntity &se) : MonCounter(name, title,&m_rate)
MonRate::MonRate(char *name, char *title, StatEntity &se) : MonCounter(name, title,&m_rate)
MonRate::MonRate(char *name, char *title, StatEntity &se) : MonCounter(name, title,&m_rate)
{
  m_statent = &se;
  this->m_LastCount = se.nEntries();
  m_type = C_RATE;
  i_typ = R_STATENT;
}
void MonRate::makeRate(unsigned long long dt)
{
  switch (i_typ)
  {
    case: R_STATENT:
    {
      m_rate = (double)(m_statent->nEntries())-m_LastCount)/dt;
      this->m_LastCount = m_statent->nEntries();
      break;
    }
    case R_INT:
    {
      m_rate = (double(*m_datptr)-m_LastCount)/dt;
      this->m_LastCount = *m_datptr;
      break;
    }
    case R_FLOAT:
    {
      m_rate = (double(*m_datptr)-m_fLastCount)/dt;
      this->m_fLastCount = (float)*m_datptr;
      break;
    }
    case DOUBLE:
    {
      m_rate = (double(*m_datptr)-m_LastCount)/dt;
      this->m_LastCount = *m_datptr;
      break;
    }
  }
}
#endif
