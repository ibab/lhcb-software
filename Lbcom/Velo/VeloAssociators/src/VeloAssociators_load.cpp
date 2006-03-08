// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( VeloAssociators ) {

  DECLARE_ALGORITHM( InternalVeloCluster2MCHitLinker );
  DECLARE_ALGORITHM( VeloDigit2MCHitLinker );
  DECLARE_ALGORITHM( VeloDigit2MCParticleLinker );

  DECLARE_TOOL( VeloCluster2VeloDigitTool );

}


