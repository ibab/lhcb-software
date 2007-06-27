// $Id: IOTSmearer.h,v 1.4 2007-06-27 15:22:24 janos Exp $
#ifndef OTSIMULATION_IOTSMEARER_H 
#define OTSIMULATION_IOTSMEARER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"


// MCEvent
#include "Event/MCOTDeposit.h"

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
  virtual void smearDistance( LHCb::MCOTDeposit* aDeposit ) const = 0;

  /// Get the resolution (without magnetic field correction)
  virtual double resolution() const = 0;

  /// Get the resolution (with magnetic field correction)
  virtual double resolution( const Gaudi::XYZPoint& aPoint) const = 0;

  /// Smear the time 
  virtual void smearTime(LHCb::MCOTDeposit* aDeposit) const = 0;

};

#endif // OTSIMULATION_IOTSMEARER_H
