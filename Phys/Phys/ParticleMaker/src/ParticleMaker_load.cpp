// $Id:
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( ParticleMaker ) {

  // Particle Makers
  DECLARE_TOOL( CombinedParticleMaker );
  DECLARE_TOOL( NoPIDsParticleMaker );


  DECLARE_TOOL( CaloParticleMaker    ) ;
  DECLARE_TOOL( PhotonMaker          ) ;
  DECLARE_TOOL( MergedPi0Maker       ) ;
  DECLARE_TOOL( ResolvedPi0Maker     ) ;

  DECLARE_TOOL( ParticleMakerSeq );     // Sequencer of Particle makers

  DECLARE_ALGORITHM( PreLoadParticles );

  /// @todo Restore photon parameters tools
  //DECLARE_TOOL( PhotonParameters ) ;
  //  DECLARE_TOOL( PhotonFromMergedParameters );

  DECLARE_TOOL( ParticleStuffer );

}
