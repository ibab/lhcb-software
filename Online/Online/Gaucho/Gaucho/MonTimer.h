#ifndef MONTIMER_H
#define MONTIMER_H
#include "Gaucho/GenTimer.h"
class MonSubSys;

class MonTimer : public GenTimer //: public DimTimer
{
  MonSubSys *m_Hsys;
//  unsigned long long m_dueTime;
public:
  MonTimer(MonSubSys *tis, int period = 10);
  virtual ~MonTimer(void);
  void timerHandler ( void );
  void Stop();
};
#endif
