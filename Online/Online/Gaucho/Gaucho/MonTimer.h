#ifndef ONLINE_GAUCHO_MONTIMER_H
#define ONLINE_GAUCHO_MONTIMER_H

#include "Gaucho/GenTimer.h"
#include "Gaucho/BRTL_Lock.h"
class MonSubSys;

class MonTimer : public GenTimer
{
  MonSubSys *m_Hsys;
public:
  MonTimer(MonSubSys *tis, int period = 10);
  virtual ~MonTimer(void);
  void timerHandler ( void );
  void Stop();
};
#endif  // ONLINE_GAUCHO_MONTIMER_H
