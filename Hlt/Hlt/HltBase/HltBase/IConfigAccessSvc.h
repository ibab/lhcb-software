// $Id: IConfigAccessSvc.h,v 1.1 2008-01-24 22:21:09 graven Exp $
#ifndef ICONFIGACCESSSVC_H 
#define ICONFIGACCESSSVC_H 1

// Include files
#include <string>
#include "GaudiKernel/INamedInterface.h"
#include "boost/optional.hpp"
#include "HltBase/PropertyConfig.h"


/** @class IConfigAccessSvc IConfigAccessSvc.h
 *  
 *  functionality:
 *         Interface for obtaining an
 *         individual configuration description
 *         given its ID
 *
 *         The ID corresponds to the MD5 digest of the contents
 *         of the configration description
 *
 *  @author Gerhard Raven
 *  @date   2007-12-12
 */


class IConfigAccessSvc : virtual public INamedInterface  {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID();
  virtual ~IConfigAccessSvc();
  typedef std::string CfgID_t ; //FIXME: replace by 128 bit quantity... (it's an MD5 sum of PropertyConfig instances)
  virtual boost::optional<PropertyConfig> read(const CfgID_t& ref) = 0;
  virtual CfgID_t                        write(const PropertyConfig& config) = 0;
};
#endif // ICONFIGACCESSSVC_H
