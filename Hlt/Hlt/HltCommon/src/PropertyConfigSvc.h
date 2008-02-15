// $Id: PropertyConfigSvc.h,v 1.3 2008-02-15 08:20:01 graven Exp $
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
#include "HltBase/IPropertyConfigSvc.h"
#include "HltBase/PropertyConfig.h"


/** @class PropertyConfigSvc PropertyConfigSvc.h
 *  
 *  functionality:
 *         configure algorithms, services and tools...
 *
 *         TODO: split into two seperate svcs, 
 *               one for getting configuration info (using the 
 *               access svc for I/O) and navigating through it,
 *               and a second, which uses the first,
 *               to actually _use_ it to configure
 *               an application..
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class PropertyConfigSvc : public Service,
                          virtual public IPropertyConfigSvc {
public:
  PropertyConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PropertyConfigSvc( ); ///< Destructor
  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();    ///< Service initialization

  virtual PropertyConfig currentConfiguration(const INamedInterface& obj) const;
  virtual PropertyConfig::digest_type findInTree(const ConfigTreeNode::digest_type& configTree, const std::string& name) const;


protected:
  // helper functions
  const std::list<ConfigTreeNode::digest_type>& collectNodeRefs(const ConfigTreeNode::digest_type& nodeRef) const;
  const std::vector<PropertyConfig::digest_type>& collectLeafRefs(const ConfigTreeNode::digest_type& nodeRef) const;

  bool loadConfig(const ConfigTreeNode::digest_type& nodeRef);

  /// given a configuration, configure the matching algorithm, service, tool
  StatusCode configure(const PropertyConfig& config) const;
  // resolve the ID to get a list of  configurations, then use them to configure
  StatusCode configure(const ConfigTreeNode::digest_type& configID) const;
  // reconfigure: first configure, then call sysReinitialize on the top algorithm
  StatusCode reconfigure(const ConfigTreeNode::digest_type& top) const; 

  // populate cache with specified objects, and return pointer to it
  const PropertyConfig* resolvePropertyConfig(const PropertyConfig::digest_type& ref) const;
  const ConfigTreeNode* resolveConfigTreeNode(const ConfigTreeNode::digest_type& ref) const;

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
  typedef std::map<PropertyConfig::digest_type,  PropertyConfig> PropertyConfigMap_t;
  typedef std::map<ConfigTreeNode::digest_type,  ConfigTreeNode> ConfigTreeNodeMap_t;

  //TODO: use multimap instead???
  typedef std::map<ConfigTreeNode::digest_type,  std::vector<PropertyConfig::digest_type> > Tree2LeafMap_t;
  typedef std::map<ConfigTreeNode::digest_type,  std::list<ConfigTreeNode::digest_type> > Tree2NodeMap_t;
  
  mutable std::auto_ptr<MsgStream>     m_msg;
  std::string                          s_accessSvc;
  IJobOptionsSvc*                      m_joboptionsSvc;
  IAlgManager*                         m_appMgr;
  IConfigAccessSvc*                    m_accessSvc;
  mutable PropertyConfigMap_t          m_configs;  // config ref -> config (leaf)
  mutable ConfigTreeNodeMap_t          m_nodes;    // node   ref -> node
  mutable Tree2LeafMap_t               m_leavesInTree; // top level node ref -> config refs (leaves)
  mutable Tree2NodeMap_t               m_nodesInTree; // top level node ref -> node refs
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
