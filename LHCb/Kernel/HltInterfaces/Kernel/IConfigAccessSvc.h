#ifndef ICONFIGACCESSSVC_H 
#define ICONFIGACCESSSVC_H 1

// Include files
#include <string>
#include "GaudiKernel/INamedInterface.h"
#include "boost/optional.hpp"
#include "Kernel/PropertyConfig.h"
#include "Kernel/ConfigTreeNode.h"
#include "Kernel/ConfigTreeNodeAlias.h"

/** @class IConfigAccessSvc IConfigAccessSvc.h
 *  
 *  functionality:
 *         Interface for obtaining an
 *         individual configuration description
 *         or (an element of) the graph structure 
 *         given its ID
 *
 *         The ID corresponds to the MD5 digest of the contents
 *         of the configration description
 *
 *  @author Gerhard Raven
 *  @date   2007-12-12
 */


class IConfigAccessSvc : virtual public extend_interfaces<INamedInterface>  {
public:
  DeclareInterfaceID(IConfigAccessSvc,3,0);
  virtual ~IConfigAccessSvc() = default;

  // read & write PropertyConfig instances: vertices (aka leafs) in the configuration graph
  virtual boost::optional<PropertyConfig> readPropertyConfig(const PropertyConfig::digest_type& ref) = 0;
  virtual PropertyConfig::digest_type    writePropertyConfig(const PropertyConfig& config) = 0;

  // read & write ConfigTree instances: edges (aka nodes) in the configuration graph
  virtual boost::optional<ConfigTreeNode> readConfigTreeNode(const ConfigTreeNode::digest_type& ref) = 0;
  virtual ConfigTreeNode::digest_type    writeConfigTreeNode(const ConfigTreeNode& edge) = 0;
  //
  // read & write ConfigTreeAlias instances: allow entry into the graph by 'name' (i.e. alias)
  // note that it is not guaranteed that this mapping is, in general, immutable... 
  // (except, by policy, such as for TCK aliases!, and, by construction, TOPLEVEL aliases!)
  // If you need something which is by construction immutable, use the digest instead!
  virtual boost::optional<ConfigTreeNode>  readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type& alias) = 0;
  //@TODO: in case of TOPLEVEL alias, this _must_ verify consistency with any existing configs of the same release/type..
  virtual ConfigTreeNodeAlias::alias_type writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& alias) = 0;

  // introspection: list all aliases in some 'major' category (TCK, TOPLEVEL, TAG)
  // precondition: alias.major() + '/' == alias.str() (? or sould we allow matches more specific than major?)
  virtual std::vector<ConfigTreeNodeAlias> configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& alias) = 0;

  // TODO: gimme all aliases which refer to a given node
  // virtual std::vector<ConfigTreeNodeAlias> configTreeNodeAliases(const ConfigTree::digest_type& ref) = 0;
};
#endif // ICONFIGACCESSSVC_H
