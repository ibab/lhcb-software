// $Id: IOTRandomDepositCreator.h,v 1.5 2008-05-28 20:10:34 janos Exp $
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
  typedef std::vector<LHCb::MCOTDeposit*> OTDeposits;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTRandomDepositCreator; }

  virtual void createDeposits(OTDeposits& deposits) const = 0;

};
#endif // OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 
