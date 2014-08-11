// $Id$
#ifndef BS2JPSIPHIANGLECALCULATOR_H
#define BS2JPSIPHIANGLECALCULATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVPartAngleCalculator.h"            // Interface

/** @class Bs2JpsiPhiAngleCalculator Bs2JpsiPhiAngleCalculator.h
 *
 *  Calculates the three angles in a B->J/psi(ll)phi(KK) decay.
 *  Angles given in both helicity and transversity bases.
 *
 *  @author Greig Cowan
 *  @date   2008-05-12
 */

class Bs2JpsiPhiAngleCalculator : public GaudiTool, 
                                  virtual public IP2VVPartAngleCalculator
{

public:

  /// Standard constructor
  Bs2JpsiPhiAngleCalculator( const std::string& type,
                             const std::string& name,
                             const IInterface* parent);

  virtual ~Bs2JpsiPhiAngleCalculator( ); ///< Destructor

  StatusCode initialize() ;

public:

  double calculateThetaL( const LHCb::Particle* particle );
  double calculateThetaK( const LHCb::Particle* particle );
  double calculatePhi( const LHCb::Particle* particle );
  double calculateTransThetaTr( const LHCb::Particle* particle );
  double calculateTransPhiTr( const LHCb::Particle* particle );
  double calculateTransThetaV( const LHCb::Particle* particle );

  StatusCode calculateAngles( const LHCb::Particle* particle,
                              double& thetaL,
                              double& thetaK,
                              double& phi);


  StatusCode calculateTransversityAngles( const LHCb::Particle* particle,
                                          double& Theta_tr,
                                          double& Phi_tr,
                                          double& Theta_V );

private:

  IParticleDescendants* m_descendants ;

};

#endif // BS2JPSIPHIANGLECALCULATOR_H
