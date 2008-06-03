// $Id: IP2VVMCPartAngleCalculator.h,v 1.3 2008-06-03 13:44:01 pkoppenb Exp $
#ifndef IMCANGLECALCULATOR_H 
#define IMCANGLECALCULATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class MCParticle;
}


static const InterfaceID IID_IP2VVMCPartAngleCalculator ( "IP2VVMCPartAngleCalculator", 2, 0 );

/** @class IP2VVMCPartAngleCalculator IP2VVMCPartAngleCalculator.h Kernel/IP2VVMCPartAngleCalculator.h
 *  
 *  Calculates the three angles in a P->VV decay. The methods are called
 *  ThetaL, ThetaK and Phi to comply with the B->MuMuK* conventions and
 *  Theta_tr, Phi_tr and Theta_phi_tr to comply with the Bs->J/psi Phi 
 *  conventions for the transversity basis. 
 *
 *  @author Thomas Blake, Greig Cowan
 *  @date   2007-08-13
 *  @modified 2008-06-02
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
  virtual StatusCode calculateAngles( const LHCb::MCParticle* mother, 
                                      double& thetal, 
                                      double& thetak, 
                                      double& phi ) = 0;
  
  /// get intermediate state mass (eg dilepton mass)
  virtual double calculateMass( const LHCb::MCParticle* mother ) = 0; 

  /// get Theta_tr (angle of the L+ wrt z-axis in dilepton frame)
  virtual double calculateTransThetaTr( const LHCb::MCParticle* mother) = 0 ;
 
  /// get Phi_tr (azimutal angle of the L+ in dilepton frame)
  virtual double calculateTransPhiTr( const LHCb::MCParticle* mother ) = 0 ;
  
  /// get Theta_phi_tr (polar angle between the x'-axis and the decaying K+ in the dikaon frame)
  virtual double calculateTransThetaV( const LHCb::MCParticle* mother ) = 0 ;


  /// get all three transversity angles
  virtual StatusCode calculateTransversityAngles( const LHCb::MCParticle* mother,
                                                  double& Theta_tr, 
                                                  double& Phi_tr, 
                                                  double& Theta_V ) = 0 ;
  

protected:

private:

};
#endif // IMCANGLECALCULATOR_H
