// $Id: IP2VVAngleCalculator.h,v 1.1 2007-09-18 13:35:58 pkoppenb Exp $
#ifndef IB2LLXANGLETOOL_H 
#define IB2LLXANGLETOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IP2VVAngleCalculator ( "IP2VVAngleCalculator", 1, 0 );

/** @class IP2VVAngleCalculator IP2VVAngleCalculator.h
 *  
 *  Low-level tool that calculates the three angles between the four
 *  decay products of a P->VV decay. Use IP2VVPartAngleCalculator
 *  or IP2VVMCPartAngleCalculator instead.
 *
 *  @author Thomas Blake
 *  @date   2007-08-22
 */
class IP2VVAngleCalculator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IP2VVAngleCalculator; }
  

  /// calculate the angle between two planes in the rest frame of the mother particle
  virtual double calculatePlaneAngle( const Gaudi::LorentzVector&, const Gaudi::LorentzVector&,
                                      const Gaudi::LorentzVector&, const Gaudi::LorentzVector&,
                                      const Gaudi::LorentzVector& ) = 0;

  /// calculate the polar angle in the rest frame of the mother particle
  virtual double calculatePolarAngle( const Gaudi::LorentzVector&, const Gaudi::LorentzVector&, 
                                      const Gaudi::LorentzVector& ) = 0;

protected:

private:

};
#endif // IB2LLXANGLETOOL_H
