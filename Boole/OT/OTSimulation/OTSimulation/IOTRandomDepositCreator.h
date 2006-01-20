// $Id: IOTRandomDepositCreator.h,v 1.5 2006-01-20 12:57:05 janos Exp $
#ifndef OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 
#define OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb
{
  class MCOTDeposit;
}

typedef std::vector<LHCb::MCOTDeposit*> MCOTDepositVec;

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
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTRandomDepositCreator; }

  virtual StatusCode createDeposits(MCOTDepositVec* depVector) const = 0; 

};
#endif // OTSIMULATION_IOTRANDOMDEPOSITCREATOR_H 
