// $Id: ParticleMakerBase.h,v 1.5 2010-08-16 16:40:38 odescham Exp $
#ifndef PARTICLEMAKERBASE_H 
#define PARTICLEMAKERBASE_H 1

// from STL
#include <string>

// from DaVinci
#include "Kernel/DaVinciAlgorithm.h"
// PartProp
#include "Kernel/ParticleProperty.h" 
// Brem adder
#include "Kernel/IBremAdder.h"

/** @class ParticleMakerBase ParticleMakerBase.h
 *
 *  Base class for all Particle Maker Algorithms
 *
 *  @author P. Koppenburg
 *  @date   2009-04-21
 */
class ParticleMakerBase : public DaVinciAlgorithm
{

public:

  /// Standard constructor
  ParticleMakerBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleMakerBase( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

  virtual StatusCode initialize() ; ///< Finalize

protected:

  /// The method that each implementation should implement
  virtual StatusCode makeParticles(LHCb::Particle::Vector& ) = 0 ;

  /// protoparticles
  const LHCb::ProtoParticle::ConstVector& protos() const
  {
    return m_protos;
  }
  
  // BremStrahlung correction for electron
  void addBrem( LHCb::Particle* particle );

  /// Convert a string to Upper case
  std::string to_upper( const std::string& in )
  {
    std::string out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  }
  
private:

  /// Avoid loading Particles etc.
  virtual StatusCode loadEventInput();

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

  // list of PIDs for which BremStrahlung correction is activated
  std::vector<std::string> m_addBremPhoton;

  IBremAdder* bremAdder(){return m_brem;}

private:

  /// Local ProtoParticle container.
  LHCb::ProtoParticle::ConstVector m_protos;

  /// Track selector tool
  IBremAdder* m_brem;

};
#endif // PARTICLEMAKERBASE
