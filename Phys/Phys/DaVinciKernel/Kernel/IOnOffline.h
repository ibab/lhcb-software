// $Id: IOnOffline.h,v 1.12 2008-12-11 07:56:07 cattanem Exp $
#ifndef DAVINCIKERNEL_IONOFFLINE_H 
#define DAVINCIKERNEL_IONOFFLINE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IOnOffline ( "IOnOffline", 3, 0 );

/** @class IOnOffline IOnOffline.h Kernel/IOnOffline.h
 *  
 *  Tool that tells the application if one runs in On or Offline Mode
 *  Now uses context. Can only be used as private tool.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 *
 *  @author Juan PALACIOS
 *  @date   2008-09-29
 */
class IOnOffline : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IOnOffline; }

  /// On or Offline ?
  virtual bool online() const = 0 ;

  /// Return name of IDistanceCalculator implementation 
  virtual const std::string& distanceCalculatorType() const = 0 ;

  /// Return name of unconstrained vertex fitter
  virtual const std::string& vertexFitterType() const = 0 ;

  /// Return name of IRelatedPVFinder implementation
  virtual const std::string& relatedPVFinderType() const = 0;

  /// Return location of default primary vertices
  virtual const std::string& primaryVertexLocation() const = 0 ;

  /// Return root of a TES location.
  /// Typical values could be "Phys" or "Hlt"
  /// Parent code would then append to "/Event/" to create
  /// absolute TES trunk location for data under control
  /// of the parent.
  virtual const std::string& trunkOnTES() const = 0;
  
};
#endif // DAVINCIKERNEL_IONOFFLINE_H
