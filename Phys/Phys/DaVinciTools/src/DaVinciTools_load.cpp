// $Id: DaVinciTools_load.cpp,v 1.2 2002-03-27 20:34:59 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciTools ) {

  DECLARE_TOOL( AxPart2MCParticleAsct );
  DECLARE_TOOL( MCUtilityTool );
  DECLARE_TOOL( PhysDesktop );
  DECLARE_TOOL( ParticleTransporter );
  DECLARE_TOOL( LagrangeMassVertexFitter );
  DECLARE_TOOL( UnconstVertexFitter );  
  DECLARE_TOOL( GeomDispCalculator );
  DECLARE_TOOL( PIDFilterCriterion );
  DECLARE_TOOL( KinFilterCriterion );
  DECLARE_TOOL( ParticleFilter );
  DECLARE_TOOL( ParticleStuffer );

  DECLARE_ALGORITHM( FillParticleAlgorithm );
  
}
