// $Id$
#ifndef BD2KSTARMUMUANGLECALCULATOR_H 
#define BD2KSTARMUMUANGLECALCULATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVPartAngleCalculator.h"            // Interface


/** @class Bd2KstarMuMuAngleCalculator Bd2KstarMuMuAngleCalculator.h
 *  
 *  Calculates the three angles in a \f$ B_{d} \rightarrow K^{+} \pi^{-} \mu^{+} \mu^{-} \f$ decay.
 *  Angles are given in both the helicity and transversity bases.
 *
 *
 *  The Helicity Basis for \f$ B_{d} \rightarrow K^{*}(892)0 \mu^{+} \mu^{-} \f$
 *  is defined by three angles \f$ \theta_{L} \f$, \f$ \theta_{K} \f$ and \f$ \phi \f$.
 * 
 *  These angles are defined as:
 * 
 *  \f$ \theta_{L} \f$ as the angle between the \f$ {\mu^{+}}(\mu^{-}) \f$ and the direction 
 *  opposite the \f$ {B_{d}}(\bar{B_{d}}) \f$ in the rest frame of the \f$ {\mu^{+}\mu^{-}} \f$. 
 *  Equivalently this is the angle between the \f$ {\mu^{+}} \f$ in the \f$ {\mu^{+}\mu^{-}} \f$ rest 
 *  frame and the direction of the \f$ {\mu^{+}\mu^{-}} \f$ in the B rest-frame.
 *
 *  \f$ \theta_{K} \f$ as the angle between the \f$ K \f$ in the \f$ {K^{*}} \f$ frame and 
 *  the \f$ K^{*} \f$ in the B rest-frame.
 *  
 *  \f$ \phi \f$ is defined in the B rest-frame as the angle between the planes defined by the 
 *  \f$ {\mu_{+}}(\mu_{-}) \f$ and the \f$ K \pi \f$.
 *
 *
 *  @author Thomas Blake
 *  @date   2010-05-04
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
  
  /// Get daughters from B0 
  StatusCode daughters( const LHCb::Particle* mother );  
  
  
private:  

  IParticleDescendants* m_descendants ;
  
  const LHCb::Particle* m_pMuMinus;
  const LHCb::Particle* m_pMuPlus;
  const LHCb::Particle* m_pPi;
  const LHCb::Particle* m_pK;
  
};
#endif // BD2KSTARMUMUANGLECALCULATOR_H
