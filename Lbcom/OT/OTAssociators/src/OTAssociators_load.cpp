// $Id: OTAssociators_load.cpp,v 1.6 2004-09-03 12:08:10 jnardull Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Relations/RelationMACROs.h"

// local
#include "OTAssociators/OTTime2MCDepositAsct.h"
#include "OTAssociators/OTTime2MCHitAsct.h"
#include "OTAssociators/OTTime2MCParticleAsct.h"

// Implement the relation factories
IMPLEMENT_Relation1D( OTTime , MCParticle );
IMPLEMENT_Relation1D( OTTime , MCHit );
IMPLEMENT_Relation1D( OTTime , MCOTDeposit );

// Implement the tool factories
static const ToolFactory<OTTime2MCHitAsct>    s_OTTime2MCHitAsctFactory;
const IToolFactory& OTTime2MCHitAsctFactory = s_OTTime2MCHitAsctFactory;
static const ToolFactory<OTTime2MCParticleAsct>    
                              s_OTTime2MCParticleAsctFactory;
const IToolFactory& OTTime2MCParticleAsctFactory = 
                              s_OTTime2MCParticleAsctFactory;
static const ToolFactory<OTTime2MCDepositAsct> 
                              s_OTTime2MCDepositAsctFactory;
const IToolFactory& OTTime2MCDepositAsctFactory = 
                              s_OTTime2MCDepositAsctFactory;

DECLARE_FACTORY_ENTRIES( OTAssociators ) 
{
  DECLARE_TOOL( OTTime2MCHitAsct );
  DECLARE_TOOL( OTTime2MCParticleAsct );
  DECLARE_TOOL( OTTime2MCDepositAsct );

  DECLARE_Relation1D( OTTime , MCParticle );
  DECLARE_Relation1D( OTTime , MCHit );
  DECLARE_Relation1D( OTTime , MCOTDeposit );

  DECLARE_ALGORITHM( OTTime2MCHitAlg );
  DECLARE_ALGORITHM( OTTime2MCParticleAlg );
  DECLARE_ALGORITHM( OTTime2MCDepositAlg );

}
