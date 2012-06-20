#ifndef ONLINE_GAUCHO_RATESVC_H
#define ONLINE_GAUCHO_RATESVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "PubSvc.h"
#include "GauchoAppl/COutService.h"
#include <vector>
// Forward declarations
class MonAdder;
class AdderSys;
class SaveTimer;
class IMonitorSvc;
class MyErrh;
class IIncidentSvc;
class IGauchoMonitorSvc;
class IHistogramSvc;
class OUTServiceDescr
{
public:
  std::string m_serviceName;
  long long last_update;
  void *m_svc;
  void *m_buffer;
  int m_buffersize;
  OUTServiceDescr(char *name, void *srvc)
  {
    m_serviceName = std::string(name);
    m_svc = srvc;//new ObjService(m_serviceName.c_str(),(char*)"C",&empty,4,&m_buffer,&m_buffersize);
  }
  ~OUTServiceDescr()
  {
//    delete m_svc;
  }
};
typedef std::map<std::string, OUTServiceDescr*> OUTServiceMap;
typedef std::pair<std::string, OUTServiceDescr*> OUTServicePair;
typedef OUTServiceMap::iterator OUTServIter;
//class ISimpleTrendWriter;

class RateSvc : public PubSvc
{
public:
  RateSvc(const std::string& name, ISvcLocator* sl);
  virtual ~RateSvc();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode stop();
  void makerate(MonMap* mmap);
  void makecounters(MonMap* mmap);
  void *m_oldProf;
  void analyze(void *buff, int siz,MonMap* mmap);
  OUTServiceMap m_outputServicemap;
  OUTServiceDescr *findOUTService(std::string servc);
  std::string m_NamePrefix;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
//  ISimpleTrendWriter* m_trender;
  bool m_enableTrending;
  COUTServiceMap m_outmap;
private:
  MyErrh *m_errh;

  unsigned long long m_prevupdate;
};
#endif // ONLINE_GAUCHO_RATESVC_H
