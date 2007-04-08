// $Id: IOTSmearer.h,v 1.2 2007-04-08 16:54:51 janos Exp $
#ifndef OTSIMULATION_IOTSMEARER_H 
#define OTSIMULATION_IOTSMEARER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class MCOTDeposit;
}

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
  virtual void smear( LHCb::MCOTDeposit* ) = 0;
  /// Get the resolution (without magnetic field correction)
  virtual double resolution() = 0;
  /// Get the resolution (with magnetic field correction)
  virtual double resolution( Gaudi::XYZPoint& ) = 0;
};

#endif // OTSIMULATION_IOTSMEARER_H
