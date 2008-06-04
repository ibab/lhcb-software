// $Id: Bd2KstarMuMuAngleCalculator.h,v 1.2 2008-06-04 16:18:14 pkoppenb Exp $
#ifndef BD2KSTARMUMUANGLECALCULATOR_H 
#define BD2KSTARMUMUANGLECALCULATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVPartAngleCalculator.h"            // Interface


/** @class Bd2KstarMuMuAngleCalculator Bd2KstarMuMuAngleCalculator.h
 *  
 *  Calculates the three angles in a B->llKPi decay, eg B->MuMuK*
 *  Angles given in both the helicity and transversity bases.
 *
 *  @author Thomas Blake
 *  @date   2007-08-02
 */

class Bd2KstarMuMuAngleCalculator : public GaudiTool, virtual public IP2VVPartAngleCalculator {
public: 
  /// Standard constructor
  Bd2KstarMuMuAngleCalculator( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~Bd2KstarMuMuAngleCalculator( ); ///< Destructor
  
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

   StatusCode initialize() ;
  

protected:

private:
  
  
  IParticleDescendants* m_descendants ;
  IP2VVAngleCalculator* m_angle;
  
  
  
};
#endif // BD2KSTARMUMUANGLECALCULATOR_H
