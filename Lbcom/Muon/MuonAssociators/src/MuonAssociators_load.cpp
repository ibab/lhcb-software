// $Id: MuonAssociators_load.cpp,v 1.1.1.1 2002-06-28 09:59:33 dhcroft Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Relations/RelationMACROs.h"

// local
#include "MuonAssociators/MuonDigit2MCParticleAsct.h"
#include "MuonAssociators/MuonCoord2MCParticleAsct.h"


// Implement the relation factories
IMPLEMENT_Relation1D( MuonDigit , MCParticle );
IMPLEMENT_Relation1D( MuonCoord , MCParticle );

// Implement the tool factories
static const ToolFactory<MuonDigit2MCParticleAsct>    
                              s_MuonDigit2MCParticleAsctFactory;
const IToolFactory& MuonDigit2MCParticleAsctFactory = 
                              s_MuonDigit2MCParticleAsctFactory;

static const ToolFactory<MuonCoord2MCParticleAsct>    
                              s_MuonCoord2MCParticleAsctFactory;
const IToolFactory& MuonCoord2MCParticleAsctFactory = 
                              s_MuonCoord2MCParticleAsctFactory;


DECLARE_FACTORY_ENTRIES( MuonAssociators ) 
{
  DECLARE_TOOL( MuonDigit2MCParticleAsct );
  DECLARE_TOOL( MuonCoord2MCParticleAsct );

  DECLARE_Relation1D( MuonDigit , MCParticle );
  DECLARE_Relation1D( MuonCoord , MCParticle );

  DECLARE_ALGORITHM( MuonDigit2MCParticleAlg );
  DECLARE_ALGORITHM( MuonCoord2MCParticleAlg );

}
