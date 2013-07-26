#ifndef CONFIGZIPFILEACCESSSVC_H 
#define CONFIGZIPFILEACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include "boost/optional.hpp"
// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include "Kernel/IConfigAccessSvc.h"

/** @class ConfigFileAccessSvc ConfigZipFileAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to zip files
 *
 *  @author Gerhard Raven
 *  @date   2011-04-19
 */

namespace ConfigZipFileAccessSvc_details  {
   class ZipFile;
}

class ConfigZipFileAccessSvc : public Service,
                            virtual public IConfigAccessSvc {
public:
  ConfigZipFileAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ConfigZipFileAccessSvc( );     ///< Destructor

  StatusCode queryInterface(const InterfaceID& , void** );

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
  
  ConfigZipFileAccessSvc_details::ZipFile*  file() const;

  mutable std::auto_ptr<MsgStream>     m_msg;
  std::string                          m_name;   ///< filename of zip file from which to read configurations
  std::string                          m_mode;   ///< which flags to specify when opening the zip file
  mutable std::auto_ptr<ConfigZipFileAccessSvc_details::ZipFile>               m_zipfile;

  template <typename T> boost::optional<T> read(const std::string& path) const;
  template <typename T> bool write(const std::string& path,const T& object) const;
  std::string propertyConfigPath( const PropertyConfig::digest_type& digest ) const;
  std::string configTreeNodePath( const ConfigTreeNode::digest_type& digest ) const;
  std::string configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const;

  MsgStream& msg(MSG::Level level) const;
};
#endif // CONFIGZIPFILEACCESSSVC_H
