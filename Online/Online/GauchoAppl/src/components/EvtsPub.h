#ifndef ONLINE_GAUCHO_EVTSPUB_H
#define ONLINE_GAUCHO_EVTSPUB_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GauchoAppl/MonAdder.h"
#include "PubSvc.h"

// Forward declarations
class DimService;
//class ISimpleTrendWriter;

class EvtsPub : public PubSvc
{
public:
  EvtsPub(const std::string& name, ISvcLocator* sl);
  virtual ~EvtsPub();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
  StatusCode finalize();
  void analyze(void *buff, int siz,MonMap* mmap);
//  unsigned long long m_prevupdate;
  bool m_enableTrending;

  DimService *m_noEvtsSvc;
  DimService *m_EvtRateSvc;

  int m_NoEvts;
  double m_EvtRate;
  std::string m_SvcInfix;
//  ISimpleTrendWriter* m_trender;
};
#endif // ONLINE_GAUCHO_EVTSPUB_H
