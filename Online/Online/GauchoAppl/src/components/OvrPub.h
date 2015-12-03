#ifndef ONLINE_GAUCHO_OVRPUB_H
#define ONLINE_GAUCHO_OVRPUB_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GauchoAppl/MonAdder.h"
#include "PubSvc.h"

// Forward declarations
class DimService;
//class ISimpleTrendWriter;

class OvrPub : public PubSvc
{
public:
  OvrPub(const std::string& name, ISvcLocator* sl);
  virtual ~OvrPub();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
  StatusCode finalize();
  void analyze(void *buff, int siz,MonMap* mmap);
//  unsigned long long m_prevupdate;
  bool m_enableTrending;

  DimService *m_NoBytesSvc;
  DimService *m_NoFilesSvc;
  DimService *m_NoMEPsSvc;

  float m_NoBytes;
  float m_NoFiles;
  float m_NoMEPs;
  double m_MEPRate;
//  ISimpleTrendWriter* m_trender;
};
#endif // ONLINE_GAUCHO_OVRPUB_H
