// $Id: DaVinciMCTools_load.cpp,v 1.3 2002-11-13 16:29:36 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(DaVinciMCTools) {

  DECLARE_TOOL( DebugTool );
  DECLARE_TOOL( MCDecayFinder );
  DECLARE_TOOL( MCParticleMaker );

  DECLARE_ALGORITHM( MCPIDProtoPAlg );
  
}
