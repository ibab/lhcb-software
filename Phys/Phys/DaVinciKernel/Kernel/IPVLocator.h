// $Id: IPVLocator.h,v 1.2 2005-01-06 10:30:43 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IPVLOCATOR_H 
#define DAVINCIKERNEL_IPVLOCATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPVLocator ( "IPVLocator", 1, 0 );

/** @class IPVLocator IPVLocator.h Kernel/IPVLocator.h
 *  
 *  Simple Tool that locates the position of the primary vertices in the TES
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-10-27
 */
class IPVLocator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPVLocator; }

  /// Get PV location in TES
  virtual std::string getPVLocation(void) const = 0 ;

  /// Set PV location in TES
  virtual StatusCode setPVLocation(std::string&) = 0 ;

};
#endif // DAVINCIKERNEL_IPVLOCATOR_H
