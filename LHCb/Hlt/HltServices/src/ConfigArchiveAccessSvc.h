// $Id: ConfigTarFileAccessSvc.h,v 1.1 2010-05-05 13:20:44 graven Exp $
#ifndef CONFIGARCHIVEACCESSSVC_H 
#define CONFIGARCHIVEACCESSSVC_H 

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


class IArchive;

class ConfigArchiveAccessSvc : public extends1<Service,IConfigAccessSvc> {
public:
  ConfigArchiveAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~ConfigArchiveAccessSvc( ) override = default;     ///< Destructor

  StatusCode initialize() override;    ///< Service initialization

  boost::optional<PropertyConfig>  readPropertyConfig(const PropertyConfig::digest_type& ref) override;
  PropertyConfig::digest_type      writePropertyConfig(const PropertyConfig& config) override;

  boost::optional<ConfigTreeNode>  readConfigTreeNode(const ConfigTreeNode::digest_type& ref) override;
  ConfigTreeNode::digest_type      writeConfigTreeNode(const ConfigTreeNode& config) override;

  boost::optional<ConfigTreeNode>  readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type&) override;
  ConfigTreeNodeAlias::alias_type writeConfigTreeNodeAlias(const ConfigTreeNodeAlias&) override;

  std::vector<ConfigTreeNodeAlias> configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type&) override;
protected:
  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }

private:
  virtual IArchive*  file() const = 0;

  mutable std::unique_ptr<MsgStream>   m_msg;

  template <typename T> boost::optional<T> read(const std::string& path) const;
  template <typename T> bool write(const std::string& path,const T& object) const;
  std::string propertyConfigPath( const PropertyConfig::digest_type& digest ) const;
  std::string configTreeNodePath( const ConfigTreeNode::digest_type& digest ) const;
  std::string configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const;

  MsgStream& msg(MSG::Level level) const;
};
#endif // CONFIGTARFILEACCESSSVC_H
