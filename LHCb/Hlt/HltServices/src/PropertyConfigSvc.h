// $Id: PropertyConfigSvc.h,v 1.1 2010-05-05 13:20:44 graven Exp $
#ifndef PROPERTYCONFIGSVC_H 
#define PROPERTYCONFIGSVC_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iterator>
#include "boost/optional.hpp"


// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/VectorMap.h"

// from HltInterfaces
#include "Kernel/IConfigAccessSvc.h"
#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/PropertyConfig.h"


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
class PropertyConfigSvc : public extends<Service,IPropertyConfigSvc>
                        , public IToolSvc::Observer {
private:
  typedef std::map<PropertyConfig::digest_type,  PropertyConfig> PropertyConfigMap_t;
  typedef std::map<ConfigTreeNode::digest_type,  ConfigTreeNode> ConfigTreeNodeMap_t;
  typedef std::map<ConfigTreeNodeAlias::alias_type,  ConfigTreeNode::digest_type> ConfigTreeNodeAliasMap_t;

  //TODO: use multimap instead???
  typedef std::map<ConfigTreeNode::digest_type,  std::vector<PropertyConfig::digest_type> > Tree2LeafMap_t;
  typedef std::map<ConfigTreeNode::digest_type,  std::list<ConfigTreeNode::digest_type> > Tree2NodeMap_t; // replace std::list with std::deque???
  
  typedef std::map<std::string, PropertyConfig::digest_type> ConfigPushed_t;
public:
  PropertyConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  ~PropertyConfigSvc( ) override  = default ; ///< Destructor
  StatusCode initialize() override;    ///< Service initialization
  StatusCode finalize() override;    ///< Service initialization

  PropertyConfig currentConfiguration(const INamedInterface& obj) const override;
  PropertyConfig::digest_type findInTree(const ConfigTreeNode::digest_type& configTree, const std::string& name) const override;

  const Tree2NodeMap_t::mapped_type& collectNodeRefs(const ConfigTreeNode::digest_type& nodeRef) const override;
  const Tree2NodeMap_t::mapped_type& collectNodeRefs(const ConfigTreeNodeAlias::alias_type& nodeRef) const override;
  const Tree2LeafMap_t::mapped_type& collectLeafRefs(const ConfigTreeNode::digest_type& nodeRef) const override;
  const Tree2LeafMap_t::mapped_type& collectLeafRefs(const ConfigTreeNodeAlias::alias_type& nodeRef) const override;

  const PropertyConfig* resolvePropertyConfig(const PropertyConfig::digest_type& ref) const override;
  const ConfigTreeNode* resolveConfigTreeNode(const ConfigTreeNode::digest_type& ref) const override;
  const ConfigTreeNode* resolveConfigTreeNode(const ConfigTreeNodeAlias::alias_type& ref) const override;
  // helper functions

  // preload a configuration
  StatusCode loadConfig(const ConfigTreeNode::digest_type& nodeRef);
  // resolve the ID to get a list of  configurations, then use them to configure
  using Service::configure;  
  StatusCode configure(const ConfigTreeNode::digest_type& configID, bool callSetProperties) const;
  StatusCode configure(const ConfigTreeNodeAlias::alias_type& alias, bool callSetProperties) const;
  // reconfigure: first configure, then call sysReinitialize on the top algorithm
  StatusCode reconfigure(const ConfigTreeNode::digest_type& top) const; 

  //               component             property               regex        replacement
  typedef std::map<std::string, std::map<std::string, std::map< std::string, std::string> > > TransformMap;

protected:
//
//   component name
//             property name
//                       replacement rule
//   { 'GaudiSequencer/.*' : { 'Members' : { "'PatPV3D/HltPVsPV3D'" : "'PatPV3D/HltPVsPV3D', 'HltMoveVerticesForSwimming/HltMovePVs4Swimming'" } } }
//
  class Transformer {
   public:
       Transformer(const std::string& component,MsgStream& os): m_os(os), m_c(component) {}
       void push_back( const std::map<std::string, std::map<std::string,std::string> >* pmap ) { m_list.push_back(pmap); }
       bool empty() const { return m_list.empty(); }
       PropertyConfig::Prop operator()(const PropertyConfig::Prop& in);
   private:
       typedef std::map<std::string,std::string> ReplMap_t;
       typedef std::map<std::string, ReplMap_t > PropMap_t;
       typedef std::vector< const PropMap_t* >  List_t;
       List_t  m_list;
       MsgStream&  m_os;
       std::string m_c;
  };
  
  // check validity of given config
  StatusCode validateConfig(const PropertyConfig::digest_type& ref) const;

  IConfigAccessSvc* cas() const { return m_accessSvc;}


  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }


private:
  TransformMap                         m_transform;

  mutable std::unique_ptr<MsgStream>   m_msg;
  std::string                          s_accessSvc;
  SmartIF<IJobOptionsSvc>              m_joboptionsSvc;
  SmartIF<IToolSvc>                    m_toolSvc;
  SmartIF<IAlgManager>                 m_algMgr;
  SmartIF<IAppMgrUI>                   m_appMgrUI;
  SmartIF<IConfigAccessSvc>            m_accessSvc;
  mutable PropertyConfigMap_t          m_configs;  // config ref -> config (leaf)
  mutable ConfigTreeNodeMap_t          m_nodes;    // node   ref -> node
  mutable ConfigTreeNodeAliasMap_t     m_aliases;    // node   ref -> node
  mutable Tree2LeafMap_t               m_leavesInTree; // top level node ref -> config refs (leaves)
  mutable Tree2NodeMap_t               m_nodesInTree; // top level node ref -> node refs
  mutable ConfigPushed_t               m_configPushed;
  std::map<std::string,const IAlgTool*> m_toolmap;
  std::vector<std::string>             m_prefetch;    ///< configurations to load at initialization
  std::vector<std::string>             m_skip;        ///< items NOT to configure with this service
  std::string                          m_ofname;
  std::unique_ptr<std::ostream>        m_os;
  bool                                 m_createGraphVizFile;
  bool                                 m_allowFlowChanges;
  mutable std::unique_ptr<std::vector<std::string> > m_initialTopAlgs;

  MsgStream& msg(MSG::Level level) const;

  void onCreate(const IAlgTool* tool);

  template <typename T> T* resolve(const std::string& name) const;
  StatusCode invokeSetProperties(const PropertyConfig& config) const;

  StatusCode setTopAlgs(const ConfigTreeNode::digest_type& id) const;
  StatusCode findTopKind(const ConfigTreeNode::digest_type& configID,
                         const std::string& kind,
                         std::back_insert_iterator<std::vector<const PropertyConfig*> > components) const;
  void createGraphVizFile(const PropertyConfig::digest_type& ref, const std::string& fname) const;
  ConfigTreeNode::digest_type resolveAlias(const ConfigTreeNodeAlias::alias_type& alias) const;
  StatusCode outOfSyncConfigs(const ConfigTreeNode::digest_type& top, 
                              std::back_insert_iterator<std::vector<const PropertyConfig*> >  ) const;
};
#endif // PROPERTYCONFIGSVC_H
