#ifndef ONLINE_GAUCHO_MEMTRND_H
#define ONLINE_GAUCHO_MEMTRND_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Gaucho/GenTimer.h"
#include "Gaucho/BRTL_Lock.h"
class ISimpleTrendWriter;
class TrndTimer;
class MemTrnd : public Service
{
public:
  MemTrnd(const std::string& name, ISvcLocator* sl);
  virtual ~MemTrnd();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode initialize();
  StatusCode start();
  StatusCode stop();
  StatusCode finalize();
  int m_interv;
  std::string m_trendfile;
  ISimpleTrendWriter* m_trender;
  IService *m_isvc ;
  std::string m_PName;
  TrndTimer *m_timer;

};
class TrndTimer : public GenTimer
{
public:
  TrndTimer(ISimpleTrendWriter *trender,MemTrnd *mtrend,int period);
  void timerHandler ( void );
  BRTLLock m_lock;
  void Stop();
  ISimpleTrendWriter* m_trender;
  MemTrnd *m_MemTrnd;
};
#endif // ONLINE_GAUCHO_MEMTRND_H
