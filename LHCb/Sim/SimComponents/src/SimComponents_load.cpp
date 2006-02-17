// $Id: SimComponents_load.cpp,v 1.2 2006-02-17 17:38:43 cattanem Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(SimComponents) {

  DECLARE_ALGORITHM( DumpMCEventAlg );
  DECLARE_ALGORITHM( EvtTypeChecker );
  DECLARE_ALGORITHM( MCHitMonitor );

  DECLARE_ALGORITHM( PackMCParticle );
  DECLARE_ALGORITHM( UnpackMCParticle );
  DECLARE_ALGORITHM( CompareMCParticle );

  DECLARE_ALGORITHM( PackMCVertex );
  DECLARE_ALGORITHM( UnpackMCVertex );
  DECLARE_ALGORITHM( CompareMCVertex );

  DECLARE_SERVICE( EvtTypeSvc );

  DECLARE_TOOL( MCDecayFinder );
   
}
