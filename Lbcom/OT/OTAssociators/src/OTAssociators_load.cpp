// $Id: OTAssociators_load.cpp,v 1.1.1.1 2002-05-26 22:48:42 jvantilb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "OTAssociators/OTCluster2MCHitAsct.h"
#include "OTAssociators/OTCluster2MCParticleAsct.h"

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

  DECLARE_ALGORITHM( OTCluster2MCHitAlg );
  DECLARE_ALGORITHM( OTCluster2MCParticleAlg );

}
