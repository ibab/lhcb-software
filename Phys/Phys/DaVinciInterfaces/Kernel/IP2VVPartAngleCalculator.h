// $Id: IP2VVPartAngleCalculator.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef IP2VVPARTANGLECALCULATOR_H
#define IP2VVPARTANGLECALCULATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb
{
  class Particle;
}


/** @class IP2VVPartAngleCalculator IP2VVPartAngleCalculator.h
 *
 *  Calculates the three angles in a P->VV decay in both the helicity
 *  and the transversity bases. The methods are called
 *  ThetaL, ThetaK and Phi to comply with the B->MuMuK* conventions and
 *  Theta_tr, Phi_tr and Theta_V (=phi,K*) to comply with the Bs->J/psi Phi
 *  conventions for the transversity basis.
 *
 *  @author Thomas Blake, Greig Cowan
 *  @date   2007-08-02
 *  @modified 2008-06-02
 */
class GAUDI_API IP2VVPartAngleCalculator : virtual public IAlgTool
{
  
 public:
  
  DeclareInterfaceID(IP2VVPartAngleCalculator, 2, 0);
  
 public:

  /// get theta_L (angle of the L+ wrt B in dilepton frame)
  virtual double calculateThetaL( const LHCb::Particle* mother) = 0 ;

  /// get theta_K (angle of the K+ wrt B in dilepton frame)
  virtual double calculateThetaK( const LHCb::Particle* mother ) = 0 ;

  /// get phi (angle of the K* and LL decay planes in dilepton frame)
  virtual double calculatePhi( const LHCb::Particle* mother ) = 0 ;

  /// get all three helicity angles
  virtual StatusCode calculateAngles( const LHCb::Particle* mother,
                                      double& thetal,
                                      double& thetak,
                                      double& phi ) = 0 ;

  /// get Theta_tr (angle of the L+ wrt z-axis in dilepton frame)
  virtual double calculateTransThetaTr( const LHCb::Particle* mother) = 0 ;

  /// get Phi_tr (azimutal angle of the L+ in dilepton frame)
  virtual double calculateTransPhiTr( const LHCb::Particle* mother ) = 0 ;

  /// get Theta_phi_tr (polar angle between the x'-axis and the decaying K+ in the dikaon frame)
  virtual double calculateTransThetaV( const LHCb::Particle* mother ) = 0 ;

  /// get all three transversity angles
  virtual StatusCode calculateTransversityAngles( const LHCb::Particle* mother,
                                                  double& Theta_tr,
                                                  double& Phi_tr,
                                                  double& Theta_V ) = 0 ;

};
#endif // IP2VVPARTANGLECALCULATOR_H
