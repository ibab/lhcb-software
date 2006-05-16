// $Id: NoPIDsParticleMaker.h,v 1.7 2006-05-16 17:59:02 pkoppenb Exp $
#ifndef NOPIDSPARTICLEMAKER_H 
#define NOPIDSPARTICLEMAKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleMaker.h"            // Interface

class ParticleProperty;
class IParticlePropertySvc;
class IParticle2State;
/** @class NoPIDsParticleMaker NoPIDsParticleMaker.h
 *  
 *  The simplest possible particle maker.
 *  It assigned PID hypothesis to charged protoparticle.
 *  Thus some imitation of "high-level-trigger-charged-particle"
 *  is performed
 *
 *  @author Patrick KOPPENBURG
 *  @date   2006-01-23
 */
class NoPIDsParticleMaker : public GaudiTool, virtual public IParticleMaker {
public: 
  /// Standard constructor
  NoPIDsParticleMaker( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~NoPIDsParticleMaker( ); ///< Destructor

  StatusCode initialize() ; ///< Initialize

  StatusCode finalize() ; ///< Initialize

  
  /// Dispatch the making of particles 
  StatusCode makeParticles( LHCb::Particle::ConstVector & parts );

protected:

  
  /** Fill the particle from protoparticle using ID  
   *  @param proto    pointer to ProtoParticle
   *  @param property particle property information
   *  @param particle particle (output) 
   *  @return status code 
   */
  StatusCode fillParticle 
  ( const LHCb::ProtoParticle*    proto    , 
    const ParticleProperty* property , 
    LHCb::Particle*               particle ) const ;
  
  /// accessor to Particle properties service 
  inline IParticlePropertySvc* ppSvc  () const { return m_ppSvc  ; }
  
  /// set particl eproperties for particle and for antiparticle  
  StatusCode setPPs( const std::string& pid ) ;
  

private:

  // particle property service 
  IParticlePropertySvc* m_ppSvc   ;  
  
  // ID of the particle 
  std::string             m_pid   ;
  // ID of the anti-particle 
  std::string             m_apid  ;
  
  // properties of particle
  const ParticleProperty* m_pp    ;
  
  // properties of anti-particle
  const ParticleProperty* m_app   ;
  
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

  /// Particle to state convertion tool
  IParticle2State* m_p2s ;
};
#endif // NOPIDSPARTICLEMAKER_H
