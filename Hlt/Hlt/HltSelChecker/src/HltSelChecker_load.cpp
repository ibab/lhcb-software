
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( HltSelChecker ) {

  // Algorithms
  DECLARE_ALGORITHM( DecayChainNTuple );
  DECLARE_ALGORITHM( CheckHLTGeneric );
}

