// $Id: IOTEffCalculator.h,v 1.2 2007-04-08 16:54:51 janos Exp $
#ifndef OTSIMULATION_IOTEFFCALCULATOR_H 
#define OTSIMULATION_IOTEFFCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class MCOTDeposit;
}

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
  virtual void calculate( LHCb::MCOTDeposit* aDeposit, bool& accept) = 0;
};

#endif // OTSIMULATION_IOTEFFCALCULATOR_H

