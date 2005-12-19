// $Id: STAssociators_load.cpp,v 1.1.1.1 2005-12-19 15:42:42 mneedham Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare all the factories
DECLARE_FACTORY_ENTRIES( STAssociators ) {

  DECLARE_TOOL( STSelectAllDigits );
  DECLARE_TOOL( STSelectFromCluster );  

  DECLARE_ALGORITHM( STCluster2MCParticleLinker);
  DECLARE_ALGORITHM( STCluster2MCHitLinker );
  DECLARE_ALGORITHM( STDigit2MCParticleLinker );
  DECLARE_ALGORITHM( STDigit2MCHitLinker );

}

