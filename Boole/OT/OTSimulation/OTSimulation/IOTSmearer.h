// $Id: IOTSmearer.h,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $
#ifndef OTSIMULATION_IOTSMEARER_H 
#define OTSIMULATION_IOTSMEARER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class MCOTDeposit;

static const InterfaceID IID_OTSmearer( "IOTSmearer", 1, 0 );

/** @class IOTSmearer IOTSmearer.h "OTSimulation/IOTSmearer.h"
 *  
 *  Outer tracker smearer interface.
 *
 *  @author Marco Cattaneo
 *  @date   08/01/2002
 */
class IOTSmearer : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTSmearer; }
  /// Actual operator function
  virtual StatusCode smear( MCOTDeposit* ) = 0;
  /// Get the resolution (without magnetic field correction)
  virtual double resolution() = 0;
  /// Get the resolution (with magnetic field correction)
  virtual double resolution( HepPoint3D& ) = 0;


};
#endif // OTSIMULATION_IOTSMEARER_H
