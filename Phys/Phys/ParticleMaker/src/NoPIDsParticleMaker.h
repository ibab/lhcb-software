// $Id: NoPIDsParticleMaker.h,v 1.12 2009-06-12 16:06:02 pkoppenb Exp $
#ifndef NOPIDSPARTICLEMAKER_H 
#define NOPIDSPARTICLEMAKER_H 1

// Include files
#include "ChargedParticleMakerBase.h"            // Interface

/** @class NoPIDsParticleMaker NoPIDsParticleMaker.h
 *  
 *  The simplest possible particle maker.
 *  It assigns a PID hypothesis to charged protoparticle.
 *  Thus some imitation of "high-level-trigger-charged-particle"
 *  is performed
 *
 *  @author Patrick KOPPENBURG
 *  @date   2006-01-23
 */
class NoPIDsParticleMaker : public ChargedParticleMakerBase {
public: 
  /// Standard constructor
  NoPIDsParticleMaker( const std::string& name,ISvcLocator* pSvcLocator);
  
  virtual ~NoPIDsParticleMaker( ); ///< Destructor
  
  StatusCode initialize() ; ///< Initialize
  
  StatusCode finalize() ; ///< Initialize

  /// Dispatch the making of particles 
  StatusCode makeParticles( LHCb::Particle::Vector & parts );
  
protected:
  
  
  /** Fill the particle from protoparticle using ID  
   *  @param proto    pointer to ProtoParticle
   *  @param property particle property information
   *  @param particle particle (output) 
   *  @return status code 
   */
  StatusCode fillParticle 
  ( const LHCb::ProtoParticle*    proto    , 
    const LHCb::ParticleProperty* property , 
    LHCb::Particle*               particle ) const ;

private:

  // CL to be used 
  double                  m_CL     ;
  
  // number of calls 
  size_t                  m_calls  ;
  
  // total created particles 
  double                  m_sum    ;
  
  // total created particles (sum2)  
  double                  m_sum2   ;
  
};
// ============================================================================
// The END 
// ============================================================================
#endif // NOPIDSPARTICLEMAKER_H
// ============================================================================
