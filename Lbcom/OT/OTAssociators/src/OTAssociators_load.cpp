// $Id: OTAssociators_load.cpp,v 1.10 2006-01-31 13:31:05 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( OTAssociators ) 
{
  DECLARE_ALGORITHM( OTTime2MCHitLinker );
  DECLARE_ALGORITHM( OTTime2MCDepositLinker );
  DECLARE_ALGORITHM( OTTime2MCParticleLinker );
}
