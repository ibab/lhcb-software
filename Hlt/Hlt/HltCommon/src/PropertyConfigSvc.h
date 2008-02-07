// $Id: PropertyConfigSvc.h,v 1.1 2008-02-07 16:36:58 graven Exp $
#ifndef PROPERTYCONFIGSVC_H 
#define PROPERTYCONFIGSVC_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "boost/optional.hpp"
// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/VectorMap.h"

// from HltBase
#include "HltBase/IConfigAccessSvc.h"
#include "HltBase/PropertyConfig.h"


/** @class PropertyConfig PropertyConfig.h
 *  
 *  functionality:
 *         configure the HLT algorithms...
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class PropertyConfigSvc : public Service  {
public:
  PropertyConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PropertyConfigSvc( ); ///< Destructor
  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();    ///< Service initialization

protected:
  /// given a configuration, configure the algorithm
  StatusCode configure(const PropertyConfig& config) const;
  // resolve the ID to get a configuration, then use it to configure
  StatusCode configure(const PropertyConfig::digest_type& configID) const;
  // reconfigure: first configure, then call sysReinitialize on the top algorithm
  StatusCode reconfigure(const PropertyConfig::digest_type& top) const; 

  // populate cache with specified config
  bool loadConfig(const PropertyConfig::digest_type& ref);

  // check validity of given config
  bool validateConfig(const PropertyConfig::digest_type& ref) const;


  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }
private:
  typedef std::map<PropertyConfig::digest_type,  PropertyConfig> ConfigMap_t;
  
  mutable std::auto_ptr<MsgStream>     m_msg;
  std::string                          s_accessSvc;
  IJobOptionsSvc*                      m_joboptionsSvc;
  IAlgManager*                         m_appMgr;
  IConfigAccessSvc*                    m_accessSvc;
  ConfigMap_t                          m_configs;
  std::vector<std::string>             m_prefetch;    ///< configurations to load at initialization
  std::vector<std::string>             m_skip;        ///< items NOT to configure with this service
  std::string                          m_ofname;
  std::auto_ptr<std::ostream>          m_os;
  bool                                 m_createGraphVizFile;

  MsgStream& msg(MSG::Level level) const;

  StatusCode findServicesAndTopAlgorithms(const PropertyConfig::digest_type&top,
                                          std::vector<IService*>& svcs,
                                          std::vector<IAlgorithm*>& algs) const;
  void createGraphVizFile(const PropertyConfig::digest_type& ref, const std::string& fname) const;
};
#endif // PROPERTYCONFIGSVC_H
