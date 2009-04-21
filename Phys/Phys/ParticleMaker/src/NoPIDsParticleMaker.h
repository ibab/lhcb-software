// $Id: NoPIDsParticleMaker.h,v 1.10 2009-04-21 19:15:41 pkoppenb Exp $
#ifndef NOPIDSPARTICLEMAKER_H 
#define NOPIDSPARTICLEMAKER_H 1

// Include files
#include "ParticleMakerBase.h"            // Interface

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
class NoPIDsParticleMaker : public ParticleMakerBase {
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
  
  /// set particl eproperties for particle and for antiparticle  
  StatusCode setPPs( const std::string& pid ) ;
  
  
private:
  
  // ID of the particle 
  std::string             m_pid   ;
  // ID of the anti-particle 
  std::string             m_apid  ;
  
  // properties of particle
  const LHCb::ParticleProperty* m_pp    ;
  
  // properties of anti-particle
  const LHCb::ParticleProperty* m_app   ;
  
  typedef std::vector<std::string> Addresses ;
  Addresses               m_inputs ;
  
  // CL to be used 
  double                  m_CL     ;
  
  // statistics: 
  
  // number of calls 
  size_t                  m_calls  ;
  
  // total created particles 
  double                  m_sum    ;
  
  // total created particles (sum2)  
  double                  m_sum2   ;
  
  // Job options to keep long tracks
  bool m_longTracks;
  
  // Job options to keep upstream tracks
  bool m_downstreamTracks;
  
  // Job options to keep VTT tracks
  bool m_vttTracks;
  
  // Job options to keep velo tracks
  bool m_veloTracks;
  
};
// ============================================================================
// The END 
// ============================================================================
#endif // NOPIDSPARTICLEMAKER_H
// ============================================================================
