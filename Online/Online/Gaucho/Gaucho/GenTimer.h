#ifndef GENTIMER_H
#define GENTIMER_H
#include "RTL/rtl.h"
#include "Gaucho/BRTL_Lock.h"
#define TIMER_PREC_SEC 0
#define TIMER_PREC_MILLI 1
#define TIMER_PREC_MICRO 2
#define TIMER_PREC_NANO 3
#define TIMER_TYPE_PERIODIC (1<<0)
#define TIMER_MODIFYER_SYNCHRONIZED (1<<1)

class GenTimer
{
private:
  bool m_periodic;
  bool m_synched;
  bool m_ForceExit;
public:
  void *m_arg;
  unsigned long long  m_period;
  unsigned long long m_dueTime;
  unsigned long long m_lastfire;
  unsigned long long m_lastdelta;
  unsigned long long *m_extlastdelta;
  lib_rtl_thread_t m_thread;
  int m_type;
  void makeDeltaT(void);
public:
  BRTLLock m_lock;
  bool m_dontdimlock;
  GenTimer(void *arg, int period = 10000,int typ=TIMER_TYPE_PERIODIC+TIMER_MODIFYER_SYNCHRONIZED);
// Period is in milli-seconds...
  virtual ~GenTimer(void);
  virtual void timerHandler ( void )=0;
  void Start();
  void Stop();
  void setExtLastDelta(unsigned long long *a){m_extlastdelta = a;return;};
  unsigned long long getDeltaTime(int incr=0);
  void *ThreadRoutine();
  unsigned long long GetLastDelta(){return m_lastdelta;};
};
#endif
