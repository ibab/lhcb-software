// $Id: OTAssociators_load.cpp,v 1.5 2003-07-15 11:31:06 jvantilb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Relations/RelationMACROs.h"

// local
#include "OTAssociators/OTCluster2MCDepositAsct.h"
#include "OTAssociators/OTCluster2MCHitAsct.h"
#include "OTAssociators/OTCluster2MCParticleAsct.h"
#include "OTAssociators/OTDigit2MCDepositAsct.h"
#include "OTAssociators/OTDigit2MCHitAsct.h"
#include "OTAssociators/OTDigit2MCParticleAsct.h"


// Implement the relation factories
IMPLEMENT_Relation1D( OTCluster , MCParticle );
IMPLEMENT_Relation1D( OTCluster , MCHit );
IMPLEMENT_Relation1D( OTCluster , MCOTDeposit );
IMPLEMENT_RelationW1D( OTDigit , MCParticle, int );
IMPLEMENT_RelationW1D( OTDigit , MCHit, int );
IMPLEMENT_RelationW1D( OTDigit , MCOTDeposit, int );

// Implement the tool factories
static const ToolFactory<OTCluster2MCHitAsct>    s_OTCluster2MCHitAsctFactory;
const IToolFactory& OTCluster2MCHitAsctFactory = s_OTCluster2MCHitAsctFactory;
static const ToolFactory<OTCluster2MCParticleAsct>    
                              s_OTCluster2MCParticleAsctFactory;
const IToolFactory& OTCluster2MCParticleAsctFactory = 
                              s_OTCluster2MCParticleAsctFactory;
static const ToolFactory<OTCluster2MCDepositAsct> 
                              s_OTCluster2MCDepositAsctFactory;
const IToolFactory& OTCluster2MCDepositAsctFactory = 
                              s_OTCluster2MCDepositAsctFactory;
static const ToolFactory<OTDigit2MCHitAsct>    s_OTDigit2MCHitAsctFactory;
const IToolFactory& OTDigit2MCHitAsctFactory = s_OTDigit2MCHitAsctFactory;
static const ToolFactory<OTDigit2MCParticleAsct> 
                              s_OTDigit2MCParticleAsctFactory;
const IToolFactory& OTDigit2MCParticleAsctFactory = 
                              s_OTDigit2MCParticleAsctFactory;
static const ToolFactory<OTDigit2MCDepositAsct> 
                              s_OTDigit2MCDepositAsctFactory;
const IToolFactory& OTDigit2MCDepositAsctFactory = 
                              s_OTDigit2MCDepositAsctFactory;

DECLARE_FACTORY_ENTRIES( OTAssociators ) 
{
  DECLARE_TOOL( OTCluster2MCHitAsct );
  DECLARE_TOOL( OTCluster2MCParticleAsct );
  DECLARE_TOOL( OTCluster2MCDepositAsct );
  DECLARE_TOOL( OTDigit2MCHitAsct );
  DECLARE_TOOL( OTDigit2MCParticleAsct );
  DECLARE_TOOL( OTDigit2MCDepositAsct );

  DECLARE_Relation1D( OTCluster , MCParticle );
  DECLARE_Relation1D( OTCluster , MCHit );
  DECLARE_Relation1D( OTCluster , MCOTDeposit );
  DECLARE_RelationW1D( OTDigit , MCParticle, int );
  DECLARE_RelationW1D( OTDigit , MCHit, int );
  DECLARE_RelationW1D( OTDigit , MCOTDeposit, int );

  DECLARE_ALGORITHM( OTCluster2MCHitAlg );
  DECLARE_ALGORITHM( OTCluster2MCParticleAlg );
  DECLARE_ALGORITHM( OTCluster2MCDepositAlg );
  DECLARE_ALGORITHM( OTDigit2MCHitAlg );
  DECLARE_ALGORITHM( OTDigit2MCParticleAlg );
  DECLARE_ALGORITHM( OTDigit2MCDepositAlg );

}
