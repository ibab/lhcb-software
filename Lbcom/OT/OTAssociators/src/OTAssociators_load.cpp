// $Id: OTAssociators_load.cpp,v 1.9 2006-01-30 13:34:33 janos Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( OTAssociators ) 
{
  DECLARE_ALGORITHM( OTTime2MCHitLinker );
  DECLARE_ALGORITHM( OTTime2MCDepositLinker );
  DECLARE_ALGORITHM( OTTime2MCParticleLinker );
  //DECLARE_ALGORITHM( TestOTTime2MCHitRel );

}
