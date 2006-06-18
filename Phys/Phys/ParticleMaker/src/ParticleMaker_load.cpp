// $Id:
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( ParticleMaker ) {

  // Particle Makers
  DECLARE_TOOL( CombinedParticleMaker );
  DECLARE_TOOL( NoPIDsParticleMaker );


  /// @todo Restore photon particle makers
  //  DECLARE_TOOL( PhotonFromMergedParticleMaker );
  //  DECLARE_TOOL( PhotonParticleMaker    ) ;
  //  DECLARE_TOOL( CnvPhotonParticleMaker ) ;

  DECLARE_TOOL( ParticleMakerSeq );     // Sequencer of Particle makers

  DECLARE_ALGORITHM( PreLoadParticles );

  /// @todo Restore photon parameters tools
  //  DECLARE_TOOL( PhotonParameters ) ;
  //  DECLARE_TOOL( PhotonFromMergedParameters );

  DECLARE_TOOL( ParticleStuffer );

}
