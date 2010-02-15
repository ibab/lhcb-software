#pragma once
#include "dis.hxx"
//#include "CCPCHsys.h"
class CCPCSubSys;
class HistTimer : public DimTimer
{
  CCPCSubSys *m_Hsys;
  int m_period;
  long long m_dueTime;
  int getDeltaTime(void);
public:
  HistTimer(CCPCSubSys *tis, int period = 10);
  virtual ~HistTimer(void);
  void timerHandler ( void );
  void Start();
};
