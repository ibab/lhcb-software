#ifndef ONLINE_GAUCHO_ADDTIMER_H
#define ONLINE_GAUCHO_ADDTIMER_H

#include "Gaucho/GenTimer.h"
#include "Gaucho/BRTL_Lock.h"
class MonAdder;
class AddTimer : public GenTimer
{
  MonAdder *m_owner;
public:
  AddTimer(MonAdder *tis, int period = 5, int typ = 0);
  virtual ~AddTimer(void);
  void timerHandler ( void );
  void Stop();
};
#endif  // ONLINE_GAUCHO_ADDTIMER_H
