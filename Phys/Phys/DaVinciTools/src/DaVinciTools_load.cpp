// $Id: DaVinciTools_load.cpp,v 1.36 2004-07-16 16:12:11 pkoppenb Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Associator tool
#include "Particle2VertexAsct.h"
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

DECLARE_TOOL_FACTORY( Particle2VertexAsct);
DECLARE_DATAOBJECT_FACTORY( Particle2VertexTable);

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciTools ) {

  DECLARE_TOOL( PhysDesktop );

  // Particle Makers
  //  DECLARE_TOOL( ChargedParticleMaker ); // obsolete
  //  DECLARE_TOOL( ElectronParticleMaker ); // obsolete
  //  DECLARE_TOOL( MuonParticleMaker ); // obsolete
  //  DECLARE_TOOL( NeutralParticleMaker ); // obsolete
  //  DECLARE_TOOL( RichParticleMaker ); // obsolete
  DECLARE_TOOL( CombinedParticleMaker ); // "the official one"
  DECLARE_TOOL( PhotonParticleMaker    ) ;  
  DECLARE_TOOL( CnvPhotonParticleMaker ) ;  
  DECLARE_TOOL( PhotonFromMergedParticleMaker );
  DECLARE_TOOL( NoPIDsParticleMaker );
  DECLARE_TOOL( ParticleMakerSeq );     // Sequencer of Particle makers

  DECLARE_TOOL( ParticleStuffer );

  // Filter
  DECLARE_TOOL( ParticleFilter );

  DECLARE_TOOL( BooleanFilterCriterion );
  DECLARE_TOOL( FlightDistanceFilterCriterion );
  DECLARE_TOOL( KinFilterCriterion );
  DECLARE_TOOL( LifetimeSignificanceCriterion );
  DECLARE_TOOL( MassDifferenceFilterCriterion );
  DECLARE_TOOL( MassFilterCriterion );
  DECLARE_TOOL( PIDFilterCriterion );
  DECLARE_TOOL( PVIPFilterCriterion );
  DECLARE_TOOL( TrackTypeFilterCriterion );
  DECLARE_TOOL( VtxFilterCriterion );

  // Filter n particles
  DECLARE_TOOL( KinBestParticles );

  // Fitters
  DECLARE_TOOL( LagrangeDirectionFitter );
  DECLARE_TOOL( LagrangeMassVertexFitter );
  DECLARE_TOOL( UnconstVertexFitter );  
  DECLARE_TOOL( LifetimeFitter );
  DECLARE_TOOL( GeomDispCalculator );

  // Transporters
  DECLARE_TOOL( CombinedTransporter );
  DECLARE_TOOL( LinearTransporter );
  DECLARE_TOOL( NsctVeloTransporter );
  DECLARE_TOOL( ParabolicTransporter );
  DECLARE_TOOL( TrTransporter );

  // Miscellaneous
  DECLARE_TOOL( PhotonParameters       ) ;  
  DECLARE_TOOL( PhotonFromMergedParameters );
  DECLARE_TOOL( DecodeSimpleDecayString );
  DECLARE_TOOL( DecayFinder );
  DECLARE_TOOL( CheckOverlap );
  DECLARE_TOOL( Particle2VertexAsct );

  DECLARE_DATAOBJECT( Particle2VertexTable );

  // Algorithms
  DECLARE_ALGORITHM( Particle2VertexWithIPS );
  DECLARE_ALGORITHM( PreLoadParticles );
  DECLARE_ALGORITHM( PreDV );
  DECLARE_ALGORITHM( ChangePID );
  DECLARE_ALGORITHM( FilterByBestParticles );
  
}

