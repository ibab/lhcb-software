#ifndef BUSYSVC_H
#define BUSYSVC_H

#include "GaudiKernel/Service.h"
#include "Gaucho/GenTimer.h"
class IGauchoMonitorSvc;
class BusySvc;
class IMonitorSvc;
class IdleTimer : public GenTimer
{
public:
  BusySvc *m_bsysvc;
  IdleTimer(BusySvc *master, int period = 5000) : GenTimer(master,period)
  {
    m_bsysvc = master;
  };
  virtual ~IdleTimer();
  void timerHandler(void);
};

class BusySvc : public Service
{
public:
  long m_times_old[7];
  double m_idlebogus;
  double m_busybogus;
  double m_bogus;
  double m_Mybogus;
  int m_NumCores;
  int m_numCores;
  long m_Memtot;
  long m_Memfree;
  long m_Membuff;
  long m_MemSwaptot;
  long m_MemSwapfree;
  bool m_first;
  IMonitorSvc *m_pMonitorSvc;
  IdleTimer *m_timer;
  BusySvc(const std::string& name, ISvcLocator* sl);
  virtual ~BusySvc();
  //IInterface pure virtual member functions
//  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
  void calcIdle();
  void getBogus(double &bogus);
};
#endif // BUSYSVC_H
