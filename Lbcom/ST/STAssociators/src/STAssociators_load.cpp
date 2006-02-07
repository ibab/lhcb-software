// $Id: STAssociators_load.cpp,v 1.2 2006-02-07 08:46:39 mneedham Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare all the factories
DECLARE_FACTORY_ENTRIES( STAssociators ) {


  DECLARE_ALGORITHM( STCluster2MCParticleLinker);
  DECLARE_ALGORITHM( STCluster2MCHitLinker );
  DECLARE_ALGORITHM( STDigit2MCParticleLinker );
  DECLARE_ALGORITHM( STDigit2MCHitLinker );

}

