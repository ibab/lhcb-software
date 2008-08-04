// $Id: HltConfigSvc.h,v 1.7 2008-08-04 12:09:34 graven Exp $
#ifndef HLTCONFIGSVC_H 
#define HLTCONFIGSVC_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "PropertyConfigSvc.h"


/** @class HltConfig HltConfig.h
 *  
 *  functionality:
 *         configure the HLT algorithms...
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class HltConfigSvc : public PropertyConfigSvc, virtual public IIncidentListener {
public:

  HltConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltConfigSvc( );           ///< Destructor
  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();      ///< Service finalization
  virtual void handle(const Incident&);

private:
  void dummyCheckOdin();
  void checkOdin();

  typedef unsigned int TCK_t ;
  typedef std::map<TCK_t,std::string> TCKMap_t;

  std::vector<TCK_t>           m_prefetchTCK;     ///< which TCK to prefetch
  TCKMap_t                     m_tck2config;      ///< from TCK to configuration ID
  mutable TCKMap_t             m_tck2configCache; ///< from TCK to configuration ID
  TCK_t                        m_initialTCK;      ///< which TCK to start with...
  TCK_t                        m_configuredTCK;   ///< which TCK is currently in use?
  IDataProviderSvc            *m_evtSvc;          ///< get Evt Svc to get ODIN (which contains TCK)
  IIncidentSvc                *m_incidentSvc;     ///< 
  bool                         m_checkOdin;

  // resolve TCK -> toplevel config ID, then call method with ID
  ConfigTreeNode::digest_type tck2id(const TCK_t& tck) const;

};
#endif // HLTCONFIGSVC_H
