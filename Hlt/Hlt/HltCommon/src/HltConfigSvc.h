// $Id: HltConfigSvc.h,v 1.3 2008-07-04 12:45:33 graven Exp $
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
  void dummyVerifyTCK();
  void verifyTCK();

  typedef unsigned int TCK_t ;
  typedef std::map<std::string,std::string> TCKMap_t;

  TCKMap_t                     m_tck2config;      ///< from TCK to configuration ID
  TCK_t                        m_initialTCK;      ///< which TCK to start with...
  TCK_t                        m_configuredTCK;   ///< which TCK is currently in use?
  IDataProviderSvc            *m_evtSvc;          ///< get Evt Svc to get ODIN (which contains TCK)

  // resolve TCK -> toplevel config ID, then call method with ID
  template <typename RET, typename FUN>
  RET forward(const TCK_t& tck,const FUN& fun) const;

  StatusCode configure(const TCK_t& tck) const {
    typedef StatusCode (PropertyConfigSvc::*fun_t)(const ConfigTreeNode::digest_type&) const;
    fun_t f = &PropertyConfigSvc::configure;
    return forward<StatusCode>(tck,boost::lambda::bind(f,this,boost::lambda::_1));
  }
  StatusCode reconfigure(const TCK_t& tck) const {
    return forward<StatusCode>(tck,boost::lambda::bind(&PropertyConfigSvc::reconfigure,this,boost::lambda::_1)); 
  }
  bool loadConfig(const TCK_t& tck) {
    return forward<bool>(tck,boost::lambda::bind(&PropertyConfigSvc::loadConfig,this,boost::lambda::_1)); 
  }

  ConfigTreeNode::digest_type tck2config(const TCK_t& tk) const;

};
#endif // HLTCONFIGSVC_H
