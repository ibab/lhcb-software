// $Id: IP2VVMCPartAngleCalculator.h,v 1.1 2007-09-18 13:39:06 pkoppenb Exp $
#ifndef IMCANGLECALCULATOR_H 
#define IMCANGLECALCULATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/MCParticle.h"

static const InterfaceID IID_IP2VVMCPartAngleCalculator ( "IP2VVMCPartAngleCalculator", 1, 0 );

/** @class IP2VVMCPartAngleCalculator IP2VVMCPartAngleCalculator.h v1r3/IP2VVMCPartAngleCalculator.h
 *  
 *  Calculates the three angles in a P->VV decay. The methods are called
 *  ThetaL, ThetaK and Phi to comply with the B->MuMuK* conventions,
 *  but tools for other decays could be implemented.
 *
 *  @author Thomas Blake
 *  @date   2007-08-13
 */
class IP2VVMCPartAngleCalculator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IP2VVMCPartAngleCalculator; }

  /// get theta_L (angle of the L+ wrt B in dilepton frame)
  virtual double calculateThetaL( const LHCb::MCParticle* mother ) = 0;
  /// get theta_K (angle of the K+ wrt B in dilepton frame)
  virtual double calculateThetaK( const LHCb::MCParticle* mother ) = 0;
  /// get phi (angle of the K* and LL decay planes in dilepton frame)
  virtual double calculatePhi( const LHCb::MCParticle* mother ) = 0;
  /// get all three
  virtual StatusCode calculateAngles( const LHCb::MCParticle* mother, double& thetal, double& thetak, double& phi ) = 0;
  /// get intermediate state mass (eg dilepton mass)
  virtual double calculateMass( const LHCb::MCParticle* mother ) = 0; 

protected:

private:

};
#endif // IMCANGLECALCULATOR_H
