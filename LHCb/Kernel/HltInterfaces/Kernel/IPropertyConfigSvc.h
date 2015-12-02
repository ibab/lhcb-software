#ifndef IPROPERTYCONFIGSVC_H 
#define IPROPERTYCONFIGSVC_H 1

// Include files
#include <vector>
#include <list>
#include "GaudiKernel/INamedInterface.h"
#include "Kernel/ConfigTreeNode.h"
#include "Kernel/ConfigTreeNodeAlias.h"
#include "Kernel/PropertyConfig.h"


/** @class IPropertyConfigSvc IPropertyConfigSvc.h
 *  
 *  functionality:
 *         Interface for obtaining information about configurations 
 *
 *
 *  @author Gerhard Raven
 *  @date   2008-01-01
 */


class IPropertyConfigSvc : virtual public extend_interfaces<INamedInterface>  {
public:
  DeclareInterfaceID(IPropertyConfigSvc,2,0);
  virtual ~IPropertyConfigSvc() = default;

  // Return the configuration object for this object...
  virtual PropertyConfig currentConfiguration(const INamedInterface& obj) const = 0;

  // Return the configuration ID for this object...
  PropertyConfig::digest_type currentConfigurationID(const INamedInterface& obj) const 
  { return currentConfiguration(obj).digest(); }

  // Search a configuration tree, specified by 'configTree', for the configuration ID of the 
  // object with name 'name'
  virtual PropertyConfig::digest_type findInTree(const ConfigTreeNode::digest_type& configTree, const std::string& name) const = 0;

  PropertyConfig::digest_type findInTree(const ConfigTreeNode::digest_type& configTree, const INamedInterface& iface) const
  { return findInTree(configTree,iface.name()); }

  // populate cache with specified objects, and return pointer to it
  // Note: the service remains the owner of these objects!!!
  virtual const PropertyConfig* resolvePropertyConfig(const PropertyConfig::digest_type& ref) const = 0;
  virtual const ConfigTreeNode* resolveConfigTreeNode(const ConfigTreeNode::digest_type& ref) const = 0;
  virtual const ConfigTreeNode* resolveConfigTreeNode(const ConfigTreeNodeAlias::alias_type& alias) const = 0;

  //TODO: these four need to move to the IConfigAccessSvc layer to allow for
  //      bulk transaction optimization... note: only allow for alias in that case?
  virtual const std::list<ConfigTreeNode::digest_type>& collectNodeRefs(const ConfigTreeNode::digest_type& nodeRef) const = 0;
  virtual const std::list<ConfigTreeNode::digest_type>& collectNodeRefs(const ConfigTreeNodeAlias::alias_type& alias) const = 0;
  virtual const std::vector<PropertyConfig::digest_type>& collectLeafRefs(const ConfigTreeNode::digest_type& nodeRef) const = 0;
  virtual const std::vector<PropertyConfig::digest_type>& collectLeafRefs(const ConfigTreeNodeAlias::alias_type& alias) const = 0;





  // virtual std::vector<MD5::Digest> topLevelConfigsUsing(const CfgID_t& name) const = 0;
  //
  // return name of the object with the specified id in n, provided id is a valid CfgID
  // virtual StatusCode name(const CfgID_t& toplevelConfig, std::string& n) const = 0 ;

  // return type of the object with the specified id in n, provided id is a valid CfgID
  // virtual StatusCode type(const CfgID_t& id, std::string& t) const = 0 ;

  // return string value of the property of the object specified by id
  //  and key key, provided id is a valid CfgID, and the object has a property
  //  with the specified key
  // virtual StatusCode propertyAsString(const CfgID_t& id, const std::string& key,
  //                                    std::string& property) const = 0;
  // bool template<typename T> property(const CfgID_t& id, T& prop) const;
};
#endif // IPROPERTYCONFIGSVC_H
