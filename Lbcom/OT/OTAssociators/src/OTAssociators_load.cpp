// $Id: OTAssociators_load.cpp,v 1.3 2002-07-04 11:20:27 hierck Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Relations/RelationMACROs.h"

// local
#include "OTAssociators/OTCluster2MCHitAsct.h"
#include "OTAssociators/OTCluster2MCParticleAsct.h"
#include "OTAssociators/OTCluster2MCDepositAsct.h"


// Implement the relation factories
IMPLEMENT_Relation1D( OTCluster , MCParticle );
IMPLEMENT_Relation1D( OTCluster , MCHit );
IMPLEMENT_Relation1D( OTCluster , MCOTDeposit );

// Implement the tool factories
static const ToolFactory<OTCluster2MCHitAsct>    s_OTCluster2MCHitAsctFactory;
const IToolFactory& OTCluster2MCHitAsctFactory = s_OTCluster2MCHitAsctFactory;

static const ToolFactory<OTCluster2MCParticleAsct>    
                              s_OTCluster2MCParticleAsctFactory;
const IToolFactory& OTCluster2MCParticleAsctFactory = 
                              s_OTCluster2MCParticleAsctFactory;

// Implement the tool factories
static const ToolFactory<OTCluster2MCDepositAsct>    s_OTCluster2MCDepositAsctFactory;
const IToolFactory& OTCluster2MCDepositAsctFactory = s_OTCluster2MCDepositAsctFactory;

DECLARE_FACTORY_ENTRIES( OTAssociators ) 
{
  DECLARE_TOOL( OTCluster2MCHitAsct );
  DECLARE_TOOL( OTCluster2MCParticleAsct );
  DECLARE_TOOL( OTCluster2MCDepositAsct );

  DECLARE_Relation1D( OTCluster , MCParticle );
  DECLARE_Relation1D( OTCluster , MCHit );
  DECLARE_Relation1D( OTCluster , MCOTDeposit );

  DECLARE_ALGORITHM( OTCluster2MCHitAlg );
  DECLARE_ALGORITHM( OTCluster2MCParticleAlg );
  DECLARE_ALGORITHM( OTCluster2MCDepositAlg );

}
