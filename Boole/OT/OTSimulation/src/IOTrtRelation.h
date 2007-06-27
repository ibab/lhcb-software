// $Id: IOTrtRelation.h,v 1.3 2007-06-27 15:44:53 janos Exp $
#ifndef OTSIMULATION_IOTRTRELATION_H 
#define OTSIMULATION_IOTRTRELATION_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// MCEvent
#include "Event/MCOTDeposit.h"

static const InterfaceID IID_IOTrtRelation( "IOTrtRelation", 1, 0 );

/** @class IOTrtRelation IOTrtRelation.h "OTSimulation/IOTrtRelation.h"
 *  
 *  Outer tracker r-t relation interface.
 *
 *  @author Marco Cattaneo
 *  @date   08/01/2002
 */

class IOTrtRelation : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IOTrtRelation; }

  /// tool 'operation'. Fills the deposit time entry.
  virtual void convertRtoT(LHCb::MCOTDeposit* aDeposit) const = 0;
  
  /// r-t relation with correction for the magnetic field
  virtual double driftTime(const double driftDist, const Gaudi::XYZPoint& aPoint) const = 0;
  
  /// inverse r-t relation with correction for the magnetic field
  virtual double driftDistance( const double driftTime, 
                                const Gaudi::XYZPoint& aPoint ) const = 0;

};
#endif // OTSIMULATION_IOTRTRELATION_H
