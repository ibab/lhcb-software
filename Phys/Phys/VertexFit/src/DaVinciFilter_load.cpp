// $Id: 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Associator tool
#include "Particle2VertexAsct.h"
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

DECLARE_TOOL_FACTORY( Particle2VertexAsct);
DECLARE_DATAOBJECT_FACTORY( Particle2VertexTable);

DECLARE_FACTORY_ENTRIES( DaVinciFilter ) {

  // Filter
  DECLARE_TOOL( ParticleFilter );

  DECLARE_TOOL( BooleanFilterCriterion );
  DECLARE_TOOL( FlightDistanceFilterCriterion );
  DECLARE_TOOL( KinFilterCriterion );
  DECLARE_TOOL( LifetimeSignificanceCriterion );
  DECLARE_TOOL( MassDifferenceFilterCriterion );
  DECLARE_TOOL( MassFilterCriterion );
  DECLARE_TOOL( Momentum2FlightAngleFilterCriterion ) ;
  DECLARE_TOOL( PIDFilterCriterion );
  DECLARE_TOOL( PVIPFilterCriterion );
  DECLARE_TOOL( TrackTypeFilterCriterion );
  DECLARE_TOOL( VtxFilterCriterion );

  // Filter n particles
  DECLARE_TOOL( KinBestParticles );

  DECLARE_ALGORITHM( FilterByBestParticles );

  DECLARE_ALGORITHM( Particle2VertexWithIPS );
  DECLARE_DATAOBJECT( Particle2VertexTable );
  DECLARE_TOOL( Particle2VertexAsct );

}
