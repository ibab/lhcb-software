// $Id: ConfigTarFileAccessSvc.h,v 1.1 2010-05-05 13:20:44 graven Exp $
#ifndef CONFIGCDBACCESSSVC_H 
#define CONFIGCDBACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include "boost/optional.hpp"
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
 *  @date   2009-06-04
 */

namespace ConfigCDBAccessSvc_details  {
   class CDB;
}

class ConfigCDBAccessSvc : public extends1<Service,IConfigAccessSvc> {
public:
  ConfigCDBAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~ConfigCDBAccessSvc( ) override = default;     ///< Destructor

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
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }
  
  ConfigCDBAccessSvc_details::CDB*  file() const;

  mutable std::unique_ptr<MsgStream> m_msg;
  mutable std::string                m_name;   ///< filename of tar file from which to read configurations
  std::string                        m_mode;   ///< which flags to specify when opening the tar file
  mutable std::unique_ptr<ConfigCDBAccessSvc_details::CDB> m_file;
  bool                                 m_compress; ///< do we want to transparently compress items on write?

  template <typename T> boost::optional<T> read(const std::string& path) const;
  template <typename T> bool write(const std::string& path,const T& object) const;
  std::string propertyConfigPath( const PropertyConfig::digest_type& digest ) const;
  std::string configTreeNodePath( const ConfigTreeNode::digest_type& digest ) const;
  std::string configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const;

  MsgStream& msg(MSG::Level level) const;
};
#endif // CONFIGTARFILEACCESSSVC_H
