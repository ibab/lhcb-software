// $Id: DaVinciTools_load.cpp,v 1.8 2002-09-11 16:47:09 gcorti Exp $
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

  DECLARE_TOOL( PhysDesktop );
  DECLARE_TOOL( ChargedParticleMaker );
  DECLARE_TOOL( MuonParticleMaker );
  DECLARE_TOOL( NeutralParticleMaker );
  DECLARE_TOOL( LagrangeMassVertexFitter );
  DECLARE_TOOL( UnconstVertexFitter );  
  DECLARE_TOOL( GeomDispCalculator );
  DECLARE_TOOL( PIDFilterCriterion );
  DECLARE_TOOL( KinFilterCriterion );
  DECLARE_TOOL( ParticleFilter );
  DECLARE_TOOL( ParticleStuffer );
  DECLARE_TOOL( CombinedTransporter );
  DECLARE_TOOL( LinearTransporter );
  DECLARE_TOOL( NsctVeloTransporter );
  DECLARE_TOOL( ParabolicTransporter );
  DECLARE_TOOL( DecayFinder );
  //DECLARE_TOOL( DecayTreeTool );
  
}
