#ifndef ONLINE_GAUCHO_BUSYPUB_H
#define ONLINE_GAUCHO_BUSYPUB_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/MonAdder.h"
#include "PubSvc.h"

// Forward declarations
class DimService;
class ISimpleTrendWriter;

class BusyPub : public PubSvc
{
public:
  BusyPub(const std::string& name, ISvcLocator* sl);
  virtual ~BusyPub();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
  void analyze(void *buff, int siz,MonMap* mmap);
//  unsigned long long m_prevupdate;
  bool m_enableTrending;
  DimService *m_FarmLoad;
  float m_fLoad;
  ISimpleTrendWriter* m_trender;
  IToolSvc *m_isvc ;
  ISvcLocator* m_sl;

};
#endif // ONLINE_GAUCHO_BUSYPUB_H
