// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "VeloAssociators/VeloCluster2MCHitAsct.h"
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"

static const ToolFactory<VeloCluster2MCParticleAsct> s_VeloCluster2MCParticleAsctFactory; 
const       IToolFactory& VeloCluster2MCParticleAsctFactory = s_VeloCluster2MCParticleAsctFactory;

static const ToolFactory<VeloCluster2MCHitAsct> s_VeloCluster2MCHitAsctFactory; 
const       IToolFactory& VeloCluster2MCHitAsctFactory = s_VeloCluster2MCHitAsctFactory;


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( VeloAssociators ) {

  //  DECLARE_TOOL( VeloCluster2MCParticleAsct );  
  // DECLARE_TOOL( VeloCluster2MCHitAsct ); 

  DECLARE_ALGORITHM( VeloCluster2MCParticleAlg );
  DECLARE_ALGORITHM( VeloCluster2MCHitAlg );

}


