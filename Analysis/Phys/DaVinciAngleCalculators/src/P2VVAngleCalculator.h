// $Id$
#ifndef B2LLXANGLETOOL_H 
#define B2LLXANGLETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVAngleCalculator.h"            // Interface

/** @class P2VVAngleCalculator P2VVAngleCalculator.h
 *  
 *  Implementation of low-level tool that calculates the three angles between the four
 *  decay products of a P->VV decay. Use IP2VVPartAngleCalculator
 *  or IP2VVMCPartAngleCalculator instead.
 *
 *
 *  @author Thomas Blake, Greig Cowan
 *  @date   2007-08-22
 */

class P2VVAngleCalculator : public GaudiTool, virtual public IP2VVAngleCalculator {
public: 
  /// Standard constructor
  P2VVAngleCalculator( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~P2VVAngleCalculator( ); ///< Destructor

  /// calculate the angle between two planes in the rest frame of the mother particle
  double calculatePlaneAngle( const Gaudi::LorentzVector&, const Gaudi::LorentzVector&,
                              const Gaudi::LorentzVector&, const Gaudi::LorentzVector&,
                              const Gaudi::LorentzVector& );
  /// calculate the polar angle in the rest frame of the mother particle
  double calculatePolarAngle( const Gaudi::LorentzVector&, const Gaudi::LorentzVector&, 
                              const Gaudi::LorentzVector& );


   
   double calculateThetaTr( const Gaudi::LorentzVector&,
                            const Gaudi::LorentzVector&, 
                            const Gaudi::LorentzVector&,
                            const Gaudi::LorentzVector& );
  
   double calculatePhiTr( const Gaudi::LorentzVector&, 
                          const Gaudi::LorentzVector&, 
                          const Gaudi::LorentzVector&, 
                          const Gaudi::LorentzVector& );


protected:

private:

};
#endif // B2LLXANGLETOOL_H
