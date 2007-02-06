// $Id: IOnOffline.h,v 1.5 2007-02-06 09:57:56 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IONOFFLINE_H 
#define DAVINCIKERNEL_IONOFFLINE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IOnOffline ( "IOnOffline", 2, 0 );

/** @class IOnOffline IOnOffline.h Kernel/IOnOffline.h
 *  
 *  Tool that tells the application if one runs in On or Offline Mode
 *  Now uses context. Can only be used as private tool.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class IRelatedPVFinder ;
class IOnOffline : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IOnOffline; }

  /// On or Offline ?
  virtual bool online(void) const = 0 ;

  /// Return DispCalculator
  virtual std::string dispCalculator() const = 0 ;

  /// Return Unconstrained Vertex Fitter
  virtual std::string vertexFitter() const = 0 ;

  /// Get PV location in TES
  virtual std::string getPVLocation(void) const = 0 ;

  /// PV relator
  virtual IRelatedPVFinder* pvRelator(void) const = 0 ;

};
#endif // DAVINCIKERNEL_IONOFFLINE_H
