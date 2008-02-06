// $Id: ConfigDBAccessSvc.h,v 1.1 2008-02-06 14:57:05 graven Exp $
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

  virtual StatusCode queryInterface(const InterfaceID& , void** );

  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();      ///< Service initialization

  boost::optional<PropertyConfig>  read(const PropertyConfig::digest_type& ref);
  PropertyConfig::digest_type     write(const PropertyConfig& config);

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
  StatusCode createTopTable();

  boost::optional<PropertyConfig> get(const PropertyConfig::digest_type& ref);
  size_t prefetch(const PropertyConfig::digest_type& ref);

  typedef std::map<PropertyConfig::digest_type,PropertyConfig>  cache_t;

  mutable std::auto_ptr<MsgStream>     m_msg;
  std::string                          m_connection;
  coral::ISessionProxy*                m_session;
  cache_t                              m_cache;
  bool                                 m_createSchema;
  bool                                 m_prefetch;

};
#endif // CONFIGDBACCESSSVC_H
