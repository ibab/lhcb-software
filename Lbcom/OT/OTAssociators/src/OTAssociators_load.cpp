// $Id: OTAssociators_load.cpp,v 1.2 2002-05-27 11:38:34 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Relations/RelationMACROs.h"

// local
#include "OTAssociators/OTCluster2MCHitAsct.h"
#include "OTAssociators/OTCluster2MCParticleAsct.h"


// Implement the relation factories
IMPLEMENT_Relation1D( OTCluster , MCParticle );
IMPLEMENT_Relation1D( OTCluster , MCHit );

// Implement the tool factories
static const ToolFactory<OTCluster2MCHitAsct>    s_OTCluster2MCHitAsctFactory;
const IToolFactory& OTCluster2MCHitAsctFactory = s_OTCluster2MCHitAsctFactory;

static const ToolFactory<OTCluster2MCParticleAsct>    
                              s_OTCluster2MCParticleAsctFactory;
const IToolFactory& OTCluster2MCParticleAsctFactory = 
                              s_OTCluster2MCParticleAsctFactory;


DECLARE_FACTORY_ENTRIES( OTAssociators ) 
{
  DECLARE_TOOL( OTCluster2MCHitAsct );
  DECLARE_TOOL( OTCluster2MCParticleAsct );

  DECLARE_Relation1D( OTCluster , MCParticle );
  DECLARE_Relation1D( OTCluster , MCHit );

  DECLARE_ALGORITHM( OTCluster2MCHitAlg );
  DECLARE_ALGORITHM( OTCluster2MCParticleAlg );

}
