// $Id:
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( ParticleMaker ) {

  // Particle Makers
  DECLARE_TOOL( CnvPhotonParticleMaker ) ;  
  DECLARE_TOOL( CombinedParticleMaker ); // "the official one"
  DECLARE_TOOL( NoPIDsParticleMaker );
  DECLARE_TOOL( PhotonFromMergedParticleMaker );
  DECLARE_TOOL( PhotonParticleMaker    ) ;  

  DECLARE_TOOL( ParticleMakerSeq );     // Sequencer of Particle makers

  DECLARE_ALGORITHM( PreLoadParticles );

  DECLARE_TOOL( PhotonParameters ) ;  
  DECLARE_TOOL( PhotonFromMergedParameters );
 
  DECLARE_TOOL( ParticleStuffer );

}
