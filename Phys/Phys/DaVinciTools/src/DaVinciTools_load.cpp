// $Id: DaVinciTools_load.cpp,v 1.1 2002-02-08 18:25:42 gcorti Exp $
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

  DECLARE_TOOL( AddFourMom );
  DECLARE_TOOL( AxPart2MCParticleAsct );
  DECLARE_TOOL( ImpactPar );
  DECLARE_TOOL( MCUtilityTool );

}
