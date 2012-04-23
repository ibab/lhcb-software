#ifndef ONLINE_GAUCHO_MEPSVC_H
#define ONLINE_GAUCHO_MEPSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Gaucho/MonAdder.h"
#include "PubSvc.h"
#include "Gaucho/COutService.h"

// Forward declarations
class GenStatSvc : public PubSvc
{
public:
  std::string m_prefix;
  COUTServiceMap m_outmap;
  GenStatSvc(const std::string& name, ISvcLocator* sl);
  virtual ~GenStatSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
//  DetData_T m_DetData;
//  DetData_T m_DetData_diff;
  void analyze(void *buff, int siz,MonMap* mmap);
};
#endif // ONLINE_GAUCHO_MEPSVC_H
