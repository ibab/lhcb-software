// $Id: IPVLocator.h,v 1.1 2004-10-27 14:09:58 pkoppenb Exp $
#ifndef DAVINCITOOLS_IPVLOCATOR_H 
#define DAVINCITOOLS_IPVLOCATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPVLocator ( "IPVLocator", 1, 0 );

/** @class IPVLocator IPVLocator.h DaVinciTools/IPVLocator.h
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
#endif // DAVINCITOOLS_IPVLOCATOR_H
