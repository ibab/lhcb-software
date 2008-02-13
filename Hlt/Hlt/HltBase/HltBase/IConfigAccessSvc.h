// $Id: IConfigAccessSvc.h,v 1.3 2008-02-13 14:52:30 graven Exp $
#ifndef ICONFIGACCESSSVC_H 
#define ICONFIGACCESSSVC_H 1

// Include files
#include <string>
#include "GaudiKernel/INamedInterface.h"
#include "boost/optional.hpp"
#include "HltBase/PropertyConfig.h"
#include "HltBase/ConfigTreeNode.h"


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


class IConfigAccessSvc : virtual public INamedInterface  {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID();
  virtual ~IConfigAccessSvc();

  // read & write PropertyConfig instances: vertices (aka leafs) in the configuration graph
  virtual boost::optional<PropertyConfig> readPropertyConfig(const PropertyConfig::digest_type& ref) = 0;
  virtual PropertyConfig::digest_type    writePropertyConfig(const PropertyConfig& config) = 0;

  // read & write ConfigTree instances: edges (aka nodes) in the configuration graph
  virtual boost::optional<ConfigTreeNode> readConfigTreeNode(const ConfigTreeNode::digest_type& ref) = 0;
  virtual ConfigTreeNode::digest_type    writeConfigTreeNode(const ConfigTreeNode& edge) = 0;
};
#endif // ICONFIGACCESSSVC_H
