#ifndef CONFIGZIPFILEACCESSSVC_H 
#define CONFIGZIPFILEACCESSSVC_H 1

// Include files
#include <string>
#include <memory>
#include "boost/optional.hpp"

#include "ConfigArchiveAccessSvc.h"

/** @class ConfigFileAccessSvc ConfigZipFileAccessSvc.h
 *  
 *  functionality:
 *        read/write configure information to zip files
 *
 *  @author Gerhard Raven
 *  @date   2011-04-19
 */

class IArchive;

class ConfigZipFileAccessSvc : public ConfigArchiveAccessSvc {
public:
  ConfigZipFileAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ConfigZipFileAccessSvc( );     ///< Destructor
  StatusCode finalize();      ///< Service initialization
private:  
  IArchive*  file() const;
  std::string                          m_name;   ///< filename of zip file from which to read configurations
  std::string                          m_mode;   ///< which flags to specify when opening the zip file
  mutable std::unique_ptr<IArchive>    m_file;

};
#endif // CONFIGZIPFILEACCESSSVC_H
