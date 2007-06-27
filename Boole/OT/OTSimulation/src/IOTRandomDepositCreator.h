// $Id: IOTRandomDepositCreator.h,v 1.4 2007-06-27 15:27:12 janos Exp $
#ifndef OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 
#define OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// MCEvent
#include "Event/MCOTDeposit.h"

static const InterfaceID IID_OTRandomDepositCreator( "IOTRandomDepositCreator", 1, 0 );

/** @class IOTRandomDepositCreator IOTRandomDepositCreator.h "OTSimulation/IOTRandomDepositCreator.h"
 *  
 *  Outer tracker random deposit generator.
 *
 *  @author M Needham
 *  @date   28/02/2003
 */

class IOTRandomDepositCreator : virtual public IAlgTool {
public:

  /// Handy typdedef
  typedef std::vector<LHCb::MCOTDeposit*> MCOTDepositVec;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTRandomDepositCreator; }

  virtual void createDeposits(MCOTDepositVec* depVector) const = 0; 

};
#endif // OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 
