#ifndef ONLINE_GAUCHO_CNTRPUB_H
#define ONLINE_GAUCHO_CNTRPUB_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GauchoAppl/MonAdder.h"
#include "PubSvc.h"
#include <boost/regex.hpp>


// Forward declarations
class DimService;
typedef struct
{
    int type;
    std::string name;
    int idata;
    float rdata;
    DimService *svc;
}SVCDescr;
class CntrPub: public PubSvc {
public:
  CntrPub(const std::string& name, ISvcLocator* sl);
  virtual ~CntrPub();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
  StatusCode finalize();
  void analyze(void *buff, int siz, MonMap* mmap);

  DimService *m_noEvtsSvc;
  DimService *m_EvtRateSvc;
  std::string m_counterPattern;
  boost::regex m_counterPatternRegex;
  std::map<std::string,SVCDescr*> m_cntrSvcMap;
  int m_NoEvts;
  double m_EvtRate;
  std::string m_SvcInfix;
  bool m_enableTrending;
};
#endif // ONLINE_GAUCHO_CNTRPUB_H
