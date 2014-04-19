// $Id: ConfigTarFileAccessSvc.h,v 1.1 2010-05-05 13:20:44 graven Exp $
#ifndef CONFIGTARFILEACCESSSVC_H 
#define CONFIGTARFILEACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include "boost/optional.hpp"

#include "ConfigArchiveAccessSvc.h"

/** @class ConfigFileAccessSvc ConfigFileAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to files
 *
 *  @author Gerhard Raven
 *  @date   2009-06-04
 */


class IArchive;

class ConfigTarFileAccessSvc : public ConfigArchiveAccessSvc {
public:
  ConfigTarFileAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ConfigTarFileAccessSvc( );     ///< Destructor
  StatusCode finalize();      ///< Service initialization
private:
  IArchive*  file() const override;
  std::string                          m_name;   ///< filename of tar file from which to read configurations
  std::string                          m_mode;   ///< which flags to specify when opening the tar file
  mutable std::unique_ptr<IArchive>    m_file;

};
#endif // CONFIGTARFILEACCESSSVC_H
