// $Id: DaVinciTools_load.cpp,v 1.75 2007-03-21 13:02:40 pkoppenb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciTools ) {

  // Miscellaneous
  DECLARE_TOOL( Algorithm2ID ) ;
  DECLARE_TOOL( CheckOverlap );
  DECLARE_TOOL( CheckVeloOverlap );
  DECLARE_TOOL( ContextTool );
  DECLARE_TOOL( DecayFinder );     
  DECLARE_TOOL( DecodeSimpleDecayString );
  DECLARE_TOOL( GeomDispCalculator ) ;
  DECLARE_TOOL( NeutralCCChangePIDTool ) ;
  DECLARE_TOOL( OnOfflineTool ) ;
  DECLARE_TOOL( ParticleDescendants ) ;
  DECLARE_TOOL( Particle2State ) ;
  DECLARE_TOOL( Particle2VertexIPSAsct ) ;
  DECLARE_TOOL( PhysDesktop );
  DECLARE_TOOL( RecursivePlotTool ) ;
  DECLARE_TOOL( RelatedPV ) ;
  DECLARE_TOOL( RelatedPVFinder ) ;
  DECLARE_TOOL( SimplePlotTool ) ;
  DECLARE_TOOL( TrgDispCalculator ) ;
  DECLARE_TOOL( BremAdder ) ;
  DECLARE_TOOL( CaloElectron ) ;

  // Algorithms
  DECLARE_ALGORITHM( CheckPV );  
  DECLARE_ALGORITHM( CheckSelResult );
  DECLARE_ALGORITHM( ConjugateNeutralPID );
  DECLARE_ALGORITHM( MakeResonances ) ;
  DECLARE_ALGORITHM( Particle2VertexAsctAlg );  
  DECLARE_ALGORITHM( SimplePlots );  
  DECLARE_ALGORITHM( SelResultsCorrelations );

}

