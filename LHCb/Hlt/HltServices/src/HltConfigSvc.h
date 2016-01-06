#ifndef HLTCONFIGSVC_H
#define HLTCONFIGSVC_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include "Kernel/TCK.h"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PropertyConfigSvc.h"


/** @class HltConfig HltConfig.h
 *
 *  functionality:
 *         configure the HLT algorithms...
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class HltConfigSvc : public extends1<PropertyConfigSvc, IIncidentListener> {
public:

  HltConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  ~HltConfigSvc( ) override = default; ///< Destructor
  StatusCode initialize() override;    ///< Service initialization
  StatusCode start() override;    ///< Service initialization
  StatusCode finalize() override;      ///< Service finalization
  void handle(const Incident&) override;

private:
  void dummyCheckOdin();
  void checkOdin();
  void createHltDecReports();

  void updateMap(Property&);
  void updateInitial(Property&);

  typedef std::map<TCK,std::string> TCKMap_t;

  std::vector<std::string>     m_outputContainerName;  ///< location of HltDecReports in which to record configured TCK
  std::string                  m_prefetchDir;     ///< which set of configurations
                                                  ///< to search for same types as initial TCK
                                                  ///< and to prefetch...

  TCKMap_t                          m_tck2config;      ///< from TCK to configuration ID
  std::map<std::string,std::string> m_tck2config_;     ///< from TCK to configuration ID

  TCK                          m_initialTCK;      ///< which TCK to start with...
  std::string                  m_initialTCK_;     ///< which TCK to start with...

  mutable TCKMap_t             m_tck2configCache; ///< from TCK to configuration ID
  TCK                          m_configuredTCK { 0 } ;   ///< which TCK is currently in use?
  SmartIF<IDataProviderSvc>    m_evtSvc ;          ///< get Evt Svc to get ODIN (which contains TCK)
  SmartIF<IIncidentSvc>        m_incidentSvc;     ///<
  ToolHandle<IGenericTool>     m_decodeOdin;
  bool                         m_checkOdin;
  bool                         m_maskL0TCK;
  unsigned int                 m_id = ~0u;
  bool                         m_hlt2mode;

  // resolve TCK -> toplevel config ID, then call method with ID
  ConfigTreeNode::digest_type tck2id(TCK    tck) const;

};
#endif // HLTCONFIGSVC_H
