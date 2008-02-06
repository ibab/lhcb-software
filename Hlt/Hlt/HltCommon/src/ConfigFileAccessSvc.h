// $Id: ConfigFileAccessSvc.h,v 1.1 2008-02-06 14:57:05 graven Exp $
#ifndef CONFIGFILEACCESSSVC_H 
#define CONFIGFILEACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include "boost/optional.hpp"
// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include "HltBase/IConfigAccessSvc.h"

/** @class ConfigFileAccessSvc ConfigFileAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to files
 *
 *  @author Gerhard Raven
 *  @date   2007-12-14
 */
class ConfigFileAccessSvc : public Service,
                            virtual public IConfigAccessSvc {
public:
  ConfigFileAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ConfigFileAccessSvc( );     ///< Destructor

  virtual StatusCode queryInterface(const InterfaceID& , void** );

  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();      ///< Service initialization

  boost::optional<PropertyConfig>  read(const PropertyConfig::digest_type& ref);
  PropertyConfig::digest_type      write(const PropertyConfig& config);

private:
  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }
  
  mutable std::auto_ptr<MsgStream>     m_msg;
  std::string                          m_dir;   ///< where to read/write configurations from/to?

  MsgStream& msg(MSG::Level level) const;
};
#endif // CONFIGFILEACCESSSVC_H
