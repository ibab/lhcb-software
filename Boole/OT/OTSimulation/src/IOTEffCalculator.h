// $Id: IOTEffCalculator.h,v 1.5 2010-02-28 11:22:51 nserra Exp $
#ifndef OTSIMULATION_IOTEFFCALCULATOR_H 
#define OTSIMULATION_IOTEFFCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// MCEvent
#include "Event/MCOTDeposit.h"

static const InterfaceID IID_IOTEffCalculator( "IOTEffCalculator", 1, 0 );

/** @class IOTEffCalculator IOTEffCalculator.h "OTSimulation/IOTEffCalculator.h"
 *  
 *  Outer tracker smearer interface.
 *
 *  @author Marco Cattaneo
 *  @date   08/01/2002
 */

class IOTEffCalculator : virtual public IAlgTool {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IOTEffCalculator; }
  
  /// Actual operator function
  virtual void calculate( LHCb::MCOTDeposit* aDeposit, const double slope, bool& accept) const = 0;

};

#endif // OTSIMULATION_IOTEFFCALCULATOR_H

