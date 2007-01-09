// $Id: STAssociators_load.cpp,v 1.3 2007-01-09 15:04:59 jvantilb Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare all the factories
DECLARE_FACTORY_ENTRIES( STAssociators ) {

  DECLARE_ALGORITHM( STCluster2MCParticleLinker);
  DECLARE_ALGORITHM( STCluster2MCHitLinker );
  DECLARE_ALGORITHM( STDigit2MCParticleLinker );
  DECLARE_ALGORITHM( STDigit2MCHitLinker );

}
