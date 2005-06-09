// $Id: IOnOffline.h,v 1.2 2005-06-09 13:10:40 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IONOFFLINE_H 
#define DAVINCIKERNEL_IONOFFLINE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IOnOffline ( "IOnOffline", 1, 0 );

/** @class IOnOfflineTool IOnOfflineTool.h Kernel/IOnOffline.h
 *  
 *  Tool that tells the application if one runs in On or Offline Mode
 *  Replaces the IPVLocator tool
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class IOnOffline : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IOnOffline; }

  /// On or Offline ?
  virtual bool online(void) const = 0 ;

  /// On or Offline ?
  virtual StatusCode setOnline(const bool&) = 0 ;

  /// Return DispCalculator
  virtual std::string dispCalculator() const = 0 ;

  /// Return Unconstrained Vertex Fitter
  virtual std::string vertexFitter() const = 0 ;

  /// Get PV location in TES
  virtual std::string getPVLocation(void) const = 0 ;

};
#endif // DAVINCIKERNEL_IONOFFLINE_H
