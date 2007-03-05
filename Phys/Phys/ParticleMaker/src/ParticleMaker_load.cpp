
//=========================================================================
/** @file ParticleMaker_load.cpp
 *
 *  Declaration of objects in the component library ParticleMaker
 *
 *  CVS Log :-
 *  $Id: ParticleMaker_load.cpp,v 1.7 2007-03-05 10:41:56 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//=========================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( ParticleMaker ) 
{

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
  //DECLARE_TOOL( PhotonFromMergedParameters );

  DECLARE_TOOL( ParticleStuffer );

}
