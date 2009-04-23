// $Id: ParticleMakerBase.h,v 1.2 2009-04-23 10:39:31 pkoppenb Exp $
#ifndef PARTICLEMAKERBASE_H 
#define PARTICLEMAKERBASE_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "Kernel/DVAlgorithm.h"
// PartProp
#include "Kernel/ParticleProperty.h" 

/** @class ParticleMakerBase ParticleMakerBase.h
 *
 *  Base class for all Particle Maker Algorithms
 *
 *  @author P. Koppenburg
 *  @date   2009-04-21
 */
namespace
{
  std::string to_upper( const std::string& in )
  {
    std::string  out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  };
};

class ParticleMakerBase : public DVAlgorithm {
public:
  /// Standard constructor
  ParticleMakerBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleMakerBase( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

  virtual StatusCode initialize() ; ///< Finalize
  virtual StatusCode finalize(){return DVAlgorithm::finalize();} ; ///< Finalize

protected:
  /// The method that each implementation should implement
  virtual StatusCode makeParticles(LHCb::Particle::Vector& ) = 0 ;
  /// set particl eproperties for particle and for antiparticle  
  StatusCode setPPs( const std::string& pid ) ;

  /// protoparticles
  const LHCb::ProtoParticles* protos(){
    const LHCb::ProtoParticles* pp = 0 ;
    if ( exist<LHCb::ProtoParticles>( m_input )){
      pp = get< LHCb::ProtoParticles > (m_input);
    } else {
      Warning("No ProtoParticles at "+m_input,StatusCode::FAILURE,1);
    }
    return pp ;
  }

protected:

  /// ID of the particle 
  std::string             m_pid   ;
  /// ID of the anti-particle 
  std::string             m_apid  ;
  /// properties of particle
  const LHCb::ParticleProperty* m_pp    ;
  
  /// properties of anti-particle
  const LHCb::ParticleProperty* m_app   ;

  /// Input Location of protoparticles
  std::string m_input ;

private:

};
#endif // PARTICLEMAKERBASE
