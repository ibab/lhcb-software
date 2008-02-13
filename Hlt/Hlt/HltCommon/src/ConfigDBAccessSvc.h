// $Id: ConfigDBAccessSvc.h,v 1.2 2008-02-13 14:55:22 graven Exp $
#ifndef CONFIGDBACCESSSVC_H 
#define CONFIGDBACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include <map>
#include "boost/optional.hpp"
#include "HltBase/IConfigAccessSvc.h"
// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

namespace coral {
   class ISessionProxy;
};

/** @class ConfigDBAccessSvc ConfigDBAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to CORAL database
 *
 *  @author Gerhard Raven
 *  @date   2007-12-20
 */
class ConfigDBAccessSvc : public Service,
                          virtual public IConfigAccessSvc {
public:
  ConfigDBAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ConfigDBAccessSvc( );     ///< Destructor

  StatusCode queryInterface(const InterfaceID& , void** );

  StatusCode initialize();    ///< Service initialization
  StatusCode finalize();      ///< Service initialization

  boost::optional<PropertyConfig>  readPropertyConfig(const PropertyConfig::digest_type& ref);
  PropertyConfig::digest_type     writePropertyConfig(const PropertyConfig& config);

  boost::optional<ConfigTreeNode>  readConfigTreeNode(const ConfigTreeNode::digest_type& ref);
  ConfigTreeNode::digest_type     writeConfigTreeNode(const ConfigTreeNode& config);

private:
  MsgStream& msg(MSG::Level level) const;
  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }
  

  StatusCode openConnection();
  StatusCode createSchema();

  mutable std::auto_ptr<MsgStream>     m_msg;
  std::string                          m_connection;
  coral::ISessionProxy*                m_session;
  bool                                 m_createSchema;

};
#endif // CONFIGDBACCESSSVC_H
