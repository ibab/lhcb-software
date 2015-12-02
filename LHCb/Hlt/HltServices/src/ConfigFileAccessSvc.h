// $Id: ConfigFileAccessSvc.h,v 1.1 2010-05-05 13:20:44 graven Exp $
#ifndef CONFIGFILEACCESSSVC_H 
#define CONFIGFILEACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include "boost/optional.hpp"
#include "boost/filesystem/path.hpp"
// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include "Kernel/IConfigAccessSvc.h"

/** @class ConfigFileAccessSvc ConfigFileAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to files
 *
 *  @author Gerhard Raven
 *  @date   2007-12-14
 */
class ConfigFileAccessSvc : public extends1<Service, IConfigAccessSvc> {
public:
  ConfigFileAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~ConfigFileAccessSvc( ) override = default;     ///< Destructor

  StatusCode initialize();    ///< Service initialization
  StatusCode finalize();      ///< Service initialization

  boost::optional<PropertyConfig>  readPropertyConfig(const PropertyConfig::digest_type& ref);
  PropertyConfig::digest_type      writePropertyConfig(const PropertyConfig& config);

  boost::optional<ConfigTreeNode>  readConfigTreeNode(const ConfigTreeNode::digest_type& ref);
  ConfigTreeNode::digest_type      writeConfigTreeNode(const ConfigTreeNode& config);

  boost::optional<ConfigTreeNode>  readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type&);
  ConfigTreeNodeAlias::alias_type writeConfigTreeNodeAlias(const ConfigTreeNodeAlias&);

  std::vector<ConfigTreeNodeAlias> configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type&);
private:
  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream&   debug() const { return msg(MSG::DEBUG);   }
  MsgStream&    info() const { return msg(MSG::INFO);    }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream&   error() const { return msg(MSG::ERROR);   }
  MsgStream&   fatal() const { return msg(MSG::FATAL);   }
  MsgStream&  always() const { return msg(MSG::ALWAYS);  }

  boost::filesystem::path dir() const;
   
  mutable std::unique_ptr<MsgStream> m_msg;
  mutable std::string                m_dir;   ///< where to read/write configurations from/to?

  // TODO: replace fs::path with a concrete config...
  bool isCompatible(const ConfigTreeNodeAlias& alias, const boost::filesystem::path& dirName ) const;

  boost::filesystem::path propertyConfigPath( const PropertyConfig::digest_type& digest ) const;
  boost::filesystem::path configTreeNodePath( const ConfigTreeNode::digest_type& digest ) const;
  boost::filesystem::path configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const;
  bool create_directories( boost::filesystem::path dir ) const;

  MsgStream& msg(MSG::Level level) const;
};
#endif // CONFIGFILEACCESSSVC_H
