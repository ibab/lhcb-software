// $Id: ConfigDBAccessSvc.h,v 1.1 2010-05-05 13:20:44 graven Exp $
#ifndef CONFIGDBACCESSSVC_H 
#define CONFIGDBACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include <map>
// fix against clash on windows with MsgStream...
#ifdef _WIN32
#define NOMSG
#define NOGDI
#endif
#include "boost/optional.hpp"
#include "Kernel/IConfigAccessSvc.h"
#include "DetCond/ICOOLConfSvc.h"
// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

namespace coral {
   class ISessionProxy;
}

/** @class ConfigDBAccessSvc ConfigDBAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to CORAL database
 *
 *  @author Gerhard Raven
 *  @date   2007-12-20
 */
class ConfigDBAccessSvc : public extends1<Service,IConfigAccessSvc> {
public:
  ConfigDBAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~ConfigDBAccessSvc( ) override = default;     ///< Destructor

  StatusCode initialize();    ///< Service initialization
  StatusCode finalize();      ///< Service initialization

  boost::optional<PropertyConfig>  readPropertyConfig(const PropertyConfig::digest_type& ref);
  PropertyConfig::digest_type     writePropertyConfig(const PropertyConfig& config);

  boost::optional<ConfigTreeNode>  readConfigTreeNode(const ConfigTreeNode::digest_type& ref);
  ConfigTreeNode::digest_type     writeConfigTreeNode(const ConfigTreeNode& config);

  boost::optional<ConfigTreeNode>  readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type&);
  ConfigTreeNodeAlias::alias_type writeConfigTreeNodeAlias(const ConfigTreeNodeAlias&);

  std::vector<ConfigTreeNodeAlias> configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& alias);

private:
  MsgStream& msg(MSG::Level level) const;
  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }

  template <typename T> struct table_traits;

  template <typename T> boost::optional<T>                 read(const typename table_traits<T>::key_type&);
  template <typename T> typename table_traits<T>::key_type write(const T& );
  template <typename T> void                               createTable();
  
  StatusCode openConnection();
  StatusCode createSchema();

  void generateCacheTableEntries(const ConfigTreeNode& target);
  bool hasCacheEntries( const std::string&, const std::string&);
  template <typename iter> void writeCacheEntries( const std::string&, const std::string&, iter, iter);
  void createCacheTables();

  mutable std::unique_ptr<MsgStream>   m_msg;
  std::string                          m_connection;
  coral::ISessionProxy*                m_session = nullptr;
  ICOOLConfSvc*                        m_coolConfSvc = nullptr;
  bool                                 m_readOnly;
  bool                                 m_createSchema;

};
#endif // CONFIGDBACCESSSVC_H
