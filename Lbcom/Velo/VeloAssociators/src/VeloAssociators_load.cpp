// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Relations/RelationMACROs.h"

// local
#include "VeloAssociators/VeloCluster2MCHitAsct.h"
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"

// Tool factories
static const ToolFactory<VeloCluster2MCParticleAsct> s_VeloCluster2MCParticleAsctFactory; 
const       IToolFactory& VeloCluster2MCParticleAsctFactory = s_VeloCluster2MCParticleAsctFactory;

static const ToolFactory<VeloCluster2MCHitAsct> s_VeloCluster2MCHitAsctFactory; 
const       IToolFactory& VeloCluster2MCHitAsctFactory = s_VeloCluster2MCHitAsctFactory;

// Relation factories
IMPLEMENT_RelationW2D( VeloCluster , MCParticle , double );
IMPLEMENT_RelationW2D( VeloCluster , MCVeloHit , double );


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( VeloAssociators ) {

  DECLARE_TOOL( VeloCluster2MCParticleAsct );  
  DECLARE_TOOL( VeloCluster2MCHitAsct ); 

  DECLARE_RelationW2D( VeloCluster , MCParticle , double );
  DECLARE_RelationW2D( VeloCluster , MCVeloHit , double );

  DECLARE_ALGORITHM( VeloCluster2MCParticleAlg );
  DECLARE_ALGORITHM( VeloCluster2MCHitAlg );
  DECLARE_ALGORITHM( VeloCluster2MCHitLinker );

}


